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




StompProtocol::StompProtocol():
isTerminateServer(false),isTerminate(false), isConnected(false),username(""),channels(),eventsMap(),reciepts() {}

StompProtocol::~StompProtocol(){}

//server:
    void StompProtocol::processServer(StompFrame& frame, ConnectionHandler& hendler){
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
            receiptHendel(frame, hendler);
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

    void StompProtocol::receiptHendel(StompFrame frame, ConnectionHandler& hendler){
        int recieptID = stoi(frame.getHeaderByName("receipt-id")); 
        string toPrint = reciepts[to_string(recieptID)];
        if(toPrint == "Logged out"){
            isConnected = false;  // Set the client's connection status to false
            isTerminateServer = true;
            hendler.close();
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
        isConnected = false;
    }

    void StompProtocol::messageHendel(StompFrame frame) {
        // Extracting channel name from the frame header
        string channelName = frame.getHeaderByName("destination");

        // Parsing the body to extract the sender's name and other event details
        string body = frame.getFrameBody();
        Event event(frame.getFrameBody());
  
        string sender = event.get_eventOwnerUser();

       // Update the event map and keep the list sorted
        std::vector<Event>& eventList = eventsMap[channelName];

        // Find the correct position to insert the new event
        auto it = std::lower_bound(eventList.begin(), eventList.end(), event, [](const Event& a, const Event& b) {
            if (a.get_date_time() != b.get_date_time()) {
                return a.get_date_time() < b.get_date_time(); // Sort by date first
            }
            return a.get_name() < b.get_name(); // Then lexicographically by name
        });

        // Insert the new event at the correct position
        eventList.insert(it, event);
    }
    
//Keybord:
    void StompProtocol::processKeybord(string& line,ConnectionHandler& hendler) {
        vector<string> lineCommands = split(line, ' '); // A utility function split needs to be defined or included before.
        string command = lineCommands[0];

        if (command == "login") {
            // login {host:port} {username} {password}
            logingHendel(lineCommands, hendler);
            // Further code to send this frame to the server
        }
        else if(isConnected){
            if (command == "join") {
            // join {channel_name}
            joinHendel(lineCommands, hendler);
            // Further code to send this frame to the server
            } else if (command == "exit") {
                // exit {channel_name}
                exitHendel(lineCommands, hendler);
                // Further code to send this frame to the server
            } else if (command == "report") {
                // report {file_name}
                reportHendel(lineCommands, hendler);
                // Further code to send this frame to the server
            } else if (command == "logout") {
                // logout
                logoutHendel(hendler);
                // Further code to send this frame to the server
            } else if(command == "summary") {
                summaryHendel(lineCommands);
            }
            else {
                std::cout << "Unknown command" << std::endl;   
            }
        }
        else{
            std::cout << "you have to login first" << std::endl;   
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

    void StompProtocol::logingHendel(vector<string>& lineCommands,ConnectionHandler& hendler ) {

        if (lineCommands.size() != 4) {
            std::cout << "Usage: login {host:port} {username} {password}" << std::endl;
            return;
        }

        if(isConnected){
            std::cout << "The client is already logged in, log out before trying again" << std::endl;
            return;
        }

        // Extracting the host and port
        string hostPort = lineCommands[1];
        size_t colonPos = hostPort.find(':');
        if (colonPos == string::npos) {
            std::cout << "Error: Host and port must be specified as host:port" << std::endl;
            return;
        }

        string host = hostPort.substr(0, colonPos);
        string portStr =  hostPort.substr(colonPos + 1);

        //casting to short
        int portInt = std::stoi(portStr);  
        short portShort = static_cast<short>(portInt);  


        hendler.set(host,portShort);

         if (!hendler.connect()) {
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
            frame.setHeadersByParts("host", "stomp.cs.bgu.ac.il"); // Assuming the host includes virtual host functionality
            frame.setHeadersByParts("login", userName);
            frame.setHeadersByParts("passcode", password);

            //sending:
            hendler.sendFrameAscii(frame.toString(), '\0');
            isConnected = true;
            username = userName;
         }

       
    }

    void StompProtocol::joinHendel(vector<string>& lineCommands, ConnectionHandler& hendler){
        if (lineCommands.size() != 2) {
            std::cout << "Usage: join {channel_name}" << std::endl;
            return;
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
        hendler.sendFrameAscii(frame.toString(), '\0');
        channels[channelName] = to_string(subId);
        reciepts[to_string(receiptId)] =("Joined channel " + lineCommands[1]);
    }

    // Function definition to create a unique identifier for each subscription
    int StompProtocol::generateSubscriptionId() {
        static int subscriptionId = 0;
        return ++subscriptionId; // Returns a unique identifier by incrementing on each call
    }


    void StompProtocol::exitHendel(vector<string>& lineCommands, ConnectionHandler& hendler){
         string channelName = lineCommands[1];
        // Check if the user is subscribed to the channel
        auto it = channels.find(channelName);
        if (it == channels.end()) {
            std::cerr << "you are not subscribed to channel " + channelName << std::endl;
            return;
        }
          
        string subId = it->second; // Get the subscription ID for the channel
        int receiptId = generateSubscriptionId();

        StompFrame frame;
        frame.setCommand("UNSUBSCRIBE");
        frame.setHeadersByParts("id",subId); // Add 'id' header with the subscription ID
        frame.setHeadersByParts("receipt", std::to_string(receiptId));
        
        //add in to reciepts
        reciepts[to_string(receiptId)] = "Exited channel " + lineCommands[1];
        
        // Once the user exits the channel, remove the subscription from the map
        channels.erase(channelName);

        //sending the frame
        hendler.sendFrameAscii(frame.toString(), '\0');

    }

    void StompProtocol::reportHendel(vector<string>& lineCommands, ConnectionHandler& hendler){
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
            hendler.sendFrameAscii(frame.toString(), '\0');
        }

    }

    void StompProtocol::logoutHendel(ConnectionHandler& hendler){
        if (!isConnected) {
            std::cout << "please login first" << std::endl;
            return;
        }

        // Build the DISCONNECT frame
        StompFrame frame;
        frame.setCommand("DISCONNECT");
        int receiptId = generateSubscriptionId(); // Assuming generateSubscriptionId provides a unique ID that can be used here
        frame.setHeadersByParts("receipt", std::to_string(receiptId));

        // Map the receipt to a specific message to confirm logout upon receipt handling
        reciepts[to_string(receiptId)] = "Logged out";

        //sending the frame
        hendler.sendFrameAscii(frame.toString(), '\0');

    }

   void StompProtocol::summaryHendel(std::vector<std::string>& lineCommands) {
        if (lineCommands.size() != 4) {
            std::cout << "Usage: summary {output_file_path}" << std::endl;
            return;
        }

        std::string outputFilePath = "./" + lineCommands[3];
        std::ofstream outputFile(outputFilePath);

        if (!outputFile.is_open()) {
            std::cerr << "Error: Could not open file " << outputFilePath << " for writing." << std::endl;
            return;
        } 

        std::string user = lineCommands[2];
        std::string channelName = lineCommands[1];
        std::vector<Event> events = eventsMap[channelName];

        // Calculate stats
        int totalReports = 0;
        int activeReports = 0;
        int forcesArrivalCount = 0;
        std::vector<std::string> eventDetails; // To store event details for later output

        for (Event& event : events) {
            if(event.get_eventOwnerUser() == user) {
                totalReports++;
                if (event.isActive()) { 
                    activeReports++;
                }
                if (event.forcesArrivalAtScene()) { 
                    forcesArrivalCount++;
                }

                // Collect event details
                std::stringstream eventStream;
                eventStream << "Report_" << totalReports << ":\n";
                eventStream << "city: " << event.get_city() << "\n";
                eventStream << "date time: " << epochToDate(event.get_date_time()) << "\n"; 
                eventStream << "event name: " << event.get_name() << "\n"; 
                std::string summary = event.get_description();
                if(summary.size() > 27){
                    summary = summary.substr(0,26) + "...";
                }
                eventStream << "summary: " << summary << "\n\n";
                eventDetails.push_back(eventStream.str());
            }  
        }

        // Write channel and stats summary
        outputFile << "Channel " << channelName << "\n";
        outputFile << "Stats:\n";
        outputFile << "Total: " << totalReports << "\n";
        outputFile << "active: " << activeReports << "\n";
        outputFile << "forces arrival at scene: " << forcesArrivalCount << "\n\n";

        outputFile << "Event Reports:\n";
        for (const auto& detail : eventDetails) {
            outputFile << detail;
        }

        outputFile.close();
        std::cout << "Summary written to " << outputFilePath << std::endl;
}


    std::string StompProtocol::epochToDate(time_t epoch) {
        // Convert epoch time to tm structure
        std::tm tm = *std::localtime(&epoch);

        // Format the date and time as "dd/MM/yy HH:mm"
        std::ostringstream oss;
        oss << std::put_time(&tm, "%d/%m/%y %H:%M");
        return oss.str();
    }


    int StompProtocol::findIndex(vector<string>& v, string val) {
        for (std::size_t i = 0; i < v.size(); i++) {
              // When the element is found
            if (v[i] == val) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }