package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.*;
import bgu.spl.net.srv.Connections;

public class StompMessagingProtocolImpl implements StompMessagingProtocol<StompFrame> {

    @Override
    public void start(int connectionId, Connections<StompFrame> connections) {
        // TODO Auto-generated method stub
        throw new UnsupportedOperationException("Unimplemented method 'start'");
    }

    @Override
    public void process(StompFrame message) {
        String commend = message.getCommend();
        if(commend == null){
            //TODO needs to send eror message
        }

        // Checks that each command is handled correctly
        else if(commend.equals("CONNECT")){
            connectHendel(message);
        }
        else if(commend.equals("SEND")){
            sendHendel(message);
        }
        else if(commend.equals("SUBSCRIBE")){
            subscribeHendel(message);
        }
        else if (commend.equals("UNSUBSCRIBE")){
            unsubscribeHendel(message);
        }
        else if (commend.equals("DISCONNECT")){
            disconnectHendel(message);
        }

        // In case the command is not valid, send an error message
        else{
           //TODO 
        }
    }

    @Override
    public boolean shouldTerminate() {
        // TODO Auto-generated method stub
        throw new UnsupportedOperationException("Unimplemented method 'shouldTerminate'");
    }


    /******************************************functions to hendels comends: ***********************************************/
    
    /**
    * @param inputFrame should contain the following {@link StompFrame.headers}:
    *        'accept-version', 'host', 'login', 'passcode'.
    *        - 'login' should be a username that exists in the database.
    *        - 'passcode' must match the username.
    *
    * @return a 'CONNECTED' message if the connection was successful,
    *         or an 'ERROR' message explaining what went wrong.
    */

    private StompFrame connectHendel(StompFrame inputFrame){
        // TODO
        throw new UnsupportedOperationException("Unimplemented method 'connectHendel'");
    }

    /**
    * @param inputFrame the {@link StompFrame.headers} should contain only the 'destination' header.
    *        If a client is not subscribed to a topic, it is not allowed to send messages to it.
    *        The {@link StompFrame.FrameBody} should not be null.
    *
    * @return a 'RECEIPT' message if the massege was successful ?????????,
    *         or an 'ERROR' if the server cannot process the frame for any reason
    *         (e.g., invalid topic, missing subscription, etc.).
    */
    private StompFrame sendHendel(StompFrame inputFrame){
        // TODO
        throw new UnsupportedOperationException("Unimplemented method 'sendHendel'");
    }

    /**
    * @param inputFrame should contain the following {@link StompFrame.headers}:
    *        'destination', 'id'.
    *        The {@link StompFrame.FrameBody} should be null.
    *
    * @return a 'RECEIPT' message if the massege was successful,
    *         or an 'ERROR' if the server cannot process the frame for any reason
    */
    private StompFrame subscribeHendel(StompFrame inputFrame){
        // TODO
        throw new UnsupportedOperationException("Unimplemented method 'subscribeHendel'");
    }

    /**
    * @param inputFrame the {@link StompFrame.headers} should contain only the 'id' header.
    *        The {@link StompFrame.FrameBody} should be null.
    *
    * @return a 'RECEIPT' message if the massege was successful,
    *         or an 'ERROR' if the server cannot process the frame for any reason
    */
    private StompFrame unsubscribeHendel(StompFrame inputFrame){
        // TODO
        throw new UnsupportedOperationException("Unimplemented method 'unsubscribeHendels'");
    }

    /**
    * @param inputFrame the {@link StompFrame.headers} should contain only the 'receipt' header.
    *        The {@link StompFrame.FrameBody} should be null.
    *
    * @return a 'RECEIPT' message with the receipt-id header sent by the client,
    *         or an 'ERROR' if the server cannot process the frame for any reason
    */
    private StompFrame disconnectHendel(StompFrame inputFrame){
        // TODO
        throw new UnsupportedOperationException("Unimplemented method 'disconnectHendel'");
    }
}
