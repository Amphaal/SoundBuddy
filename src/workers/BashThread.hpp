#pragma once

#include <QThread>
#include <QObject>
#include <QDebug>

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
        // latest file path of a file we want to stream
        QString _wantedFilePath;

        // latest output
        QString _outputFilePath;

        ///
        AVFormatContext* _inputContext = nullptr;
        AVFormatContext* _outputContext = nullptr;
        AVStream* _in_stream = nullptr;
        AVStream* _out_stream = nullptr;
        const AVCodec* _codec = nullptr;
        AVCodecContext* _codecContext = nullptr;
        AVDictionary *_muxerOptions = nullptr;

        bool _hasAwaitingNewFileToStream = false;

        static const QString _getPathToDashTempFolder() {
            return "C:\\Users\\havea\\Desktop\\test\\";
        }

        //
        void _doStreamNewerFile() {
            _hasAwaitingNewFileToStream = false;

            if (avformat_open_input(&_inputContext, _wantedFilePath.toStdString().c_str(), nullptr, nullptr) != 0) {
                emit errorOccurred("Error: Couldn't open input file.");
                return;
            }

            Defer deferred;
            deferred.defer([&] { avformat_free_context(_inputContext); });

            //
            avformat_alloc_output_context2(&_outputContext, nullptr, "dash", (_getPathToDashTempFolder() + "manifest.mpd").toStdString().c_str()); //_outputFilePath.toStdString().c_str());
            if (!_outputContext) {
                emit errorOccurred("Error: Couldn't create output context for DASH.");
                return;
            }

            deferred.defer([&] {
                if (!(_outputContext->oformat->flags & AVFMT_NOFILE)) {
                    avio_closep(&_outputContext->pb);
                }
                
                avformat_free_context(_outputContext); 
            });

            _in_stream = _inputContext->streams[0];
            _out_stream = avformat_new_stream(_outputContext, _codec);

            avcodec_parameters_copy(_out_stream->codecpar, _in_stream->codecpar);

            _out_stream->time_base = (AVRational){ 1, _codecContext->sample_rate };
            avcodec_parameters_from_context(_out_stream->codecpar, _codecContext);

            if (!(_outputContext->oformat->flags & AVFMT_NOFILE)) {
                if (avio_open(&_outputContext->pb, _outputFilePath.toStdString().c_str(), AVIO_FLAG_WRITE) < 0) {
                    emit errorOccurred("Error: Couldn't open DASH output file.");
                    return;
                }

                deferred.defer([&] (){ avio_close(_outputContext->pb); });
            }

            // Apply muxer options to the format context
            _outputContext->flags |= AVFMT_FLAG_CUSTOM_IO;
            if(avformat_write_header(_outputContext, &_muxerOptions) < 0) {
                emit errorOccurred("Error: Couldn't write header.");
                return;
            };
            
            deferred.defer([&] { av_write_trailer(_outputContext); });

            AVPacket packet;
            while (!_hasAwaitingNewFileToStream && av_read_frame(_inputContext, &packet) >= 0) {
                av_interleaved_write_frame(_outputContext, &packet);
                av_packet_unref(&packet);
            }
        }

    protected:
        void run() override {
            // Set the segment filename option
            av_dict_set(&_muxerOptions, "dash_segment_filename", (_getPathToDashTempFolder() + "segment_$Number$.m4s").toStdString().c_str(), 0);

            // You can also set other DASH muxer options as needed
            // For example, segment duration in seconds
            av_dict_set(&_muxerOptions, "seg_duration", "5", 0);

            // Find the encoder for the audio stream (e.g., AAC)
            _codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
            if (!_codec) {
                emit errorOccurred("Codec not found\n");
                return;
            }

            Defer deferred;
            _codecContext = avcodec_alloc_context3(_codec);
            if (!_codecContext) {
                emit errorOccurred("Could not allocate codec context\n");
                return;
            }
            deferred.defer([&] { avcodec_free_context(&_codecContext); });

            av_channel_layout_default(&_codecContext->ch_layout, 2);
            deferred.defer([&] { av_channel_layout_uninit(&_codecContext->ch_layout); });

            // Set codec parameters, including the sample rate
            _codecContext->sample_rate = 44100;  // Set the desired sample rate
            _codecContext->bit_rate = 128000;    // Set the bitrate
            _codecContext->sample_fmt = _codec->sample_fmts[0];  // Set the sample format

            if (avcodec_open2(_codecContext, _codec, nullptr) < 0) {
                emit errorOccurred("Could not open codec\n");
                return;
            }
            deferred.defer([&] { avcodec_close(_codecContext); });

            QObject::connect(
                this, &BashThread::_hasNewFileToRead,
                this, &BashThread::_doStreamNewerFile
            );

            this->exec();


        }
};