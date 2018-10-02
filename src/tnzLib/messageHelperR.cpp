#include <functional>

class MessageHelperR {
    public:
    MessageHelperR(std::function<void(std::string)> messageHelperFunc = NULL) : 
        callMessageHelper(std::move(messageHelperFunc)) {}

    protected:
        typedef std::function<void(std::string)> MessageHelper;
        MessageHelper callMessageHelper;
}