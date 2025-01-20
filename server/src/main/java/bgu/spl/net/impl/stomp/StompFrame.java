package bgu.spl.net.impl.stomp;

import java.util.HashMap;
import java.util.Map;

public class StompFrame {

    /********************************************* Fildes *****************************************************/
    private String commend;
    private Map<String,String> headers;
    private String frameBody;

    /********************************************* Constrector *****************************************************/

    public StompFrame (String commend, Map<String,String> headers, String frameBody){
        this.commend = commend;
        this.headers = headers;
        this.frameBody = frameBody;
    }

    public StompFrame(){
        this(null,new HashMap<>() ,null);
    }

    public StompFrame(String commend){
        this(commend,new HashMap<>() ,null);
    }

    public StompFrame(String commend, String[] headers,String frameBody){
        this(commend,new HashMap<>() ,frameBody);
        setMultyHeaders(headers);
    }
    /********************************************* Methods *****************************************************/
    //Seters:

    public void setCommend (String commend){
        this.commend = commend;
    }

    public void setHeaders (String complitHeader){
        String[] parts = complitHeader.split(":", 2);
        String headerName = parts[0].trim();
        String headerValue = parts[1].trim();
        headers.put(headerName,headerValue);
    }

    public void setMultyHeaders (String[] complitHeaders){
        for(String complitHeader: complitHeaders){
            setHeaders(complitHeader);
        }
    }

    public void setHeadersByPart (String headerName,String headerValue){
        headers.put(headerName,headerValue);
    }

    public void setFrameBody(String frameBody){
        this.frameBody = frameBody;
    }

    //Geters

    public String getCommend(){
        return commend;
    }

    public String getHeaderValue(String headerName){
        return headers.get(headerName);
    }

    public String getFrameBody(){
        return frameBody;
    }

    @Override
    public String toString() {
        StringBuilder builder = new StringBuilder();
        //Commend
        builder.append(commend).append("\n");
        //Headers
        for (Map.Entry<String, String> header : headers.entrySet()) {
            builder.append(header.getKey()).append(" : ").append(header.getValue()).append("\n");
        }
        //FrameBody
        if(frameBody != null){
            builder.append("\n").append(frameBody).append("\n");
        }
        
        return builder.toString();
    }
    
}
