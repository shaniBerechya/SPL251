#pragma once

#include "../include/ConnectionHandler.h"
#include "../include/StompFrame.h"
#include "../include/event.h"
#include <string> 
#include <iostream>
#include <future>


using namespace std;

// TODO: implement the STOMP protocol
class StompProtocol
{
private:
    //Fildes:
    volatile bool isTerminateServer;
    volatile bool isTerminate;

    
    volatile bool isConnected;
    string username;
    map<string,string> channels; 
    map<string, vector<Event>> eventsMap;
    map<string,string> reciepts;

    //handeling protocol:
    //key bord:
    void logingHendel(vector<string>& lineCommands, ConnectionHandler& Hendler);
    void joinHendel(vector<string>& lineCommands,ConnectionHandler& Hendle );
    void exitHendel(vector<string>& lineCommands, ConnectionHandler& Hendle);
    void reportHendel(vector<string>& lineCommands, ConnectionHandler& Hendle);
    void logoutHendel(ConnectionHandler& Hendle);
    void summaryHendel(vector<string>& lineCommands);

    //server:
    void receiptHendel(StompFrame frame, ConnectionHandler& hendler);
    void erorHendl(StompFrame frame);
    void messageHendel(StompFrame frame);
    void connectedtHendel(StompFrame frame);


    //other:
    vector<string> split(const string &str, char delim);
    int generateSubscriptionId();
    int findIndex(vector<string>& v, string val);
    std::string epochToDate(time_t epoch);

public:
    StompProtocol();
    ~StompProtocol();
    //server:
    void processServer(StompFrame& frame, ConnectionHandler& hendler);
    bool shouldTerminateServer();
    //Keybord:
    void processKeybord(string& line, ConnectionHandler& Hendle);
    bool shouldTerminateKeybord();

};
