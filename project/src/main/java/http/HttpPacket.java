package http;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public abstract class HttpPacket {

    private final static int MAX_BUF_SIZE = 1024;

    protected final static String VERSION = "HTTP/1.1";
    protected final static String CONTENT_LENGTH = "Content-Length";

    protected final String startLine;
    protected final Map<String, String> headers;
    protected final JSONObject body;

    protected HttpPacket(BufferedReader br) throws IOException {


        String startLine = br.readLine();
        while (startLine.isEmpty()) {
            startLine = br.readLine();
        }
        this.startLine = startLine;
        headers = readHeaders(br);

        int toRead = getToRead();
        if (toRead == 0) {
            body = new JSONObject();
            return;
        }

        final StringBuilder bodyBuilder = readBody(br, toRead);

        try {
            body = new JSONObject(bodyBuilder.toString());
        } catch (JSONException e) {
            throw new IllegalStateException(e);
        }
    }

    private int getToRead() throws IOException {
        int toRead = 0;
        if (headers.containsKey(CONTENT_LENGTH)) {
            try {
                toRead = Integer.valueOf(headers.get(CONTENT_LENGTH));
            } catch (NumberFormatException e) {
                throw new IOException("content length should be an integer");
            }
            if (toRead < 0) {
                throw new IOException("content length is negative");
            }
        }
        return toRead;
    }

    private Map<String, String> readHeaders(BufferedReader br) throws IOException {
        final Map<String, String> headers = new HashMap<>();
        while (true) {
            final String current = br.readLine();
            if (current.isEmpty()) {
                break;
            }
            int colonIndex = current.indexOf(':');
            String key = current.substring(0, colonIndex);
            String value = current.substring(colonIndex + 2);
            headers.put(key, value);
        }
        return headers;
    }

    private StringBuilder readBody(BufferedReader br, int toRead) throws IOException {
        final StringBuilder bodyBuilder = new StringBuilder();
        int read = 0;
        final char[] buffer = new char[MAX_BUF_SIZE];
        while (read < toRead) {
            int r = br.read(buffer, 0, toRead - read);
            if (r >= 0) {
                for (int i = 0; i < r; i++) {
                    bodyBuilder.append(buffer[i]);
                }
                read += r;
            }
        }
        return bodyBuilder;
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

    protected static Map<String,String> buildHeaders(JSONObject body) {
        final Map<String, String> headers = new HashMap<>();
        headers.put(CONTENT_LENGTH, String.valueOf(body.toString().length()));
        return headers;
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
