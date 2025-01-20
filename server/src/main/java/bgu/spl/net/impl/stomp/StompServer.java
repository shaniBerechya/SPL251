package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.Server;

public class StompServer {

    public static void main(String[] args) {
        //Sapty Chack for args
        if(args.length != 2){
            System.out.println("args dosent contein port and server type");
        }
        else if (args[1] != "reactor" && args[1] != "tpc") {
            System.out.println("server type should be : <reactor / tpc>");
        }
        else{
            int port = Integer.valueOf(args[0]); //geting the port from args
            //case 1: tpc
            if (args[1] == "tpc"){
                Server.threadPerClient(
                    port,
                    () -> new StompMessagingProtocolImpl(), //protocol factory,
                    () -> new StompMessageEncoderDecoder())//message encoder decoder factory)
                    .serve(); 
            }
            //case 2: tpc
            else if (args[1] == "reactor"){
                Server.reactor(
                    port,
                    Runtime.getRuntime().availableProcessors(),
                    () -> new StompMessagingProtocolImpl(), //protocol factory,
                    () -> new StompMessageEncoderDecoder())//message encoder decoder factory
                    .serve();
            }
        }
    }
}
