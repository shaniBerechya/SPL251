#include "../include/StompProtocol.h"
#include "../include/StompFrame.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <sstream> // For converting numbers to strings if needed




StompProtocol::StompProtocol() : isTerminate(false), isConnected(false){}

StompProtocol::~StompProtocol(){}

//server:
    void StompProtocol::processServer(StompFrame& frame){

    }

    bool StompProtocol::shouldTerminateServer(){

    }

    void StompProtocol::connectedtHendel(StompFrame frame){
        std::cout << "Login successful" << std::endl;
    }

    void StompProtocol::receiptHendel(StompFrame frame){
        int recieptID = stoi(frame.getHeaderByName("receipt-id")); 
        string toPrint = reciepts[recieptID];
        if(toPrint == "Logged out"){
            //TODO
             isConnected = false;  // Set the client's connection status to false

        }
        std::cout << toPrint << std::endl;

    }

    string StompProtocol::erorHendl(StompFrame frame){
        if(frame.getHeaderByName("erorMessage") == "User already logged in"){
            std::cout << "User already logged in" << std::endl;
        }
        else if(frame.getHeaderByName("erorMessage") == "Wrong password"){
            std::cout << "Wrong password" << std::endl;
        }

    }
    string StompProtocol::messageHendel(StompFrame frame){

    }
    
//Keybord:
    void StompProtocol::processKeybord(string& line) {
        vector<string> lineCommands = split(line, ' '); // A utility function split needs to be defined or included before.
        string command = lineCommands[0];

        if (command == "login") {
            // login {host:port} {username} {password}
            logingHendel(lineCommands);
            // Further code to send this frame to the server
        } else if (command == "join") {
            // join {channel_name}
            joinHendel(lineCommands);
            // Further code to send this frame to the server
        } else if (command == "exit") {
            // exit {channel_name}
            exitHendel(lineCommands);
            // Further code to send this frame to the server
        } else if (command == "report") {
            // report {file_name}
            reportHendel(lineCommands);
            // Further code to send this frame to the server
        } else if (command == "logout") {
            // logout
            logoutHendel();
            // Further code to send this frame to the server
        } else {
           std::cout << "Unknown command" << std::endl;
        }
    }
    std::vector<std::string> StompProtocol::split(const std::string& s, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }


    bool StompProtocol::shouldTerminateKeybord(){

    }

    void StompProtocol::logingHendel(vector<string>& lineCommands) {
        if (lineCommands.size() != 4) {
            std::cout << "Usage: login {host:port} {username} {password}" << std::endl;
        }

        if(isConnected){
            std::cout << "The client is already logged in, log out before trying again" << std::endl;
        }

        // Extracting the host and port
        string hostPort = lineCommands[1];
        size_t colonPos = hostPort.find(':');
        if (colonPos == string::npos) {
            std::cout << "Error: Host and port must be specified as host:port" << std::endl;
        }

        string host = hostPort.substr(0, colonPos);
        string portStr =  hostPort.substr(colonPos + 1);

        //casting to short
        int portInt = std::stoi(portStr);  
        short portShort = static_cast<short>(portInt);  

        ConnectionHandler connectionHandlerToServer(host, portShort);

         if (!connectionHandlerToServer.connect()) {
            std::cerr << "Cannot connect to server" << std::endl;
         }
         else{
             // Username and password
            string username = lineCommands[2];
            string password = lineCommands[3];

            // Building the STOMP frame for CONNECT
            StompFrame frame;
            frame.setCommand("CONNECT");
            frame.setHeadersByParts("accept-version", "1.2");
            frame.setHeadersByParts("host", host); // Assuming the host includes virtual host functionality
            frame.setHeadersByParts("login", username);
            frame.setHeadersByParts("passcode", password);

            //sending:
            connectionHandlerToServer.sendFrameAscii(frame.toString(), '\u0000');
            isConnected = true;
         }

       
    }
    void StompProtocol::joinHendel(vector<string>& lineCommands){
        if (lineCommands.size() != 2) {
            std::cout << "Usage: join {channel_name}" << std::endl;
        }

        // Extract the channel name from the input
        string channelName = "/"+ lineCommands[1];

        // Generate a subscription ID and a receipt ID
        int subId = generateSubscriptionId();
        int receiptId = generateSubscriptionId(); // Assumes that the same ID generation can be used for other purposes

        // Build the SUBSCRIBE frame
        StompFrame frame;
        frame.setCommand("SUBSCRIBE");
        frame.setHeadersByParts("destination", channelName);
        frame.setHeadersByParts("id", std::to_string(subId));
        frame.setHeadersByParts("receipt", std::to_string(receiptId));

         //sending and update data:
        connectionHandlerToServer.sendFrameAscii(frame.toString(), '\u0000');
        channels[subId] = (channelName);
        reciepts[receiptId] =("Joined channel " + lineCommands[1]);


    }

    // Function definition to create a unique identifier for each subscription
    int StompProtocol::generateSubscriptionId() {
        static int subscriptionId = 0;
        return ++subscriptionId; // Returns a unique identifier by incrementing on each call
    }


    void StompProtocol::exitHendel(vector<string>& lineCommands){
         string channelName = "/"+ lineCommands[1];
        // Check if the user is subscribed to the channel
        if (count(channels.begin(), channels.end(), channelName) == 0) {
            std::cerr << "you are not subscribed to channel " + lineCommands[1] << std::endl;
        }
          
        int subId = findIndex(channels, channelName); // Get the subscription ID for the channel
        int receiptId = generateSubscriptionId();

        StompFrame frame;
        frame.setCommand("UNSUBSCRIBE");
        frame.setHeadersByParts("id", std::to_string(subId)); // Add 'id' header with the subscription ID
        frame.setHeadersByParts("receipt", std::to_string(receiptId));
        
        //add in to reciepts
        reciepts[receiptId] = "Exited channel " + lineCommands[1];
        
        // Once the user exits the channel, remove the subscription from the map
        channels.erase(channels.begin()+subId);

        //sending the frame
        connectionHandlerToServer.sendFrameAscii(frame.toString(), '\u0000');

    }

    void StompProtocol::reportHendel(vector<string>& lineCommands){
       
    }

    void StompProtocol::logoutHendel(){
        if (!isConnected) {
            std::cout << "please login first" << std::endl;
        }

        // Build the DISCONNECT frame
        StompFrame frame;
        frame.setCommand("DISCONNECT");
        int receiptId = generateSubscriptionId(); // Assuming generateSubscriptionId provides a unique ID that can be used here
        frame.setHeadersByParts("receipt", std::to_string(receiptId));

        // Map the receipt to a specific message to confirm logout upon receipt handling
        reciepts[receiptId] = "Logged out";

        //sending the frame
        connectionHandlerToServer.sendFrameAscii(frame.toString(), '\u0000');

    }

    void StompProtocol::summaryHendel(vector<string>& lineCommands){
        
    }

    int StompProtocol::findIndex(vector<string>& v, string val) {
    for (int i = 0; i < v.size(); i++) {
      
          // When the element is found
        if (v[i] == val) {
            return i;
        }
    }