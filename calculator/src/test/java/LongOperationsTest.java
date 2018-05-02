import client.Client;
import com.sun.javaws.exceptions.InvalidArgumentException;
import org.junit.After;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;
import server.Server;

import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class LongOperationsTest {
    private Server server;

    @Before
    public void before() throws IOException {
        server = new Server((short) 5001);
        server.start();
    }

    @After
    public void after() throws IOException {
        server.stop();
        server = null;
    }

    private Integer extractId(String result) throws InvalidArgumentException {
        final Pattern p = Pattern.compile("(<a href.*>)([0-9]+)");
        Matcher matcher = p.matcher(result);
        if (matcher.find()) {
            return Integer.parseInt(matcher.group(2));
        }
        throw new InvalidArgumentException(new String[]{"Invalid argument"});
    }

    @Test
    public void simple() throws IOException, IllegalAccessException, InvocationTargetException, InstantiationException, InvalidArgumentException {
        final Client client = new Client("localhost", (short) 5001);
        String result = client.sendUnaryOperation("factorial", BigInteger.valueOf(10));
        Integer id = extractId(result);
        Assert.assertEquals(Integer.valueOf(0), id);
        while (true) {
            result = client.checkResult(BigInteger.valueOf(id));
            if (!result.equals("<html><title>Home</title><body><h1>In progress</h1></body></html>")) {
                break;
            }
        }
        Assert.assertEquals("<html><title>Home</title><body><h1>3628800</h1></body></html>", result);
    }

    @Test
    public void sqrt() throws IOException, IllegalAccessException, InvocationTargetException, InstantiationException, InvalidArgumentException {
        final Client client = new Client("localhost", (short) 5001);
        String result = client.sendUnaryOperation("square", BigInteger.valueOf(16));
        Integer id = extractId(result);
        Assert.assertEquals(Integer.valueOf(0), id);
        while (true) {
            result = client.checkResult(BigInteger.valueOf(id));
            if (!result.equals("<html><title>Home</title><body><h1>In progress</h1></body></html>")) {
                break;
            }
        }
        Assert.assertEquals("<html><title>Home</title><body><h1>4</h1></body></html>", result);
    }

    @Test
    public void manyClients() throws IOException, IllegalAccessException, InvocationTargetException, InstantiationException, ExecutionException, InterruptedException {
        ExecutorService executor = Executors.newCachedThreadPool();
        List<Future> tasks = new ArrayList<>();
        for (int i = 0; i < 30; i++) {
            tasks.add(
                    executor.submit(() -> {
                        try {
                            final Client client = new Client("localhost", (short) 5001);
                            String result = client.sendUnaryOperation("factorial", BigInteger.valueOf(10));
                            Integer id = extractId(result);
                            while (true) {
                                result = client.checkResult(BigInteger.valueOf(id));
                                if (!result.equals("<html><title>Home</title><body><h1>In progress</h1></body></html>")) {
                                    break;
                                }
                            }
                            Assert.assertEquals("<html><title>Home</title><body><h1>3628800</h1></body></html>", result);

                        } catch (Exception e) {
                            throw new RuntimeException(e);
                        }
                    }));
        }

        for (Future f : tasks) {
            f.get();
        }
    }
}
