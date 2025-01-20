package bgu.spl.net.impl.stomp;

public class main {
    public static void main(String[] args) {
        StompMessageEncoderDecoder encode = new StompMessageEncoderDecoder();
        String mesg = "S\n" + 
                        "d:de\n" + 
                        "id:1\n" + 
                        "\u0000";
        
        byte[] bytesMesg = mesg.getBytes();
        StompFrame result;
        for(byte b: bytesMesg){
            result = encode.decodeNextByte(b);
                if (result != null){
                    System.out.println(result);;
                }
            }
        }
}
