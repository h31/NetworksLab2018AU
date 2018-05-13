package ru.spbau.mit;

import java.util.concurrent.atomic.AtomicLong;

import static ru.spbau.mit.Protocol.LINE_DELIMITER;
import static ru.spbau.mit.Protocol.ProtocolException;

public final class Lot {
    private static AtomicLong lotCount = new AtomicLong(0);

    private String name;
    private long cost;
    private long id;
    private Object owner;

    public Lot(long id, String name, long startCost) throws ProtocolException {
        this.id = id;
        if (name.contains(LINE_DELIMITER)) {
            throw new ProtocolException("\\n not allowed in lots' names");
        }
        this.name = name;
        this.cost = startCost;
        if (startCost <= 0) {
            throw new ProtocolException("cost should be positive");
        }
    }

    // copy constructor.
    public Lot(Lot other) throws ProtocolException {
        this(other.id, other.name, other.cost);
    }

    // For testing purposes only.
    public Lot(long startCost) {
        this.cost = startCost;
        this.id = lotCount.getAndIncrement();
        this.name = "Lot#" + String.valueOf(this.id);
    }

    // For client's purposes only.
    public Lot(String name, long startCost) {
        this.cost = startCost;
        this.id = lotCount.getAndIncrement();
        this.name = name;
    }

    public String getName() {
        return name;
    }

    public long getCost() {
        return cost;
    }

    public void setCost(long newCost) {
        cost = newCost;
    }

    public long getId() {
        return id;
    }

    public Object getOwner() {
        return owner;
    }

    public void setOwner(Object owner) {
        this.owner = owner;
    }
}
