#pragma once
#include "../include/StompFrame.h"

class  StompMessageEncoderDecoder{
private:

public:
std::vector<unsigned char> encode(StompFrame frame);

};

