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
    bool isTerminateServer;
    bool isTerminate;
    
    bool isConnected;
    string username;
    vector<string> channels; 
    map<string, vector<Event>> eventsMap;
    vector<string> reciepts;

    std::shared_ptr<ConnectionHandler> hendlerPtr;

    //handeling protocol:
    //key bord:
    void logingHendel(vector<string>& lineCommands, std::promise<std::shared_ptr<ConnectionHandler>>& promiseHendler);
    void joinHendel(vector<string>& lineCommands,std::promise<std::shared_ptr<ConnectionHandler>>& promiseHendler );
    void exitHendel(vector<string>& lineCommands, std::promise<std::shared_ptr<ConnectionHandler>>& promiseHendler);
    void reportHendel(vector<string>& lineCommands, std::promise<std::shared_ptr<ConnectionHandler>>& promiseHendler);
    void logoutHendel(std::promise<std::shared_ptr<ConnectionHandler>>& promiseHendler);
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
    std::string epochToDate(time_t epoch);

public:
    StompProtocol();
    ~StompProtocol();
    //server:
    void processServer(StompFrame& frame);
    bool shouldTerminateServer();
    //Keybord:
    void processKeybord(string& line, std::promise<std::shared_ptr<ConnectionHandler>>& promiseHendler);
    bool shouldTerminateKeybord();

};
