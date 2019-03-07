#ifdef __APPLE__
    #include "platformHelper.mac.cpp"
#endif
#ifdef _WIN32
    #include "platformHelper.win.cpp"
#endif

bool PlatformHelper::isLaunchingAtStartup() {
    auto settings = PlatformHelper::getStartupSettingsHandler();
    return PlatformHelper::getPathToApp() == PlatformHelper::getPathToAppFromStartupSettings(settings); //compare paths
}

//ensure a file exists
bool PlatformHelper::fileExists(std::string outputFileName) {
    filesystem::path confP(outputFileName);
    confP = confP.make_absolute();
    return confP.exists();
}

/* STD PATHS */

std::string PlatformHelper::getAppDirectory() {
    return QCoreApplication::applicationDirPath().toStdString();

}
std::string PlatformHelper::getDataStorageDirectory() {
    return PlatformHelper::prepareStandardPath(QStandardPaths::AppLocalDataLocation);
}

std::string PlatformHelper::prepareStandardPath(QStandardPaths::StandardLocation pathType) {
    auto path = QStandardPaths::writableLocation(pathType).toStdString();
    QDir destDir(path.c_str());
    if (!destDir.exists()) destDir.mkpath(".");
    return path;
}