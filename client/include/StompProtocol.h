#pragma once

#include "../include/ConnectionHandler.h"
#include "../include/StompFrame.h"
#include "../include/event.h"
#include <string> 
#include <iostream>

// TODO: implement the STOMP protocol
class StompProtocol
{
private:
    //Fildes:
    bool isTerminate;
    map<string,vector<Event>> eventMap;
    vector<string> channels; 
    vector<string> reciepts;
    bool isConnected;
    ConnectionHandler connectionHandlerToServer;

    //handeling protocol:
    //key bord:
    void logingHendel(vector<string>& lineCommands);
    void joinHendel(vector<string>& lineCommands);
    void exitHendel(vector<string>& lineCommands);
    void reportHendel(vector<string>& lineCommands);
    void logoutHendel();
    void summaryHendel(vector<string>& lineCommands);

    //server:
    void receiptHendel(StompFrame frame);
    string erorHendl(StompFrame frame);
    string messageHendel(StompFrame frame);
    void connectedtHendel(StompFrame frame);


    //other:
    vector<string> split(const string &str, char delim);
    int generateSubscriptionId();
    int findIndex(vector<string>& v, string val);



public:
    StompProtocol();
    ~StompProtocol();
    //server:
    void processServer(StompFrame& frame);
    bool shouldTerminateServer();
    //Keybord:
    void processKeybord(string& line);
    bool shouldTerminateKeybord();
};
