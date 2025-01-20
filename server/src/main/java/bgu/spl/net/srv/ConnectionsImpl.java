package bgu.spl.net.srv;

import java.io.IOException;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

import bgu.spl.net.impl.stomp.StompDataBase;
import bgu.spl.net.impl.stomp.StompFrame;

public class ConnectionsImpl<T> implements Connections<T>{
    private StompDataBase database;
    private Map<Integer, ConnectionHandler<T>> idToHendler;

    public ConnectionsImpl(){
        this.database = StompDataBase.getInstance();
        this.idToHendler = new ConcurrentHashMap<>();
    }

    public void addConnectionHandler(int connectionId,ConnectionHandler<T> connectionHandler){
        idToHendler.put(connectionId, connectionHandler);
    }

    @Override
    public boolean send(int connectionId, T msg) {
        idToHendler.get(connectionId).send(msg);
        return true;
    }

    @Override
    public void send(String channel, T msg) {
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
