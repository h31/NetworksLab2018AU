package protocol;

import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.stream.Collectors;

public abstract class HttpPacket {

    protected static final String VERSION = "HTTP/1.1";
    protected List<String> body;

    public HttpPacket() {
        this.body = Collections.emptyList();
    }

    public HttpPacket(List<String> body) {
        this.body = body;
    }

    public HttpPacket(JSONObject jsonObject) {
        this.body = Collections.singletonList(jsonObject.toString());
    }

    public List<String> getBody() {
        return body;
    }

    public JSONObject getJSONBody() {
        String concatenatedBody = body.stream().collect(Collectors.joining());
        return new JSONObject(concatenatedBody);
    }

    public void setBody(List<String> body) {
        this.body = body;
    }

    public void setJSONBody(JSONObject jsonObject) {
        this.body = Collections.singletonList(jsonObject.toString());
    }

    public static List<String> parseBody(BufferedReader in) throws IOException {
        List<String> body = new ArrayList<>();
        String line;
        while (!"".equals(line = in.readLine())) {
            System.out.println(" read line in packet " + line);
            body.add(line);
        }
        return body;
    }

    protected void dumpBody(BufferedWriter out) throws IOException {
        for (String line : body) {
            System.out.println("dump body line " + line);
            out.write(line + "\r\n");
        }
        out.write("\r\n");
    }
}
