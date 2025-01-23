#include "../include/StompProtocol.h"
#include "../include/StompFrame.h"
#include "../include/event.h"
#include <algorithm> 
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <sstream> // For converting numbers to strings if needed
#include <json.hpp>
#include <fstream>
using json = nlohmann::json;
using namespace std;




StompProtocol::StompProtocol() : isTerminate(false), isConnected(false), username("") {}

StompProtocol::~StompProtocol(){}

//server:
    void StompProtocol::processServer(StompFrame& frame){
        std::string command = frame.getCommand();

        // Check the command type of the frame and handle accordingly
        if (command == "CONNECTED") {
            // Handle server's response to a CONNECT command
            connectedtHendel(frame);
        } else if (command == "MESSAGE") {
            // Handle a MESSAGE command which may contain actual data or notifications
            messageHendel(frame);
        } else if (command == "RECEIPT") {
            // Handle a RECEIPT command, which acknowledges actions previously sent to the server
            receiptHendel(frame);
        } else if (command == "ERROR") {
            // Handle an ERROR command, which reports an error from the server
            erorHendl(frame);
        } else {
            // Log an error or handle unexpected commands
            std::cerr << "Received an unsupported command: " << command << std::endl;
        }
    }

    bool StompProtocol::shouldTerminateServer(){
        return isTerminate;
    }

    void StompProtocol::connectedtHendel(StompFrame frame){
        std::cout << "Login successful" << std::endl;
    }

    void StompProtocol::receiptHendel(StompFrame frame){
        int recieptID = stoi(frame.getHeaderByName("receipt-id")); 
        string toPrint = reciepts[recieptID];
        if(toPrint == "Logged out"){
            isConnected = false;  // Set the client's connection status to false
            isTerminateServer = true;
        }
        std::cout << toPrint << std::endl;
    }

    void StompProtocol::erorHendl(StompFrame frame){
        if(frame.getHeaderByName("erorMessage") == "User already logged in"){
            std::cout << "login failed" << std::endl;
        }
        else if(frame.getHeaderByName("erorMessage") == "Wrong password"){
            std::cout << "Wrong password" << std::endl;
        }
        std::cout << "ERROR FROM THE SERVER: \n" + frame.toString()<< std::endl;
        isTerminateServer = true;
        isTerminate = true;
    }

    void StompProtocol::messageHendel(StompFrame frame) {
        // Extracting channel name from the frame header
        string channelName = frame.getHeaderByName("destination");

        // Parsing the body to extract the sender's name and other event details
        string body = frame.getFrameBody();
        Event event(frame.getFrameBody());
  
        string sender = event.get_eventOwnerUser();

       // Update the event map
        eventsMap[channelName].push_back(event);

        std::cout << "Processed message from " << sender << " in channel " << channelName << std::endl;
    }
    
//Keybord:
    void StompProtocol::processKeybord(string& line,ConnectionHandler* &handlerPtr) {
        vector<string> lineCommands = split(line, ' '); // A utility function split needs to be defined or included before.
        string command = lineCommands[0];

        if (command == "login") {
            // login {host:port} {username} {password}
            logingHendel(lineCommands, handlerPtr);
            // Further code to send this frame to the server
        } else if (command == "join") {
            // join {channel_name}
            joinHendel(lineCommands, handlerPtr);
            // Further code to send this frame to the server
        } else if (command == "exit") {
            // exit {channel_name}
            exitHendel(lineCommands, handlerPtr);
            // Further code to send this frame to the server
        } else if (command == "report", handlerPtr) {
            // report {file_name}
            reportHendel(lineCommands, handlerPtr);
            // Further code to send this frame to the server
        } else if (command == "logout") {
            // logout
            logoutHendel(handlerPtr);
            // Further code to send this frame to the server
        } else if(command == "summary") {
            summaryHendel(lineCommands);
        }
        else {
           std::cout << "Unknown command" << std::endl;
        }
    }
 
    bool StompProtocol::shouldTerminateKeybord(){
        return isTerminate;
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

    void StompProtocol::logingHendel(vector<string>& lineCommands,ConnectionHandler* &handlerPtr ) {
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

        handlerPtr = new ConnectionHandler(host, portShort);//TODO needs to be dalted

         if (!handlerPtr->connect()) {
            std::cerr << "Cannot connect to server" << std::endl;
         }
         else{
             // Username and password
            string userName = lineCommands[2];
            string password = lineCommands[3];

            // Building the STOMP frame for CONNECT
            StompFrame frame;
            frame.setCommand("CONNECT");
            frame.setHeadersByParts("accept-version", "1.2");
            frame.setHeadersByParts("host", host); // Assuming the host includes virtual host functionality
            frame.setHeadersByParts("login", userName);
            frame.setHeadersByParts("passcode", password);

            //sending:
            handlerPtr->sendFrameAscii(frame.toString(), '\u0000');
            isConnected = true;
            username = userName;
         }

       
    }

    void StompProtocol::joinHendel(vector<string>& lineCommands, ConnectionHandler* &handlerPtr){
        if (lineCommands.size() != 2) {
            std::cout << "Usage: join {channel_name}" << std::endl;
        }

        // Extract the channel name from the input
        string channelName = lineCommands[1];

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
        handlerPtr->sendFrameAscii(frame.toString(), '\u0000');
        channels[subId] = (channelName);
        reciepts[receiptId] =("Joined channel " + lineCommands[1]);
    }

    // Function definition to create a unique identifier for each subscription
    int StompProtocol::generateSubscriptionId() {
        static int subscriptionId = 0;
        return ++subscriptionId; // Returns a unique identifier by incrementing on each call
    }


    void StompProtocol::exitHendel(vector<string>& lineCommands, ConnectionHandler* &handlerPtr){
         string channelName = lineCommands[1];
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
        handlerPtr->sendFrameAscii(frame.toString(), '\u0000');

    }

    void StompProtocol::reportHendel(vector<string>& lineCommands, ConnectionHandler* &handlerPtr){
        if (lineCommands.size() != 2) {
            cout << "Usage: report {file_path}" << endl;
            return; // Return an empty frame or handle the error appropriately
        }

        string filePath = lineCommands[1];
        names_and_events namesAndEvents = parseEventsFile(filePath);
        
        //adding the senders to all the events:
        //and create the massege to send
        for(Event event : namesAndEvents.events){
            event.setEventOwnerUser(username);

            //create the frame:
            StompFrame frame;
            frame.setCommand("SEND");
            frame.setHeadersByParts("destination", namesAndEvents.channel_name);

            frame.setFrameBody(event.getBodyFromEvent());

            // Sending the frame 
            handlerPtr->sendFrameAscii(frame.toString(), '\u0000');
        }

    }

    void StompProtocol::logoutHendel(ConnectionHandler* &handlerPtr){
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
        handlerPtr->sendFrameAscii(frame.toString(), '\u0000');

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
        return -1;
    }