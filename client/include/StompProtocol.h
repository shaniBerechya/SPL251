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
    map<string,Event> eventMap;

    //handeling protocol:
    //key bord:
    StompFrame logingHendel(vector<string>& lineCommands);
    StompFrame joinHendel(vector<string>& lineCommands);
    StompFrame exitHendel(vector<string>& lineCommands);
    StompFrame reportHendel(vector<string>& lineCommands);
    StompFrame logoutHendel();

    string summaryHendel(vector<string>& lineCommands);

    //server:
    void receiptHendel(StompFrame frame);
    string erorHendl(StompFrame frame);
    string messageHendel(StompFrame frame);

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
