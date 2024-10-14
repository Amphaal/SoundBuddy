#pragma once

#include <QThread>
#include <QObject>
#include <QDebug>
#include <QMutex>

#include <src/helpers/Defer.hpp>

extern "C" {

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavfilter/avfilter.h>

}

class BashThread : public QThread {
    Q_OBJECT

    public:
        BashThread(QObject* parent = nullptr)
            : QThread(parent) {}

        void doStreamNewFile(const QString& pathToFileToStream) {
            _wantedFilePath = pathToFileToStream;
            _hasAwaitingNewFileToStream = true;
            emit _hasNewFileToRead(QPrivateSignal());
        }

        void quit() {
            _hasAwaitingNewFileToStream = true; // exits loop of packet reading
            QThread::quit();
        }

    signals:
        void errorOccurred(const QString& error);
        void _hasNewFileToRead(QPrivateSignal);

    private:
        QMutex _m;

        // latest file path of a file we want to stream
        QString _wantedFilePath;

        // latest output
        QString _outputFilePath;

        ///
        const AVCodec* _inputCodec = nullptr; // codec found that could read latest file
        AVCodecContext* _inputCodecContext = nullptr;
        AVFormatContext* _inputContext = nullptr;

        const AVCodec* _outputCodec = nullptr;
        AVCodecContext* _outputCodecContext = nullptr;
        AVFormatContext* _outputContext = nullptr;
        AVStream* _out_stream = nullptr;

        AVDictionary* _outputOptions = nullptr;

        bool _hasAwaitingNewFileToStream = false;

        static const QString _getPathToDashTempFolder() {
            return "C:\\Users\\havea\\Desktop\\test\\";
        }

