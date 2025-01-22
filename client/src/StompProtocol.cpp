#include "../include/StompProtocol.h"


StompProtocol::StompProtocol(){
    isTerminate = false;
    eventMap = new ;
}
StompProtocol::~StompProtocol(){}

//server:
    void StompProtocol::processServer(StompFrame& frame){

    }

    bool StompProtocol::shouldTerminateServer(){

    }

    void StompProtocol::receiptHendel(StompFrame frame){

    }

    string StompProtocol::erorHendl(StompFrame frame){

    }
    string StompProtocol::messageHendel(StompFrame frame){

    }
    
//Keybord:
    void StompProtocol::processKeybord(string& line){

    }
    bool StompProtocol::shouldTerminateKeybord(){

    }

    StompFrame StompProtocol::logingHendel(vector<string>& lineCommands){

    }

    StompFrame StompProtocol::joinHendel(vector<string>& lineCommands){

    }
    StompFrame StompProtocol::exitHendel(vector<string>& lineCommands){

    }
    StompFrame StompProtocol::reportHendel(vector<string>& lineCommands){

    }
    StompFrame StompProtocol::logoutHendel(){

    }

    string StompProtocol::summaryHendel(vector<string>& lineCommands){
        
    }