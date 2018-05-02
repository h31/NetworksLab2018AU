package client;

import com.sun.istack.internal.NotNull;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.lang.reflect.InvocationTargetException;
import java.math.BigInteger;

public class ClientApplication {
    @NotNull
    private final static BufferedReader console = new BufferedReader(new InputStreamReader(System.in));
    private static Client client;

    private static void help() {
        System.out.println("Commands:");
        System.out.println("  help - help for commands");
        System.out.println("  connect - connect to server");
        System.out.println("  close - close connection");
        System.out.println("  check - check long operation result");
        System.out.println("  calc - send task on calculation");
        System.out.println("  exit - exit application");
    }

    public static void main(String[] args) throws IOException {
        help();
        while (true) {
            System.out.print("> ");
            final String line = console.readLine();
            switch (line) {
                case "help":
                    help();
                    break;
                case "connect":
                    try {
                        connect();
                    } catch (Exception ex) {
                        System.out.println("Error: Please check server location");
                    }
                    break;
                case "close":
                    close();
                    break;
                case "check":
                    try {
                        check();
                    } catch (Exception ex) {
                        System.out.println("Error: please check input");
                    }
                    break;
                case "calc":
                    try {
                        calc();
                    } catch (Exception ex) {
                        System.out.println("Error: please check input");
                    }
                    break;
                case "exit":
                    System.exit(0);
                default:
                    System.out.println("Error: Invalid command");
            }
        }
    }

    private static void calc() throws IOException, IllegalAccessException, InvocationTargetException, InstantiationException {
        if (client == null) {
            System.out.println("Error: Please check connection");
        }

        System.out.println("Operations:");
        System.out.println(" + => addition");
        System.out.println(" - => subtraction");
        System.out.println(" * => multiplication");
        System.out.println(" / => division");
        System.out.println(" fac => factorial");
        System.out.println(" sqrt => square root");

        final String operation = console.readLine();

        switch (operation) {
            case "+": {
                System.out.println("x: ");
                BigInteger x = new BigInteger(console.readLine());
                System.out.println("y: ");
                BigInteger y = new BigInteger(console.readLine());
                System.out.println(client.sendBinaryOperation("addition", x, y));
            }
            break;
            case "-": {
                System.out.println("x: ");
                BigInteger x = new BigInteger(console.readLine());
                System.out.println("y: ");
                BigInteger y = new BigInteger(console.readLine());
                System.out.println(client.sendBinaryOperation("subtraction", x, y));
            }
            break;
            case "*": {
                System.out.println("x: ");
                BigInteger x = new BigInteger(console.readLine());
                System.out.println("y: ");
                BigInteger y = new BigInteger(console.readLine());
                System.out.println(client.sendBinaryOperation("multiplication", x, y));
            }
            break;
            case "/": {
                System.out.println("x: ");
                BigInteger x = new BigInteger(console.readLine());
                System.out.println("y: ");
                BigInteger y = new BigInteger(console.readLine());
                System.out.println(client.sendBinaryOperation("division", x, y));
            }
            break;
            case "fac": {
                System.out.println("x: ");
                BigInteger x = new BigInteger(console.readLine());
                System.out.println(client.sendUnaryOperation("factorial", x));
            }
            break;
            case "sqrt": {
                System.out.println("x: ");
                BigInteger x = new BigInteger(console.readLine());
                System.out.println(client.sendUnaryOperation("square", x));
            }
            break;
            default:
                System.out.println("Error: invalid operation");
        }
    }

    private static void check() throws IOException, IllegalAccessException, InvocationTargetException, InstantiationException {
        if (client == null) {
            System.out.println("Error: Please check connection");
        }

        System.out.println("Id: ");
        BigInteger id = new BigInteger(console.readLine());
        System.out.println(client.checkResult(id));
    }

    private static void close() throws IOException {
        if (client == null) {
            System.out.println("Error: Please check connection");
        }

        client.close();
        client = null;
    }

    private static void connect() throws IOException {
        if (client != null) {
            close();
        }

        System.out.println("Host: ");
        final String host = console.readLine();
        System.out.println("Port: ");
        final String portString = console.readLine();
        short port = Short.parseShort(portString);
        client = new Client(host, port);
    }
}
