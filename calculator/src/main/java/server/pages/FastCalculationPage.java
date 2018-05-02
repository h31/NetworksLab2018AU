package server.pages;

import com.sun.istack.internal.NotNull;
import protocol.HttpResponse;
import protocol.HttpResponseBuilder;
import protocol.Utility;

import java.io.UnsupportedEncodingException;
import java.lang.reflect.InvocationTargetException;
import java.math.BigInteger;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.Map;

public class FastCalculationPage {
    @NotNull
    private static final String HTML_START = "<html><title>Fast Calculation</title><body>";
    @NotNull
    private static final String HTML_END = "</body></html>";
    @NotNull
    private final HttpResponse response;

    public FastCalculationPage(@NotNull final String connection, @NotNull final String query) throws IllegalAccessException, InstantiationException, InvocationTargetException, URISyntaxException, UnsupportedEncodingException {
        Map<String, String> parameters = Utility.splitQuery(new URI(query));

        String r = "Error invalid parameters";
        if (parameters.size() == 3
                && parameters.containsKey("x")
                && parameters.containsKey("type")
                && parameters.containsKey("y")
                && parameters.get("x").length() < 100
                && parameters.get("y").length() < 100) {
            BigInteger x = new BigInteger(parameters.get("x"));
            BigInteger y = new BigInteger(parameters.get("y"));

            switch (parameters.get("type")) {
                case "addition":
                    r = x.add(y).toString();
                    break;
                case "subtraction":
                    r = x.subtract(y).toString();
                    break;
                case "multiplication":
                    r = x.multiply(y).toString();
                    break;
                case "division":
                    try {
                        r = x.divide(y).toString();
                    } catch (ArithmeticException e) {
                        r = "Error division by zero";
                    }
                    break;
            }
        }

        response = new HttpResponseBuilder()
                .setServer("Java server.ServerApplication")
                .setContentType("text/html")
                .setStatus(protocol.Status.HTTP_200)
                .setConnection(connection)
                .setBody(HTML_START + "<h1>" + r + "</h1>" + HTML_END)
                .build();
    }

    @Override
    public String toString() {
        return response.toString();
    }
}
