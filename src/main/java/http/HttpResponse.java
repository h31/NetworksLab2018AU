package http;

import org.json.JSONException;
import org.json.JSONObject;
import server.StatusCode;
import utils.UnknownStatusCodeException;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class HttpResponse extends HttpPacket {
    private final StatusCode statusCode;

    public HttpResponse(StatusCode statusCode, JSONObject body) {
        super(buildStartLine(statusCode), buildHeaders(body), body);
        this.statusCode = statusCode;
    }

    public HttpResponse(List<String> strings) throws JSONException, UnknownStatusCodeException {
        super(strings);
        final String[] splitStartLine = strings.get(0).split(" ");
        this.statusCode = StatusCode.buildStatusCode(Integer.valueOf(splitStartLine[1]));
    }

    private static String buildStartLine(StatusCode statusCode) {
        return VERSION + " " + statusCode.getCode();
    }

    private static Map<String,String> buildHeaders(JSONObject body) {
        Map<String, String> headers = new HashMap<>();
        headers.put(CONTENT_LENGTH, Integer.toString(body.length()));
        return headers;
    }

    public StatusCode getStatusCode() {
        return statusCode;
    }
}
