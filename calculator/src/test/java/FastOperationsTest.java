import client.Client;
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

public class FastOperationsTest {
    private Server server;

    @Before
    public void before() throws IOException {
        server = new Server((short) 5000);
        server.start();
    }

    @After
    public void after() throws IOException {
        server.stop();
        server = null;
    }

    @Test
    public void simpleTest() throws IOException, IllegalAccessException, InvocationTargetException, InstantiationException {
        final Client client = new Client("localhost", (short) 5000);
        String result = client.sendBinaryOperation("addition", BigInteger.valueOf(1), BigInteger.valueOf(2));
        Assert.assertEquals("<html><title>Fast Calculation</title><body><h1>3</h1></body></html>", result);
    }

    @Test
    public void addition() throws IOException, IllegalAccessException, InvocationTargetException, InstantiationException {
        final Client client = new Client("localhost", (short) 5000);
        String result = client.sendBinaryOperation("addition", BigInteger.valueOf(1), BigInteger.valueOf(2));
        Assert.assertEquals("<html><title>Fast Calculation</title><body><h1>3</h1></body></html>", result);
        result = client.sendBinaryOperation("addition", BigInteger.valueOf(5), BigInteger.valueOf(7));
        Assert.assertEquals("<html><title>Fast Calculation</title><body><h1>12</h1></body></html>", result);
    }

    @Test
    public void multiplication() throws IOException, IllegalAccessException, InvocationTargetException, InstantiationException {
        final Client client = new Client("localhost", (short) 5000);
        String result = client.sendBinaryOperation("multiplication", BigInteger.valueOf(2), BigInteger.valueOf(7));
        Assert.assertEquals("<html><title>Fast Calculation</title><body><h1>14</h1></body></html>", result);
        result = client.sendBinaryOperation("multiplication", BigInteger.valueOf(5), BigInteger.valueOf(7));
        Assert.assertEquals("<html><title>Fast Calculation</title><body><h1>35</h1></body></html>", result);
    }

    @Test
    public void division() throws IOException, IllegalAccessException, InvocationTargetException, InstantiationException {
        final Client client = new Client("localhost", (short) 5000);
        String result = client.sendBinaryOperation("division", BigInteger.valueOf(21), BigInteger.valueOf(7));
        Assert.assertEquals("<html><title>Fast Calculation</title><body><h1>3</h1></body></html>", result);
        result = client.sendBinaryOperation("division", BigInteger.valueOf(5), BigInteger.valueOf(0));
        Assert.assertEquals("<html><title>Fast Calculation</title><body><h1>Error division by zero</h1></body></html>", result);
    }


    @Test
    public void subtraction() throws IOException, IllegalAccessException, InvocationTargetException, InstantiationException {
        final Client client = new Client("localhost", (short) 5000);
        String result = client.sendBinaryOperation("subtraction", BigInteger.valueOf(3), BigInteger.valueOf(2));
        Assert.assertEquals("<html><title>Fast Calculation</title><body><h1>1</h1></body></html>", result);
        result = client.sendBinaryOperation("subtraction", BigInteger.valueOf(5), BigInteger.valueOf(7));
        Assert.assertEquals("<html><title>Fast Calculation</title><body><h1>-2</h1></body></html>", result);
    }


    @Test
    public void manyClients() throws IOException, IllegalAccessException, InvocationTargetException, InstantiationException, ExecutionException, InterruptedException {
        ExecutorService executor = Executors.newCachedThreadPool();
        List<Future> tasks = new ArrayList<>();
        for (int i = 0; i < 1000; i++) {
            tasks.add(
                    executor.submit(() -> {
                        try {
                            final Client client = new Client("localhost", (short) 5000);
                            String result = client.sendBinaryOperation("addition", BigInteger.valueOf(1), BigInteger.valueOf(2));
                            Assert.assertEquals("<html><title>Fast Calculation</title><body><h1>3</h1></body></html>", result);
                            result = client.sendBinaryOperation("addition", BigInteger.valueOf(2), BigInteger.valueOf(3));
                            Assert.assertEquals("<html><title>Fast Calculation</title><body><h1>5</h1></body></html>", result);
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