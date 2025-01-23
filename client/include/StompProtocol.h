#pragma once

#include "../include/ConnectionHandler.h"
#include "../include/StompFrame.h"
#include "../include/event.h"
#include <string> 
#include <iostream>

using namespace std;

// TODO: implement the STOMP protocol
class StompProtocol
{
private:
    //Fildes:
    bool isTerminateServer;
    bool isTerminate;
    
    bool isConnected;
    string username;
    vector<string> channels; 
    map<string, vector<Event>> eventsMap;
    vector<string> reciepts;

    //handeling protocol:
    //key bord:
    void logingHendel(vector<string>& lineCommands, ConnectionHandler* &handlerPtr);
    void joinHendel(vector<string>& lineCommands,ConnectionHandler* &handlerPtr );
    void exitHendel(vector<string>& lineCommands, ConnectionHandler* &handlerPtr);
    void reportHendel(vector<string>& lineCommands, ConnectionHandler* &handlerPtr);
    void logoutHendel(ConnectionHandler* &handlerPtr);
    void summaryHendel(vector<string>& lineCommands);

    //server:
    void receiptHendel(StompFrame frame);
    void erorHendl(StompFrame frame);
    void messageHendel(StompFrame frame);
    void connectedtHendel(StompFrame frame);


    //other:
    vector<string> split(const string &str, char delim);
    int generateSubscriptionId();
    int findIndex(vector<string>& v, string val);
    std::string epochToDate(time_t epoch) {




public:
    StompProtocol();
    ~StompProtocol();
    //server:
    void processServer(StompFrame& frame);
    bool shouldTerminateServer();
    //Keybord:
    void processKeybord(string& line, ConnectionHandler* &handlerPtr);
    bool shouldTerminateKeybord();

};
