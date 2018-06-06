package server;

import utils.data.Lot;
import utils.data.User;

import java.net.Socket;
import java.util.Map;
import java.util.Set;

public class Context {

    private final Map<Integer, Lot> lots;
    private final Set<User> users;
    private final Set<Socket> clientSockets;
    private final Socket clientSocket;
    private User user;

    public Context(Map<Integer, Lot> lots, Set<User> users, Socket clientSocket, Set<Socket> clientSockets) {
        this.lots = lots;
        this.users = users;
        this.clientSocket = clientSocket;
        this.clientSockets = clientSockets;
    }

    public Map<Integer, Lot> getLots() {
        return lots;
    }

    public void setUser(User user) {
        this.user = user;
    }

    public User getUser() {
        return user;
    }

    public void addUser(User user) {
        users.add(user);
    }

    public boolean containsUser(User user) {
        return users.contains(user);
    }

    public boolean containsLot(int lotId) {
        return lots.containsKey(lotId);
    }

    public Lot getLot(int lotId) {
        return lots.get(lotId);
    }

    public void addLot(Lot lot) {
        lots.put(lot.getLotId(), lot);
    }

    public void stopLot(int lotId) {
        lots.remove(lotId);
    }

    public void clear() {
        try {
            clientSockets.remove(clientSocket);
            users.remove(user);
        } catch (Exception ignored) {}
    }
}
