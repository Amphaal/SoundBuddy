#ifdef __APPLE__
    #include "platformHelper.mac.cpp"
#endif
#ifdef _WIN32
    #include "platformHelper.win.cpp"
#endif

bool PlatformHelper::isLaunchingAtStartup() {
    auto settings = this->getStartupSettingsHandler();
    return this->getPathToApp() == this->getPathToAppFromStartupSettings(settings); //compare paths
}