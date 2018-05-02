package server.pages;

import com.sun.istack.internal.NotNull;
import protocol.HttpResponse;
import protocol.HttpResponseBuilder;

import java.lang.reflect.InvocationTargetException;

public class HomePage {
    @NotNull
    private static final String HTML_START = "<html><title>Home</title><body>";
    @NotNull
    private static final String HTML_END = "</body></html>";
    @NotNull
    private final HttpResponse response;

    public HomePage(@NotNull final String connection) throws IllegalAccessException, InstantiationException, InvocationTargetException {
        response = new HttpResponseBuilder()
                .setServer("Java server.ServerApplication")
                .setContentType("text/html")
                .setStatus(protocol.Status.HTTP_200)
                .setConnection(connection)
                .setBody(HTML_START + "<h1>Home Page</h1>" +
                        "Fast operations:" +
                        "<ul>\n" +
                        "  <li>Addition. <br \\> <b>Example</b>: <a href=\"fast.html?type=addition&x=3&y=5\">fast.html?type=addition&x=3&y=5</a></li>\n" +
                        "  <li>Subtraction. <br \\> <b>Example</b>: <a href=\"fast.html?type=subtraction&x=3&y=5\">fast.html?type=subtraction&x=3&y=5</a></li>\n" +
                        "  <li>Multiplication. <br \\> <b>Example</b>: <a href=\"fast.html?type=multiplication&x=3&y=5\">fast.html?type=multiplication&x=3&y=5</a></li>\n" +
                        "  <li>Division. <br \\> <b>Example</b>: <a href=\"fast.html?type=division&x=3&y=5\">fast.html?type=division&x=3&y=5</a></li>\n" +
                        "</ul>" +
                        "Long operations:" +
                        "<ul>\n" +
                        "  <li>Factorial. <br \\> <b>Example</b>: <a href=\"long.html?type=factorial&x=3\">long.html?type=factorial&x=3</a></li>\n" +
                        "  <li>Square root. <br \\> <b>Example</b>: <a href=\"long.html?type=square&x=3\">long.html?type=square&x=3</a></li>\n" +
                        "</ul>"
                        + HTML_END)
                .build();
    }

    @Override
    public String toString() {
        return response.toString();
    }
}
