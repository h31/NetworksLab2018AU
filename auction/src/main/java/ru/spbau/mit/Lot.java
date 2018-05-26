package ru.spbau.mit;

import java.util.concurrent.atomic.AtomicLong;

import static ru.spbau.mit.Protocol.INLINE_DELIMITER;
import static ru.spbau.mit.Protocol.LINE_DELIMITER;
import static ru.spbau.mit.Protocol.ProtocolException;

// TODO Parcelable or Serializable.
// https://stackoverflow.com/questions/7290777/java-custom-serialization
public final class Lot {
    private static final AtomicLong lotCount = new AtomicLong(0);

    private String name;
    private long cost;
    private long id;
    private Object owner;
    private String soldState = null;

    public Lot(long id, String name, long startCost) throws ProtocolException {
        this.id = id;
        if (name.contains(LINE_DELIMITER)) {
            final String message = String.format("'%s' and '%s' not allowed in lots' names", LINE_DELIMITER, INLINE_DELIMITER);
            throw new ProtocolException(message);
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
        this.owner = other.owner;
        this.soldState = other.soldState;
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

    public String getSoldState() {
        return soldState;
    }

    public void setSoldState(String soldState) {
        this.soldState = soldState;
    }
}
