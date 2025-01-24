#include "../include/StompProtocol.h" 
#include "../include/ConnectionHandler.h"

class StompClient
{
public:
    static void handleKeyboard(StompProtocol& protocol, ConnectionHandler& hendler);  
    static void handleServer(StompProtocol& protocol,ConnectionHandler& hendler);
    static StompFrame parseStompFrame(const std::string& rawFrame);
};
