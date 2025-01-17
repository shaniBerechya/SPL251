package bgu.spl.net.impl.stomp;

import java.nio.charset.StandardCharsets;
import java.util.Arrays;

import bgu.spl.net.api.MessageEncoderDecoder;

public class StompMessageEncoderDecoder implements MessageEncoderDecoder<StompFrame> {

    /********************************************* Fildes *****************************************************/
    private byte[] bytes = new byte[1 << 10]; //start with 1k
    private int len = 0;

    /*private StompFrame  frame = new StompFrame(); // noy sure if evry time we need to erase the StompFrame
    boolean toResat = false;
    
    //Section in the StompFrame
    private enum Stage { // the section in the StompFrame
        COMMAND,
        HEADERS,
        FRAME_BODY
    }
    private Stage stage = Stage.COMMAND; // the freme start with COMMAND 
    */

    /********************************************* Methods *****************************************************/

    @Override
    public StompFrame decodeNextByte(byte nextByte) {
        if(nextByte == '\u0000'){
            String rawFrame = popString();
            return parseStompFrame(rawFrame);
        }
        pushByte(nextByte);
        return null; //not a frame yet

    }

    public StompFrame parseStompFrame(String rawFrame) {
        StompFrame frame = new StompFrame();
        // Split the raw frame into header/command part and body part at the first occurrence of two newlines
        String[] parts = rawFrame.split("\n\n", 2); 
        String headersAndCommand = parts[0];
        String body = null;
        // Check if there is a body part after the double newline
        if(parts.length > 1 && parts[1] != null && parts[1].length() >= 1) {
            body = parts[1];
        }
    
        // Split the headers and command part into separate lines
        String[] lines = headersAndCommand.split("\n");
        frame.setCommend(lines[0]); // The first line is always the command
    
        // Process each header line
        for (int i = 1; i < lines.length; i++) {
            frame.setHeaders(lines[i]); // Add each header to the frame
        }
    
        // Set the body of the frame, trimming any leading/trailing whitespace
        if (body != null) {
            frame.setFrameBody(body.trim());
        }
    
        return frame;
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
    

