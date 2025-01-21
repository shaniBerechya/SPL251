package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.*;
import bgu.spl.net.srv.Connections;

public class StompMessagingProtocolImpl implements StompMessagingProtocol<StompFrame> {
    private StompDataBase dataBase;  
    private Connections<StompFrame> connections;
    private int connectionId;
    private boolean isTerminate;

    public StompMessagingProtocolImpl(){
        dataBase = StompDataBase.getInstance();
        isTerminate = false;
    }

    @Override
    public void start(int connectionId, Connections<StompFrame> connections) {
        this.connections = connections;
        this.connectionId = connectionId;
    }

    @Override
    public void process(StompFrame message) {
        String commend = message.getCommend();
        if(commend == null){
            StompFrame respond = erorGenretor(message,
                "No valid commened",
                null,
                "Sould be one of the folowing commends: CONNECT, SEND, SUBSCRIBE, UNSUBSCRIBE, DISCONNECT");
            connections.send(connectionId, respond);
            isTerminate = true;
        }

        // Checks that each command is handled correctly
        else if(commend.equals("CONNECT")){
            StompFrame respond = connectHendel(message);
            connections.send(connectionId, respond);
            if (respond.getCommend().equals("ERROR")) {        
                isTerminate = true;
            }
        }
        else if(commend.equals("SEND")){
            StompFrame respond = sendHendel(message);
            if (respond.getCommend().equals("ERROR")) {        
                isTerminate = true;
            }
            String channel = respond.getHeaderValue("destination");
            connections.send(channel, respond);
        }
        else if(commend.equals("SUBSCRIBE")){
            StompFrame respond = subscribeHendel(message);
            connections.send(connectionId, respond);
            if (respond.getCommend().equals("ERROR")) {        
                isTerminate = true;
            }
        }
        else if (commend.equals("UNSUBSCRIBE")){
            StompFrame respond = unsubscribeHendel(message);
            connections.send(connectionId, respond);
            if (respond.getCommend().equals("ERROR")) {        
                isTerminate = true;
            }
        }
        else if (commend.equals("DISCONNECT")){
            StompFrame respond = disconnectHendel(message);
            connections.send(connectionId, respond);
            if (respond.getCommend().equals("ERROR")) {        
                isTerminate = true;
            }
        }

        // In case the command is not valid, send an error message
        else{
            StompFrame respond = erorGenretor(message,
                "No valid commened",
                null,
                "Sould be one of the folowing commends: CONNECT, SEND, SUBSCRIBE, UNSUBSCRIBE, DISCONNECT");
            connections.send(connectionId, respond);
            isTerminate = true;
        }
    }

    @Override
    public boolean shouldTerminate() {
        return isTerminate;
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
         // Extract necessary headers
        String version = inputFrame.getHeaderValue("accept-version");
        String host = inputFrame.getHeaderValue("host");
        String login = inputFrame.getHeaderValue("login");
        String passcode = inputFrame.getHeaderValue("passcode");
    
        // Check headers for validity
        if (version == null || host == null || login == null || passcode == null){
            return erorGenretor(inputFrame,
                "missing header", 
                null,
                "should contain the following headers : 'accept-version', 'host', 'login', 'passcode'.");
           
        }
        if (!"1.2".equals(version) ) {
            return erorGenretor(inputFrame,
                "Unsupported version",
                null,
                "Expected version 1.2 ");
        }

        // Check if the user is already logged in
        if (dataBase.isConnectedUser(connectionId)) {
            return erorGenretor(inputFrame,
                "User already logged in",
                null,
                "User already logged in. Log out before trying again.");
        }

        // User does not exist, create new user and add to connected users
        if (dataBase.getPasswordForUser(login) == null) {
            dataBase.addOrUpdateUser(login, passcode, connectionId);
            StompFrame respond = new StompFrame("CONNECTED");
            respond.setHeaders("version : 1.2");
            return respond;

        }
         // Correct password, login successful, add to connected users
        else if (dataBase.getPasswordForUser(login).equals(passcode)) {
            dataBase.setNewConnected(connectionId);  // Mark user as logged in
            StompFrame respond = new StompFrame("CONNECTED");
            respond.setHeaders("version : 1.2");
            return respond;
            } 

            // Wrong password
            else {
                return erorGenretor(inputFrame,
                    "Wrong password",
                    null,
                    "Login failed due to incorrect credentials. Please check your password.");
        }
    }

    

