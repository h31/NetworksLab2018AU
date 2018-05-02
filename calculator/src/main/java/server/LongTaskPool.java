package server;

import com.sun.istack.internal.NotNull;

import java.util.Map;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicInteger;

public class LongTaskPool {

    @NotNull
    private final ExecutorService longTaskPool = Executors.newCachedThreadPool();

    @NotNull
    private final Map<Integer, Future<String>> results = new ConcurrentHashMap<>();

    @NotNull
    private final AtomicInteger globalId = new AtomicInteger(0);

    public boolean contains(Integer id) {
        return results.containsKey(id);
    }

    public Future<String> get(Integer id) {
        return results.get(id);
    }

    public Integer add(Callable<String> task) {
        Integer id = globalId.getAndIncrement();
        results.put(id, longTaskPool.submit(task));
        return id;
    }
}
