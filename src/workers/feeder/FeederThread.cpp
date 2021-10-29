#include "FeederThread.h"

#include "src/_i18n/trad.hpp"

FeederThread::FeederThread(const UploadHelper* uploder, const AppSettings::ConnectivityInfos connectivityInfos) : ITNZThread(uploder, connectivityInfos) {}

void FeederThread::run() {
    //
    emit printLog(tr("WARNING ! Make sure you activated the XML file sharing in %1 > Preferences > Advanced.").arg(musicAppName()));

    try {
        this->_generateLibJSONFile();
        this->_uploadLibToServer();
    } catch (const std::exception& e) {
        // emit error as log
        emit printLog(
            QString(e.what()),
            false,
            true
        );
    }
}

// generate files
void FeederThread::_generateLibJSONFile() {
    auto itnzLibPath = this->_getMusicAppLibLocation();
    this->_processFile(itnzLibPath);

    // check warnings
    auto warningsCount = this->_libWarningsAsJSON->MemberCount();
    if (warningsCount) {
        // create warning file
        emit printLog(
            tr("WARNING ! %1 files in your library are missing important "
               "metadata and consequently were removed from the output file ! "
               "Please check the \"%2\" file for more informations.")
                .arg(warningsCount)
                .arg(OUTPUT_FILE_PATH));

        _tracksUnmolding(WARNINGS_FILE_PATH);
        this->_ohWrn->writeAsJsonFile(*this->_libWarningsAsJSON, true);
    } else {
        // remove old warning file
        auto pToRem = this->_ohWrn->getOutputPath();
        QFile::remove(pToRem);
    }

    _tracksUnmolding(OUTPUT_FILE_PATH);

    this->_ohLib->writeAsJsonFile(*this->_libAsJSON);

    emit printLog(tr("OK, output file is ready for breakfast !"));

    emit operationFinished();
}

void FeederThread::_tracksUnmolding(const char* filename) {
    emit printLog(tr("Unmolding \"%1\"...").arg(filename));
}

// upload
void FeederThread::_uploadLibToServer() {
    emit printLog(tr("Let's try to send now !"));

    this->_uploder->uploadDataToPlatform(

    );
    
    QString response = this->_ohLib->uploadFile();

    if (!response.isEmpty()) {
        emit printLog(tr("Server responded: %1").arg(response));
    } else {
        emit printLog(tr("No feedback from the server ? Strange... Please check the targeted host."));
    }
}


void FeederThread::_processFile(const QString &xmlFileLocation) {
    this->_recCount = 0;
    this->_expectedCount = 0;

    this->_libWarningsAsJSON = new QJsonDocument;
    this->_workingJSON = new QJsonDocument;
    this->_libAsJSON = new QJsonDocument;

    // set default
    this->_libWarningsAsJSON->Parse("{}");
    this->_workingJSON->Parse("{}");
    this->_libAsJSON->Parse("[]");

    // format to dict
    this->_generateJSON(xmlFileLocation);
    this->_standardizeJSON();
}

// navigate through XML and generate object
void FeederThread::_generateJSON(const QString &xmlFileLocation) {
    //
    emit printLog(tr("Pre-digesting XML file..."));

    const auto XMLReadErr = tr("Cannot read the XML file bound to your library. Are you sure you activated the XML file sharing in %1 ?").arg(musicAppName()).toStdString();

    // read xml as QString
    MusicAppLibParser* doc;
    try {
        doc = new MusicAppLibParser(xmlFileLocation);
    } catch(...) {
        throw std::logic_error(XMLReadErr);
    }
    auto xmlAsJSONString = doc->ToJSON();
    delete doc;

    // try parse to temp JSON
    QJsonDocument d;
    d.Parse(xmlAsJSONString.toUtf8());
    if (d.HasParseError()) {
        throw std::logic_error(XMLReadErr);
    }

    emit printLog(tr("Triming fat..."));

    // retrieve tracks and pass to workingJSON
    auto v = rapidjson::Pointer("/Tracks").Get(d);
    this->_expectedCount = v->MemberCount();
    if (!this->_expectedCount) {
        throw std::logic_error(
            tr("No music found in your %1 library. Please feed it some.")
            .arg(musicAppName())
            .toStdString()
        );
    }
    this->_workingJSON->CopyFrom(d["Tracks"], this->_workingJSON->GetAllocator());
}

///
/// Other Helpers
///

// log...
void FeederThread::_tracksEmitHelper() {
    bool mustReplacePrevious = this->_recCount;
    this->_recCount++;
    bool canLog = ((this->_recCount % 100) == 0 && this->_recCount <= this->_expectedCount) || this->_recCount == this->_expectedCount || !mustReplacePrevious;
    if (canLog) {
        emit printLog(tr("%1 over %2 ...").arg(this->_recCount).arg(this->_expectedCount), mustReplacePrevious);
    }
}

// seek in Music App preference file the library location
QString FeederThread::_getMusicAppLibLocation() {
    emit printLog(tr("Getting XML file location..."));
    try {
        return PlatformHelper::getMusicAppLibLocation();
    } catch(...) {
        throw std::logic_error(tr("An issue happened while fetching %1's XML file location. Have you installed %1 ?").arg(musicAppName()).toStdString());
    }
}
