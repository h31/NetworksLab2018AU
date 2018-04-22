package utils.data;

public class Lot {
    private final String description;
    private final int currentPrice;
    private final User currentLeader;
    private final int id;

    public Lot(String description, int currentPrice, User currentLeader, int id) {
        this.description = description;
        this.currentPrice = currentPrice;
        this.currentLeader = currentLeader;
        this.id = id;
    }

    public String getDescription() {
        return description;
    }

    public int getCurrentPrice() {
        return currentPrice;
    }

    public User getCurrentLeader() {
        return currentLeader;
    }

    public int getId() {
        return id;
    }
}
