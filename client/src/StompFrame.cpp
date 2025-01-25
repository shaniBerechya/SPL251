    #include "../include/StompFrame.h" 
    
    //constractors:
    StompFrame::StompFrame():command(""), headers(), frameBody("") {}
    StompFrame::StompFrame(string& command, map<string,string> completeHeaders,string& frameBody):
    command(command),headers(completeHeaders),frameBody(frameBody)  {}
    
    StompFrame::StompFrame(string& command):command(command),headers(), frameBody(""){}

    //destractors:
    StompFrame::~StompFrame(){}

    //geters:
    string StompFrame::getCommand() const{
        return command;
    }
    map<string , string> StompFrame::getHeaders() const{
        return headers;
    }
    string StompFrame::getHeaderByName(string headerName){
        return headers[headerName];
    }
    string StompFrame::getFrameBody() const{
        return frameBody;
    }

    //seters:
    void StompFrame::setCommand(string command){
        this->command = command;
    }
    
    void StompFrame::setHeadersByParts (string hederName,string hederValue){
        headers[hederName] = hederValue;
    }

    void StompFrame::setFrameBody(string frameBody){
        this->frameBody = frameBody;
    }

    //other:

    string StompFrame::toString(){
        string respond = command + "\n";

        //adding the heders:
        for (auto& header: headers){
            respond = respond + header.first + ":" + header.second + "\n";
        }

        respond = respond + "\n" + frameBody + "\n\0"; //adding delimiter
        return respond;
    }