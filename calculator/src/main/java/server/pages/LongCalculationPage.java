package server.pages;

import com.sun.istack.internal.NotNull;
import protocol.HttpResponse;
import protocol.HttpResponseBuilder;
import protocol.Utility;
import server.LongTaskPool;

import java.io.UnsupportedEncodingException;
import java.lang.reflect.InvocationTargetException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.Map;
import java.util.concurrent.Future;

public class LongCalculationPage {

    @NotNull
    private static final String HTML_START = "<html><title>Home</title><body>";
    @NotNull
    private static final String HTML_END = "</body></html>";
    @NotNull
    private final HttpResponse response;

    public LongCalculationPage(@NotNull final String connection, @NotNull final String query, @NotNull final LongTaskPool longTaskPool) throws URISyntaxException, UnsupportedEncodingException, IllegalAccessException, InstantiationException, InvocationTargetException {

        Map<String, String> parameters = Utility.splitQuery(new URI(query));

        String r = "Error long task invalid parameters";
        if (parameters.size() == 1
                && parameters.containsKey("id")
                && parameters.get("id").length() < 10) {
            try {
                Integer id = Integer.parseInt(parameters.get("id"));
                if (!longTaskPool.contains(id)) {
                    r = "id is not exists";
                } else {
                    Future<String> result = longTaskPool.get(id);
                    if (!result.isDone()) {
                        r = "In progress";
                    } else {
                        r = result.get();
                    }
                }

            } catch (Exception e) {
                r = "Invalid id";
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
