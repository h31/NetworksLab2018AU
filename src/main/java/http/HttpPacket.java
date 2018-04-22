package http;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public abstract class HttpPacket {
    private final static String NEWLINE = "\r\n";

    protected final static String VERSION = "HTTP/1.1";
    protected final static String CONTENT_LENGTH = "content-length";

    protected final String startLine;
    protected final Map<String, String> headers;
    protected final JSONObject body;

    protected HttpPacket(List<String> strings) {
        startLine = strings.get(0);
        headers = new HashMap<>();
        for (int i = 1; i < strings.size() - 3; i++) {
            String current = strings.get(i);
            int colonIndex = current.indexOf(':');
            String key = current.substring(0, colonIndex);
            String value = current.substring(colonIndex + 2);
            headers.put(key, value);
        }
        try {
            body = new JSONObject(strings.get(strings.size() - 1));
        } catch (JSONException e) {
            throw new IllegalStateException(e);
        }

    }

    protected HttpPacket(String startLine, Map<String, String> headers, JSONObject body) {
        this.startLine = startLine;
        this.headers = headers;
        this.body = body;
    }

    public List<String> toStrings() {
        final List<String> strings = new ArrayList<>();
        strings.add(startLine);
        for (Map.Entry<String, String> entry : headers.entrySet()) {
            strings.add(entry.getKey() + ": " + entry.getValue());
        }
        strings.add("");
        strings.add(body.toString());
        return strings;
    }

    public String getStartLine() {
        return startLine;
    }

    public Map<String, String> getHeaders() {
        return headers;
    }

    public JSONObject getBody() {
        return body;
    }
}
