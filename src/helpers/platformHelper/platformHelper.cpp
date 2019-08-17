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
bool PlatformHelper::fileExists(const QString &outputFileName) {
    QFileInfo confP(QString::fromStdString(outputFileName));
    return confP.exists();
}

/* STD PATHS */

QString PlatformHelper::getAppDirectory() {
    return QCoreApplication::applicationDirPath();

}
QString PlatformHelper::getDataStorageDirectory() {
    return PlatformHelper::prepareStandardPath(QStandardPaths::AppLocalDataLocation);
}

QString PlatformHelper::prepareStandardPath(QStandardPaths::StandardLocation pathType) {
    auto path = QStandardPaths::writableLocation(pathType);
    QDir destDir(path.toUtf8());
    if (!destDir.exists()) destDir.mkpath(".");
    return path;
}