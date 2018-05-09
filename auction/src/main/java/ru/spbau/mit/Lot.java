package ru.spbau.mit;

import java.util.concurrent.atomic.AtomicLong;

public final class Lot {
    private static AtomicLong lotCount = new AtomicLong(0);

    private String name;
    private long startCost;
    private long id;

    public Lot(String name, long startCost) {
        this(startCost);
        this.name = name;
    }

    public Lot(long startCost) {
        this.startCost = startCost;
        this.id = lotCount.getAndIncrement();
        this.name = "Lot#" + String.valueOf(this.id);
    }

    public String getName() {
        return name;
    }

    public long getStartCost() {
        return startCost;
    }
}