    /**
    * @param inputFrame the {@link StompFrame.headers} should contain only 'destination' header,
    *        and also we gonna add 'receipt' but it's not mendatory.
    *        If a client is not subscribed to a topic, it is not allowed to send messages to it.
    *        The {@link StompFrame.FrameBody} should not be null.
    *
    * @return a 'MESSAGE' message if the massege was successful contianig the bodyFrame of 'SEND'.
    *         or an 'ERROR' if the server cannot process the frame for any reason
    *         (e.g., invalid topic, missing subscription, etc.).
    */
    private StompFrame sendHendel(StompFrame inputFrame){
        // Extract necessary headers
        String destination = inputFrame.getHeaderValue("destination");
        String receipt = inputFrame.getHeaderValue("receipt"); //can be null
        String frameBody = inputFrame.getFrameBody();
 
        // Check frame validity:
        //case 1: frame dosen't contain 'destination' header
        if(destination == null){
            return erorGenretor(inputFrame,
                "Missing destination header", receipt,
                "Did not contain a destination header which is REQUIRED for message propagation.");
        }
        //case 2: frameBody is null
        if(frameBody == null){
            return erorGenretor(inputFrame,
                "Missing frame body",
                receipt,
                "Body cannot be null");
        }
        //case 3: User not subscribed to the topic
        if(!dataBase.isSubscribersForChannel(connectionId, destination)){
            return erorGenretor(inputFrame,
                "User not subscribed to the topic",
                receipt, 
                "You must be subscribed to the topic to send messages to it.");
        }

        //Generting the 'MESSAGE' message:
        int messageID  = dataBase.addMessage(frameBody); //adds the message to the data base
        int subscriptionID = dataBase.getSubscriptionIDForChannel(destination, connectionId);

        String[] headers = {"subscription : " + subscriptionID,"message-id : " + messageID,"destination" + destination};
        StompFrame respond = new StompFrame("MESSAGE", headers, frameBody);
        return respond;
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
        // Extract necessary headers
        String destination = inputFrame.getHeaderValue("destination");
        String id = inputFrame.getHeaderValue("id");
        String receipt = inputFrame.getHeaderValue("receipt"); //can be null

        // Check frame validity:
        //case 1: frame dosen't contain 'destination' or 'id' header
        if(destination == null || id == null){
            return erorGenretor(inputFrame,
                "Missing header", receipt,
                "should contain the following headers : 'destination', 'id'.");
        }
        //case 2: user is alredy subscribe:
        if(dataBase.isSubscribersForChannel(connectionId, destination)){
            return erorGenretor(inputFrame,
                "Already subscribed", receipt,
                "You have attempted to subscribe to a channel to which you are already subscribed");
        }
        //case 3: frame contains frame body
        if(inputFrame.getFrameBody() != null){
            return erorGenretor(inputFrame,
                "Contain frame body",
                receipt,
                "Body should be null");
        }
        
        //Subscribe logic:
        dataBase.addChannelSubscription(destination, connectionId, Integer.valueOf(id));

        //if the client ask for receipt
        if(receipt != null){
            return new StompFrame("RECEIPT", new String[] {"receipt - id : "+ receipt } , null);
        }

        return null;
    }

    /**
    * @param inputFrame the {@link StompFrame.headers} should contain only the 'id' header.
    *        The {@link StompFrame.FrameBody} should be null.
    *
    * @return a 'RECEIPT' message if the massege was successful,
    *         or an 'ERROR' if the server cannot process the frame for any reason
    */
    private StompFrame unsubscribeHendel(StompFrame inputFrame){
        // Extract necessary headers
        String id = inputFrame.getHeaderValue("id");
        String receipt = inputFrame.getHeaderValue("receipt"); //can be null

        // Check frame validity:
        //case 1: frame dosen't contain 'id' header
        if( id == null){
            return erorGenretor(inputFrame,
                "Missing header", receipt,
                "should contain the 'id' headers.");
        }
        //case 2: frame contains frame body
        if(!dataBase.isValidID(Integer.valueOf(id), connectionId)){
            return erorGenretor(inputFrame,
                "No valid id",
                receipt,
                "Never subscribed to a channel with this id");
        }
        //case 3: frame contains frame body
        if(inputFrame.getFrameBody() != null){
            return erorGenretor(inputFrame,
                "Contain frame body",
                receipt,
                "Body should be null");
        }

        //unsubscribe logic:
        String channel = dataBase.getChannel(connectionId, Integer.valueOf(id));
        dataBase.removeChannelSubscription(channel, connectionId, Integer.valueOf(id));
        
        //if the client ask for receipt
        if(receipt != null){
            return new StompFrame("RECEIPT", new String[] {"receipt - id : "+ receipt } , null);
        }
        return null;
    }

    /**
    * @param inputFrame the {@link StompFrame.headers} should contain only the 'receipt' header.
    *        The {@link StompFrame.FrameBody} should be null.
    *
    * @return a 'RECEIPT' message with the receipt-id header sent by the client,
    *         or an 'ERROR' if the server cannot process the frame for any reason
    */
    private StompFrame disconnectHendel(StompFrame inputFrame){
       // Extract necessary headers
       String receipt = inputFrame.getHeaderValue("receipt"); 
       
       //case 1: frame dosen't contain receipt header
       if( receipt == null){
        return erorGenretor(inputFrame,
            "Missing header", receipt,
            "should contain the 'receipt' headers.");
        }
        //case 2: frame contains frame body
        if(inputFrame.getFrameBody() != null){
           return erorGenretor(inputFrame,
               "Contain frame body",
               receipt,
               "Body should be null");
        }
        //Disconnect logic:
        dataBase.disconnect(connectionId);
        return new StompFrame("RECEIPT", new String[] {"receipt - id : "+ receipt } , null);
    }

    /********************************************* Other Mthods *******************************************************************/
    
    private StompFrame erorGenretor(StompFrame defultedFrame,String erorMessage,String messageID,String erorExplanetion ){
        StompFrame erorRespond = new StompFrame("ERROR");
        erorRespond.setHeaders("message :"+ erorMessage);
        if(messageID != null){
            erorRespond.setHeaders("receipt - id :" + messageID);
        }
        String bodyFrame = "The message: \n ----- \n "+ defultedFrame.toString() + "\n ----- \n " + erorExplanetion;  
        erorRespond.setFrameBody(bodyFrame);
        return erorRespond;
    }
}
