package server;

import utils.data.Lot;
import utils.data.User;

import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

public class Main {
    public static void main(String[] args) {
        Map<Integer, Lot> lots = new ConcurrentHashMap<>();
        Set<User> users = ConcurrentHashMap.newKeySet();
    }
}
