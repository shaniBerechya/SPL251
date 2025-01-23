#include "../include/StompProtocol.h" 
#include "../include/ConnectionHandler.h"

class StompClient
{
public:
    static void handleKeyboard(StompProtocol& protocol, std::promise<std::shared_ptr<ConnectionHandler>>& promiseHendler);  
    static void handleServer(StompProtocol& protocol,std::future<std::shared_ptr<ConnectionHandler>>& futureHendler);
    static StompFrame parseStompFrame(const std::string& rawFrame);
};
