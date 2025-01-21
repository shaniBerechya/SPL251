package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.MessageEncoderDecoder;
import bgu.spl.net.srv.Server;

public class StompServer {
    public static void main(String[] args) {
        int port = Integer.parseInt(args[0]);
        String mode = args[1];
        if (mode.equals("tpc")){
            Server.<StompFrame>threadPerClient(
                port,
                StompMessagingProtocolImpl::new, // Factory for the protocol
                StompMessageEncoderDecoder::new // Factory for the encoder/decoder
            ).serve();
        }

        else if(mode.equals("reactor")){
            Server.<StompFrame>reactor(
            Runtime.getRuntime().availableProcessors(), //העתקתי מהecoServer
            port,
            StompMessagingProtocolImpl::new, // Factory for the protocol
            StompMessageEncoderDecoder::new // Factory for the encoder/decoder
            ).serve();
        }
        else{
            // טיפול במקרה שהמצב שהועבר לא תקין
            System.err.println("Invalid mode: " + mode);
            System.err.println("Supported modes are: tpc, reactor");
        }
    }
}