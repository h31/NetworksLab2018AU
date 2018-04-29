import java.io.*;

public class HttpRequest {
    private String type;
    private String url;
    private static String VERSION = "HTTP/1.1";

    public HttpRequest(String type, String url) {
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
        return new HttpRequest(parts[0], parts[1]);
    }

    public void dump(OutputStream os) throws IOException {
        BufferedWriter out = new BufferedWriter(new OutputStreamWriter(os, "UTF8"));
        String request = type + " " + url + " " + VERSION + "\r\n";
        out.write(request);
        out.write("\r\n");
        out.flush();
    }

    @Override
    public String toString() {
        return "HttpRequest{" +
                "type='" + type + '\'' +
                ", url='" + url + '\'' +
                '}';
    }
}
