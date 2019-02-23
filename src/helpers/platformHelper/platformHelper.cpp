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
    boost::filesystem::path confP(outputFileName);
    confP = boost::filesystem::absolute(confP);
    return boost::filesystem::exists(confP);
}
