package http;

import org.json.JSONObject;
import server.StatusCode;

import java.io.BufferedReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class HttpResponse extends HttpPacket {
    private final StatusCode statusCode;

    public HttpResponse(StatusCode statusCode, JSONObject body) {
        super(buildStartLine(statusCode), buildHeaders(body), body);
        this.statusCode = statusCode;
    }

    public HttpResponse(BufferedReader br) throws IOException {
        super(br);
        final String[] splitStartLine = startLine.split(" ");
            this.statusCode = StatusCode.buildStatusCode(Integer.valueOf(splitStartLine[1]));
    }

    private static String buildStartLine(StatusCode statusCode) {
        return VERSION + " " + statusCode.getCode();
    }

    public StatusCode getStatusCode() {
        return statusCode;
    }
}
