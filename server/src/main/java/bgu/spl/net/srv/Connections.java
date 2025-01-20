package bgu.spl.net.srv;

import java.io.IOException;

import bgu.spl.net.impl.stomp.StompFrame;

public interface Connections<T> {

    boolean send(int connectionId, T msg);

    void send(String channel, T msg);

    void disconnect(int connectionId);

    public void addConnectionHandler(int connectionId,ConnectionHandler<T> connectionHandler);
}
