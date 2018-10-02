class PlatformHelper {
    public:
        static void openFileInOS(std::string cpURL) {
            std::string command = "open " + cpURL;
            system(command.c_str());
        }

        static void openUrlInBrowser(std::string cpURL) {
            PlatformHelper::openFileInOS(cpURL);
        }
};