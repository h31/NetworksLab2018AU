package protocol;

import org.json.JSONObject;

import java.io.*;
import java.util.List;

public class HttpRequest extends HttpPacket {
    private String type;
    private String url;

    public HttpRequest(String type, String url) {
        this.type = type;
        this.url = url;
    }

    public HttpRequest(String type, String url, List<String> body) {
        super(body);
        this.type = type;
        this.url = url;
    }

    public HttpRequest(String type, String url, JSONObject jsonObject) {
        super(jsonObject);
        this.type = type;
        this.url = url;
    }

    public String getType() {
        return type;
    }

    public void setType(String type) {
        this.type = type;
    }

    public String getUrl() {
        return url;
    }

    public void setUrl(String url) {
        this.url = url;
    }

    public static HttpRequest parse(InputStream is) throws IOException {
        BufferedReader in = new BufferedReader(new InputStreamReader(is));
        String line = in.readLine();
        String[] parts = line.split(" ");
        String type = parts[0];
        String url = parts[1];
        System.out.println("line " + line);
        List<String> body = parseBody(in);
        return new HttpRequest(type, url, body);
    }

    public void dump(OutputStream os) throws IOException {
        BufferedWriter out = new BufferedWriter(new OutputStreamWriter(os, "UTF8"));

        String request = type + " " + url + " " + VERSION + "\r\n";
        out.write(request);

        out.write("\r\n");
        dumpBody(out);
        out.flush();
    }

    @Override
    public String toString() {
        return "HttpRequest{" +
                "type='" + type + '\'' +
                ", url='" + url + '\'' +
                ", body=" + body +
                '}';
    }
}
