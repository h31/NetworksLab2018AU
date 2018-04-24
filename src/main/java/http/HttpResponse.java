package http;

import org.json.JSONObject;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class HttpResponse extends HttpPacket {
    public final static int STATUS_OK = 200;
    public final static int STATUS_NOT_IMPLEMENTED = 501;
    private final int status;

    public HttpResponse(int status, JSONObject body) {
        super(buildStartLine(status), buildHeaders(body), body);
        this.status = status;
    }

    public HttpResponse(List<String> strings) {
        super(strings);
        final String[] splitStartLine = strings.get(0).split(" ");
        this.status = Integer.valueOf(splitStartLine[1]);
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
