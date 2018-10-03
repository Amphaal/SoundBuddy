#include <functional>

class IMessageHelper {
    public:
    IMessageHelper(std::function<void(std::string)> messageHelperFunc = NULL) : 
        callMessageHelper(std::move(messageHelperFunc)) {}

    protected:
        typedef std::function<void(std::string)> MessageHelper;
        MessageHelper callMessageHelper;
}