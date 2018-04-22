package http;

import org.json.JSONObject;

import java.util.HashMap;
import java.util.Map;

public class HttpResponse extends HttpPacket {
    public final static int STATUS_OK = 200;
    private final int status;

    public HttpResponse(int status, JSONObject body) {
        super(buildStartLine(status), buildHeaders(body), body);
        this.status = status;
    }


    private static String buildStartLine(int status) {
        return VERSION + " " + status;
    }

    private static Map<String,String> buildHeaders(JSONObject body) {
        Map<String, String> headers = new HashMap<>();
        headers.put(CONTENT_LENGTH, Integer.toString(body.length()));
        return headers;
    }

    public int getStatus() {
        return status;
    }
}
