package bgu.spl.net.impl.stomp;

import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * Singleton class for managing users, channels, and messages in a STOMP protocol implementation.
 */
public class StompDataBase {

    private Map<String, String> usersPasswords; // Maps usernames to passwords
    private Map<String, Set<Integer>> channels; // Maps channels to sets of onnection IDs subscribed to that channel
    private Map<Integer, Map<Integer, String>> subscriptionsDetails; // Maps connection IDs to maps of subscription IDs to channels
    private Map<Integer, String> messages; // Maps message IDs to message contents
    private AtomicInteger messageIDCounter; // Counter for generating unique message IDs
    private Set<Integer> connectedUsers; // Set of currently connected usernames

    private static class SingletonHolder {
        private static final StompDataBase instance = new StompDataBase();
    }

    public static StompDataBase getInstance() {
        return SingletonHolder.instance;
    }

    private StompDataBase() {
        usersPasswords = new ConcurrentHashMap<>();
        channels = new ConcurrentHashMap<>();
        subscriptionsDetails = new ConcurrentHashMap<>();
        messages = new ConcurrentHashMap<>();
        messageIDCounter = new AtomicInteger(0);
        connectedUsers = ConcurrentHashMap.newKeySet();
    }

    /**
     * Checks if a user is currently connected.
     * @param connectionID The connectionID to check.
     * @return true if the user is connected, false otherwise.
     */
    public boolean isConnectedUser(Integer connectionID) {
        return connectedUsers.contains(connectionID);
    }

    /**
     * Retrieves the password associated with a given username.
     * @param username The username whose password is to be retrieved.
     * @return The password, or null if no such user exists.
     */
    public String getPasswordForUser(String username) {
        return usersPasswords.get(username);
    }

    /**
     * Adds or updates a user in the database.
     * @param username The username of the user.
     * @param password The password of the user.
     */
    public void addOrUpdateUser(String username, String password,Integer connectionID ) {
        usersPasswords.put(username, password);
        connectedUsers.add(connectionID);
    }

    /**
     * Marks a user as connected.
     * @param connectionID The connectionID of the user.
     */
    public void setNewConnected(Integer connectionID ) {
        connectedUsers.add(connectionID);
    }

    /**
     * Disconnects a user, removing them from the set of connected users.
     * @param connectionID The connectionID to disconnect.
     */
    public void disconnect(Integer connectionID) {
        connectedUsers.remove(connectionID);
        
        //remove the connectionID from all the channels which it subscribe to:
        Map<Integer, String> userChannels = subscriptionsDetails.get(connectionID);
        for (String channel : userChannels.values()) {
            channels.get(channel).remove(connectionID);
        }

        //remone the connectionID from subscriptionsDetails:
        subscriptionsDetails.remove(connectionID);

    }

    /**
     * Checks if a connection ID is currently subscribed to a specified channel.
     * @param connectionID The connection ID to check.
     * @param subscriptionID The subscriptionID to check against.
     * @return true if the connection ID is subscribed, false otherwise.
     */
    public boolean isSubscribersForChannel(Integer connectionID, String channel) {
        return getSubscriptionIDForChannel(channel, connectionID) != -1;
    }

    /**
     * Retrieves the subscription ID for a given channel and connection ID.
     * @param channel The channel name.
     * @param connectionID The connection ID.
     * @return The subscription ID, or -1 if no such subscription exists.
     */
    public int getSubscriptionIDForChannel(String channel, Integer connectionID) {
        Map<Integer, String> userChannels = subscriptionsDetails.get(connectionID);
        // Searches for a specific channel in the userChannels map and returns the corresponding connection ID
        for (Map.Entry<Integer, String> entry : userChannels.entrySet()) {
            if(entry.getValue().equals(channel)){
                return entry.getKey();
            }
        }
        return -1;
    }

    /**
     * Adds a subscription for a connection ID to a specified channel.
     * @param channel The channel to subscribe to.
     * @param connectionID The connection ID subscribing.
     * @param subID The subscription ID associated with this subscription.
     */
    public void addChannelSubscription(String channel, Integer connectionID, Integer subID) {
        // Ensure the channel exists in the map and associate it with a new HashSet if it's being referenced for the first time. Then add the connection ID to the set of IDs subscribed to this channel.
        channels.computeIfAbsent(channel, k -> new HashSet<>()).add(connectionID);

        // Ensure the connection ID exists in the map and associate it with a new HashMap if it's being referenced for the first time. Then map the subscription ID to the channel for this connection ID.
        subscriptionsDetails.computeIfAbsent(connectionID, k -> new HashMap<>()).put(subID, channel);

    }

    /**
     * Removes a subscription for a connection ID from a specified channel.
     * @param channel The channel to unsubscribe from.
     * @param connectionID The connection ID to remove.
     */
    public void removeChannelSubscription(String channel, Integer connectionID, Integer subID) {
        channels.get(channel).remove(connectionID);
        subscriptionsDetails.get(connectionID).remove(subID);

    }

    /**
     * Checks if a given subscription ID is valid, i.e., associated with any channel.
     * @param id The subscription ID to check.
     * @return true if valid, false otherwise.
     */
    public boolean isValidID(Integer id,Integer connectionID ) {
        Map<Integer, String> userChannels = subscriptionsDetails.get(connectionID);
        for (Map.Entry<Integer, String> entry : userChannels.entrySet()) {
            if(entry.getKey().equals(id)){
                return true;
            }
        }
        return false;
    }

    /**
     * Adds a message to the database and returns its new unique ID.
     * @param message The message content to add.
     * @return The unique ID assigned to the new message.
     */
    public int addMessage(String message){
        int id = messageIDCounter.incrementAndGet();
        messages.put(id, message);
        return id;
    }

    /**
     * Retrieves the content of a message based on its ID.
     * @param messageId The ID of the message to retrieve.
     * @return The content of the message, or null if no such message exists.
     */
    public String getMessageContent(int messageId) {
        return messages.get(messageId);
    }

    public String getChannel(Integer connectionId, Integer id){
        return subscriptionsDetails.get(connectionId).get(id);
    }

    public Set<Integer> getSubscribers(String channel){
        return channels.get(channel);
    }
}
