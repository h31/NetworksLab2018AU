package protocol;

import com.sun.istack.internal.NotNull;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.HashMap;
import java.util.Map;
import java.util.stream.Collectors;

public class HttpRequestBuilder {

    @NotNull
    private final static String DELIMITER = "\r\n";
    @NotNull
    final Map<String, String> variables = new HashMap<>();
    @NotNull
    private final Map<Header, String> parameters = new HashMap<>();
    @NotNull
    private String page = "";

    @NotNull
    public HttpRequestBuilder setVariable(@NotNull final String name, @NotNull final String value) {
        variables.put(name, value);
        return this;
    }

    @NotNull
    public HttpRequestBuilder setPage(@NotNull final String page) {
        this.page = page;
        return this;
    }

    @NotNull
    public HttpRequestBuilder setHost(@NotNull final String host) {
        parameters.put(Header.HOST, host);
        return this;
    }

    @NotNull
    public HttpRequestBuilder setServer(@NotNull final String value) {
        parameters.put(Header.SERVER, value);
        return this;
    }

    @NotNull
    public HttpRequestBuilder setConnection(@NotNull final String value) {
        parameters.put(Header.CONNECTION, value);
        return this;
    }

    @NotNull
    public HttpRequestBuilder setContentLength(@NotNull final Integer value) {
        parameters.put(Header.CONTENT_LENGTH, value.toString());
        return this;
    }

    @NotNull
    public HttpRequestBuilder setContentType(@NotNull final String value) {
        parameters.put(Header.CONTENT_TYPE, value);
        return this;
    }

    @NotNull
    public HttpRequest build() throws IllegalAccessException, InvocationTargetException, InstantiationException {
        final Constructor<HttpRequest> constructor = (Constructor<HttpRequest>) HttpRequest.class.getDeclaredConstructors()[0];
        constructor.setAccessible(true);

        final String stringParameters = parameters
                .entrySet()
                .stream()
                .map(x -> x.getKey() + ": " + x.getValue() + DELIMITER)
                .collect(Collectors.joining());

        String stringVariables = variables
                .entrySet()
                .stream()
                .map(x -> x.getKey() + "=" + x.getValue())
                .collect(Collectors.joining("&"));

        if (!variables.isEmpty()) {
            stringVariables = "?" + stringVariables;
        }

        return constructor.newInstance("GET /" + page + stringVariables + " HTTP/1.1"  + DELIMITER + stringParameters + DELIMITER + DELIMITER);
    }
}
