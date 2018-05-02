package server.pages;

import com.sun.istack.internal.NotNull;
import protocol.HttpResponse;
import protocol.HttpResponseBuilder;

import java.lang.reflect.InvocationTargetException;

public class $404Page {
    @NotNull
    private static final String HTML_START = "<html><title>404 Error</title><body>";
    @NotNull
    private static final String HTML_END = "</body></html>";
    @NotNull
    private final HttpResponse response;

    public $404Page(@NotNull final String connection) throws IllegalAccessException, InstantiationException, InvocationTargetException {
        response = new HttpResponseBuilder()
                .setServer("Java server.ServerApplication")
                .setContentType("text/html")
                .setStatus(protocol.Status.HTTP_404)
                .setConnection(connection)
                .setBody(HTML_START + "<h1>The Requested resource not found.</h1>" + HTML_END)
                .build();
    }

    @Override
    public String toString() {
        return response.toString();
    }
}
