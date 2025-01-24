#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <memory>
#include <future>

#include "../include/StompClient.h" 


int main(int argc, char *argv[]) {
	
    ConnectionHandler hendler;
	StompProtocol protocol;  

    // Start the keyboard listening thread
    std::thread keyboardThread(StompClient::handleKeyboard, std::ref(protocol), std::ref(hendler));

    // Start the server listening thread
    std::thread serverThread(StompClient::handleServer, std::ref(protocol), std::ref(hendler));

    keyboardThread.join();

    // After quitting, ensure the server thread also terminates
    serverThread.join();

    return 0;
    
}

// Function to handle keyboard input
void StompClient::handleKeyboard(
    StompProtocol& protocol, ConnectionHandler& hendler)
     {
    std::string line;
    while (!protocol.shouldTerminateKeybord()) {
        std::getline(std::cin, line);
        if (line != "") {
            protocol.processKeybord(line, hendler) ;
        }
    }
}

// Function to handle server responses
void StompClient::handleServer(StompProtocol& protocol, ConnectionHandler& hendler){
    while (!protocol.shouldTerminateKeybord())
    {
        if (hendler.isConected()) {
            while(!protocol.shouldTerminateServer()){
                std::cout << "handleServer in client main" << std::endl;
                string frameStr;
                hendler.getFrameAscii(frameStr, '\u0000');
                StompFrame frame = parseStompFrame(frameStr);
                std::cout << "frame received from server: \n" << frameStr << std::endl;
                protocol.processServer(frame);
            }
            hendler.close();
        }
    }   
}

		


StompFrame StompClient::parseStompFrame(const std::string& rawFrame) {
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



