package bgu.spl.net.srv;

import java.io.IOException;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

import bgu.spl.net.impl.stomp.StompDataBase;
import bgu.spl.net.impl.stomp.StompFrame;

public class ConnectionsImpl implements Connections<StompFrame>{
    private StompDataBase database;
    private Map<Integer, ConnectionHandler<StompFrame>> idToHendler;

    public ConnectionsImpl(){
        this.database = StompDataBase.getInstance();
        this.idToHendler = new ConcurrentHashMap<>();
    }

    public void addConnectionHandler(int connectionId,ConnectionHandler<StompFrame> connectionHandler ){
        idToHendler.put(connectionId, connectionHandler);
    }

    @Override
    public boolean send(int connectionId, StompFrame msg) {
        idToHendler.get(connectionId).send(msg);
        return true;
    }

    @Override
    public void send(String channel, StompFrame msg) {
        Set<Integer> subToChannel = database.getSubscribers(channel);
        for (Integer subConnectionId : subToChannel){
            send(subConnectionId,msg);
        }
    }

    @Override
    public void disconnect(int connectionId) {
        try {
            idToHendler.get(connectionId).close();
        }
        catch(IOException e){}

        idToHendler.remove(connectionId);
    }
    
}
