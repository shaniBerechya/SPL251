#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include "../include/StompProtocol.h" 
#include "../include/ConnectionHandler.h" 


int main(int argc, char *argv[]) {
	ConnectionHandler* handlerPtr = nullptr;
	StompProtocol protocol;  

    // Start the keyboard listening thread
    std::thread keyboardThread(handleKeyboard, std::ref(protocol), handlerPtr);

    // Start the server listening thread
    std::thread serverThread(handleServer, std::ref(protocol), handlerPtr);

    keyboardThread.join();

    // After quitting, ensure the server thread also terminates
    serverThread.join();

    return 0;
}

// Function to handle keyboard input
void handleKeyboard(StompProtocol& protocol, ConnectionHandler* handlerPtr) {
    std::string line;
    while (!protocol.shouldTerminateKeybord()) {
        std::getline(std::cin, line);
        if (line != "") {
            protocol.processKeybord(line, handlerPtr) ;
        }
    }
}

// Function to handle server responses
void handleServer(StompProtocol& protocol,ConnectionHandler* handlerPtr) {
    if(handlerPtr != nullptr){
		while (!protocol.shouldTerminateServer()) {
			string frameStr;
			handlerPtr->getFrameAscii(frameStr, '\u0000');
			StompFrame frame; //TODO
			protocol.processServer(frame);
		}	
        handlerPtr->close();
	}
	
}



