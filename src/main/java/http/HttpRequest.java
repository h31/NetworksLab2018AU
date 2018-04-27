package http;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.IOException;
import java.net.URI;
import java.util.Collections;
import java.util.List;
import java.util.Map;

public class HttpRequest extends HttpPacket {

    private final URI uri;
    private final HttpMethod httpMethod;


    public HttpRequest(URI uri, HttpMethod httpMethod, JSONObject body) {
        super(buildStartLine(httpMethod, uri), buildHeaders(body), body);
        this.uri = uri;
        this.httpMethod = httpMethod;
    }

    public HttpRequest(BufferedReader br) throws IOException {
        super(br);
        final String[] splitStartLine = startLine.split(" ");
        this.httpMethod = HttpMethod.valueOf(splitStartLine[0]);
        this.uri = URI.create(splitStartLine[1]);
    }

    private static String buildStartLine(HttpMethod httpMethod, URI uri) {
        return httpMethod.name().toUpperCase() + " " + uri + " " + VERSION;
    }

    public URI getURI() {
        return uri;
    }

    public HttpMethod getMethod() {
        return httpMethod;
    }

    public JSONObject getBody() {
        return body;
    }
}
