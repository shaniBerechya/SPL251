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
			StompFrame frame = parseStompFrame(frameStr);
			protocol.processServer(frame);
		}	
        handlerPtr->close();
	}
}

StompFrame parseStompFrame(const std::string& rawFrame) {
    StompFrame frame;
    
    // Split the raw frame into header/command part and body part
    size_t sizeStart = rawFrame.find("\n\n");
    std::string headersAndCommand = rawFrame.substr(0, sizeStart);
    std::string body;
    
    // Check if there is a body part
    if (sizeStart != std::string::npos && sizeStart + 2 < rawFrame.length()) {
        body = rawFrame.substr(sizeStart + 2);
    }
    
    // Split headers and command
    std::istringstream iss(headersAndCommand);
    std::string line;
    
    // First line is the command
    std::getline(iss, line);
    frame.setCommand(line);
    
    // Process remaining header lines
    while (std::getline(iss, line)) {
        size_t colonPos = line.find(':');
        std::string name = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);
        frame.setHeadersByParts(name,value);
    }
    
    // Set body if exists, trimmed
    if (!body.empty()) {
        // Trim leading and trailing whitespace
        body.erase(0, body.find_first_not_of(" \t\n\r"));
        body.erase(body.find_last_not_of(" \t\n\r") + 1);
        frame.setFrameBody(body);
    }
    
    return frame;
}



