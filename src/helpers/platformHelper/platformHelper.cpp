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

//ensure a file exists
bool PlatformHelper::fileExists(std::string outputFileName) {
    boost::filesystem::path confP(outputFileName);
    confP = boost::filesystem::absolute(confP);
    return boost::filesystem::exists(confP);
}
