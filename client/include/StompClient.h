#include "../include/StompProtocol.h" 
#include "../include/ConnectionHandler.h"

class StompClient
{
public:
    static void handleKeyboard(StompProtocol& protocol, ConnectionHandler* handlerPtr);  
    static void handleServer(StompProtocol& protocol,ConnectionHandler* handlerPtr);
    static StompFrame parseStompFrame(const std::string& rawFrame);
};