        //
        void _doStreamNewerFile() {
            //
            // LOCK THREAD UNTIL FILE STREAMED
            //

            QMutexLocker lock(&_m);
            Defer deferred;
            _hasAwaitingNewFileToStream = false;

            //
            // OPEN FILE and read metadata (important !)
            //

            qDebug() << _wantedFilePath.toStdString().c_str();

            if (avformat_open_input(&_inputContext, _wantedFilePath.toStdString().c_str(), nullptr, nullptr) != 0) {
                emit errorOccurred("Error: Couldn't open input file.");
                return;
            }
            deferred.defer([&] { 
                avformat_free_context(_inputContext);
                _inputContext = nullptr;
            });

            // Find the stream info
            if(avformat_find_stream_info(_inputContext, nullptr) < 0) {
                emit errorOccurred("Could not find stream info");
                return;
            }

            //
            // GET FILE CODEC & audio stream context
            //
            
            int audioStreamIndex = av_find_best_stream(_inputContext, AVMEDIA_TYPE_AUDIO, -1, -1, &_inputCodec, 0); // wont work without a previous avformat_find_stream_info
            if (_inputCodec == 0) {
                emit errorOccurred("No codec could be infered to read file");
                return;
            } else if (audioStreamIndex < 0 ) {
                emit errorOccurred("Could not find audio stream");
                return;
            }

            //
            // SETUP input codec context from file context
            //

            _inputCodecContext = avcodec_alloc_context3(_inputCodec);
            if (!_inputCodecContext) {
                emit errorOccurred("Could not allocate codec context\n");
                return;
            }
            deferred.defer([&] { 
                avcodec_free_context(&_inputCodecContext); 
                _inputCodecContext = nullptr;
            });

            if(avcodec_parameters_to_context(_inputCodecContext, _inputContext->streams[audioStreamIndex]->codecpar) < 0) {
                emit errorOccurred("Could not copy codec parameters to context");
                return;
            }

            //
            // CONFIGURE output stream and context
            //

            //
            avformat_alloc_output_context2(&_outputContext, nullptr, "dash", (_getPathToDashTempFolder() + "manifest.mpd").toStdString().c_str());
            if (!_outputContext) {
                emit errorOccurred("Error: Couldn't create output context for DASH.");
                return;
            }
            deferred.defer([&] { 
                avformat_free_context(_outputContext);
                _outputContext = nullptr;
            });

            //
            _out_stream = avformat_new_stream(_outputContext, nullptr);
            deferred.defer([&] { _out_stream = nullptr; }); // _out_stream would be cleared by avformat_free_context
            if (avcodec_parameters_from_context(_out_stream->codecpar, _outputCodecContext) < 0) {
                emit errorOccurred("Could not copy codec parameters to stream");
                return;
            }
            _out_stream->time_base = (AVRational){ 1, _outputCodecContext->sample_rate };

            //
            // OPEN DASH FILE
            //

            if (!(_outputContext->oformat->flags & AVFMT_NOFILE)) {
                if (avio_open(&_outputContext->pb, _outputFilePath.toStdString().c_str(), AVIO_FLAG_WRITE) < 0) {
                    emit errorOccurred("Error: Couldn't open DASH output file.");
                    return;
                }

                deferred.defer([&] (){ avio_close(_outputContext->pb); });
            }

            //
            // SETUP HEADER WITH OPTIONS
            //

            // Apply muxer options to the format context
            _outputContext->flags |= AVFMT_FLAG_CUSTOM_IO;
            av_dict_set(&_outputOptions, "media_seg_name", (_getPathToDashTempFolder() + "chunk-stream$RepresentationID$-$Number%05d$.$ext$").toStdString().c_str(), 0);
            av_dict_set(&_outputOptions, "init_seg_name", (_getPathToDashTempFolder() + "init-stream$RepresentationID$.$ext$").toStdString().c_str(), 0);
            if(avformat_write_header(_outputContext, &_outputOptions) < 0) {
                emit errorOccurred("Error: Couldn't write header.");
                return;
            };
            deferred.defer([&] { av_write_trailer(_outputContext); });

            AVPacket inputPacket = AVPacket {}; 
            AVPacket outputPacket = AVPacket {};
            deferred.defer([&] { 
                av_packet_unref(&inputPacket); 
                av_packet_unref(&outputPacket);
            });

            AVFrame frame = AVFrame {};
            deferred.defer([&] { 
                av_frame_unref(&frame);
            });

            //
            // Open input codec then feed it to muxer
            //

            if (avcodec_open2(_inputCodecContext, _inputCodec, nullptr) < 0) {
                emit errorOccurred("Could not open codec\n");
                return;
            }
            deferred.defer([&] { avcodec_close(_inputCodecContext); });

            // read input file frame... negative values means end of file
            while (!_hasAwaitingNewFileToStream && av_read_frame(_inputContext, &inputPacket) >= 0) {
                // skip reading data if not the right stream index
                if (inputPacket.stream_index != audioStreamIndex) {
                    continue;
                }

                // forward data to decoder, skip if failed
                if(avcodec_send_packet(_inputCodecContext, &inputPacket) < 0) {
                    continue;
                }

                // analyze frame
                while (avcodec_receive_frame(_inputCodecContext, &frame) >= 0) {
                    // skip if send failed
                    if(avcodec_send_frame(_outputCodecContext, &frame) < 0) {
                        continue;
                    }

                    if (avcodec_receive_packet(_outputCodecContext, &outputPacket) < 0) {
                        continue;
                    }

                    // Write the encoded packet to the output file
                    av_interleaved_write_frame(_outputContext, &outputPacket);
                }
            }
        }

    protected:
        void run() override {
            // Find the encoder for the audio stream (e.g., AAC)
            _outputCodec = avcodec_find_encoder(AV_CODEC_ID_AAC);
            if (!_outputCodec) {
                emit errorOccurred("Codec not found\n");
                return;
            }

            Defer deferred;
            _outputCodecContext = avcodec_alloc_context3(_outputCodec);
            if (!_outputCodecContext) {
                emit errorOccurred("Could not allocate codec context\n");
                return;
            }
            deferred.defer([&] { avcodec_free_context(&_outputCodecContext); });

            av_channel_layout_default(&_outputCodecContext->ch_layout, 2);
            deferred.defer([&] { av_channel_layout_uninit(&_outputCodecContext->ch_layout); });

            // Set codec parameters, including the sample rate
            _outputCodecContext->sample_rate = 44100;  // Set the desired sample rate
            _outputCodecContext->bit_rate = 128000;    // Set the bitrate
            _outputCodecContext->sample_fmt = _outputCodec->sample_fmts[0];  // Set the sample format

            if (avcodec_open2(_outputCodecContext, _outputCodec, nullptr) < 0) {
                emit errorOccurred("Could not open codec\n");
                return;
            }
            deferred.defer([&] { avcodec_close(_outputCodecContext); });

            //
            //
            //

            QObject::connect(
                this, &BashThread::_hasNewFileToRead,
                this, &BashThread::_doStreamNewerFile
            );

            this->exec();
        }
};