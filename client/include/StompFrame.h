#pragma once

#include <string> 
#include <iostream>
#include <map>
#include <vector>


using std::string;
using std::map;
using std::vector;

class StompFrame
{
private:
    string command;  // יש לוודא שימוש ב- std::string
    map<string,string> headers;
    string frameBody;
public:
    //constractors:
    StompFrame();
    StompFrame(string& command,vector<string>& completeHeaders,string& frameBody);
    StompFrame(string& command);

    //destractors:
    ~StompFrame();

    //geters:
    string getCommand() const;
    map<string , string> getHeaders() const;
    string getHeaderByName(string headerName);
    string getFrameBody() const;

    //seters:
    void setCommand(string command);
    void setHeaders (string& complitHeader);
    void setHeadersByParts (string hederName,string hederValue);
    void setMultiHeaders(vector<string>& completeHeaders);
    void setFrameBody(string& frameBody);

    //other:
    string toString();

};