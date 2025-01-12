package bgu.spl.net.impl.stomp;

import java.nio.charset.StandardCharsets;
import java.util.Arrays;

import bgu.spl.net.api.MessageEncoderDecoder;

public class StompMessageEncoderDecoder implements MessageEncoderDecoder<StompFrame> {

    /********************************************* Fildes *****************************************************/
    private byte[] bytes = new byte[1 << 10]; //start with 1k
    private int len = 0;

    private StompFrame  frame = new StompFrame(); // noy sure if evry time we need to erase the StompFrame
    boolean toResat = false;
    
    //Section in the StompFrame
    private enum Stage { // the section in the StompFrame
        COMMAND,
        HEADERS,
        FRAME_BODY
    }
    private Stage stage = Stage.COMMAND; // the freme start with COMMAND

    /********************************************* Methods *****************************************************/

    @Override
    public StompFrame decodeNextByte(byte nextByte) {
        //Cleaning the StompFrame for new frame
        if(toResat){
            this.frame = new StompFrame();
        }

        //Frame decoding logic:
        if(stage == Stage.COMMAND){
            if(nextByte == '\n'){
                frame.setCommend(popString());
                stage = Stage.HEADERS; //moving to the next section
            }
            pushByte(nextByte);
            return null; //not a frame yet
        }
        else if(stage == Stage.HEADERS){
            if(nextByte == '\n' && len > 0){ //this condition check the we have a header to add
                frame.setHeaders(popString());
            }
            else if (nextByte == '\n' && len == 0) { //the HEADERS section is over
                stage =  Stage.FRAME_BODY; //moving to the next section
            }
            pushByte(nextByte);
            return null; //not a frame yet
        }
        else if(stage == Stage.FRAME_BODY){
            if(nextByte == '\u0000'){
                frame.setFrameBody(popString());
                toResat = true; //A flage to indicates that we need to reSet the frame
                return frame;
            }
            pushByte(nextByte);
            return null ; //not a frame yet
        }
        return null;

    }

    @Override
    public byte[] encode(StompFrame frame) {
        String frameAsString = frame.toString();
        String result = frameAsString + "\u0000"; //end of frame
        return result.getBytes(); 
    }

    private String popString() {
        //notice that we explicitly requesting that the string will be decoded from UTF-8
        //this is not actually required as it is the default encoding in java.
        String result = new String(bytes, 0, len, StandardCharsets.UTF_8);
        len = 0;
        return result;
    }

    private void pushByte(byte nextByte) {
        if (len >= bytes.length) {
            bytes = Arrays.copyOf(bytes, len * 2);
        }

        bytes[len++] = nextByte;
    }

    
    
}
