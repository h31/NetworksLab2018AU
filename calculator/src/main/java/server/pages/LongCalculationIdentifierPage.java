package server.pages;

import com.sun.istack.internal.NotNull;
import protocol.HttpResponse;
import protocol.HttpResponseBuilder;
import protocol.Utility;
import server.LongTaskPool;

import java.io.UnsupportedEncodingException;
import java.lang.reflect.InvocationTargetException;
import java.math.BigInteger;
import java.net.URI;
import java.net.URISyntaxException;
import java.time.LocalDateTime;
import java.util.Map;

public class LongCalculationIdentifierPage {
    @NotNull
    private static final String HTML_START = "<html><title>Home</title><body>";
    @NotNull
    private static final String HTML_END = "</body></html>";
    @NotNull
    private final HttpResponse response;

    public LongCalculationIdentifierPage(long longTaskLimit, @NotNull final String connection, @NotNull final String query, @NotNull LongTaskPool longTaskPool) throws URISyntaxException, UnsupportedEncodingException, IllegalAccessException, InstantiationException, InvocationTargetException {

        Map<String, String> parameters = Utility.splitQuery(new URI(query));

        String r = "Error long task invalid parameters";
        if (parameters.size() == 2
                && parameters.containsKey("x")
                && parameters.containsKey("type")
                && parameters.get("x").length() < 100) {
            BigInteger x = new BigInteger(parameters.get("x"));
            if (x.compareTo(BigInteger.valueOf(0)) > 0) {
                switch (parameters.get("type")) {
                    case "factorial":
                        r = longTaskPool.add(() -> fac(longTaskLimit, x).toString()).toString();
                        break;
                    case "square":
                        r = longTaskPool.add(() -> sqrt(longTaskLimit, x).toString()).toString();
                        break;
                }
            }
        }
        response = new HttpResponseBuilder()
                .setServer("Java server.ServerApplication")
                .setContentType("text/html")
                .setStatus(protocol.Status.HTTP_200)
                .setConnection(connection)
                .setBody(HTML_START + "<h1><a href=\"task.html?id=" + r + "\">" + r + "</a></h1>" + HTML_END)
                .build();
    }

    private static BigInteger sqrt(long longTaskLimit, BigInteger x) throws InterruptedException {
        long begin = System.currentTimeMillis();
        BigInteger div = BigInteger.ZERO.setBit(x.bitLength() / 2);
        BigInteger div2 = div;
        for (; ; ) {
            BigInteger y = div.add(x.divide(div)).shiftRight(1);
            if (y.equals(div) || y.equals(div2))
                return y;
            div2 = div;
            div = y;
            if (System.currentTimeMillis() - begin > longTaskLimit) {
                throw new InterruptedException();
            }
        }
    }

    private static BigInteger fac(long longTaskLimit, BigInteger x) throws InterruptedException {
        long begin = System.currentTimeMillis();
        BigInteger r = BigInteger.valueOf(1);
        for (BigInteger i = BigInteger.valueOf(1); i.compareTo(x) <= 0; i = i.add(BigInteger.valueOf(1))) {
            r = r.multiply(i);
            if (System.currentTimeMillis() - begin > longTaskLimit) {
                throw new InterruptedException();
            }
        }
        return r;
    }

    @Override
    public String toString() {
        return response.toString();
    }
}
