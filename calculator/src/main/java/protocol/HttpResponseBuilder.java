package protocol;

import com.sun.istack.internal.NotNull;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.HashMap;
import java.util.Map;
import java.util.stream.Collectors;

public class HttpResponseBuilder {
    @NotNull
    private Status status = Status.HTTP_200;

    @NotNull
    private final static String DELIMITER = "\r\n";

    @NotNull
    private String body = "";

    @NotNull
    private final Map<Header, String> parameters = new HashMap<>();

    @NotNull
    public HttpResponseBuilder setStatus(@NotNull final Status status) {
        this.status = status;
        return this;
    }

    @NotNull
    public HttpResponseBuilder setServer(@NotNull final String value) {
        parameters.put(Header.SERVER, value);
        return this;
    }

    @NotNull
    public HttpResponseBuilder setConnection(@NotNull final String value) {
        parameters.put(Header.CONNECTION, value);
        return this;
    }

    @NotNull
    public HttpResponseBuilder setContentLength(@NotNull final Integer value) {
        parameters.put(Header.CONTENT_LENGTH, value.toString());
        return this;
    }

    @NotNull
    public HttpResponseBuilder setContentType(@NotNull final String value) {
        parameters.put(Header.CONTENT_TYPE, value);
        return this;
    }

    @NotNull
    public HttpResponseBuilder setBody(@NotNull final String value) {
        body = value;
        setContentLength(body.length());
        return this;
    }

    @NotNull
    public HttpResponse build() throws IllegalAccessException, InvocationTargetException, InstantiationException {
        final Constructor<HttpResponse> constructor = (Constructor<HttpResponse>) HttpResponse.class.getDeclaredConstructors()[0];
        constructor.setAccessible(true);

        final String stringParameters = parameters
                .entrySet()
                .stream()
                .map(x -> x.getKey() + ": " + x.getValue() + DELIMITER)
                .collect(Collectors.joining());

        return constructor.newInstance(status + DELIMITER + stringParameters + DELIMITER + DELIMITER + body);
    }
}
