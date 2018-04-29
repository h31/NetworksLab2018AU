import org.json.JSONObject;

import java.io.*;
import java.util.List;

public class HttpResponse extends HttpPacket {
    private int status;
    private String statusDescription;


    public HttpResponse(int status, List<String> body) {
        super(body);
        this.status = status;
        this.statusDescription = "NO DESCRIPTION";
    }

    public HttpResponse(int status, JSONObject jsonObject) {
        super(jsonObject);
        this.status = status;
        this.statusDescription = "NO DESCRIPTION";
    }

    public HttpResponse(int status, String statusDescription, List<String> body) {
        super(body);
        this.status = status;
        this.statusDescription = statusDescription;
    }

    public HttpResponse(int status, String statusDescription, JSONObject jsonObject) {
        super(jsonObject);
        this.status = status;
        this.statusDescription = statusDescription;
    }

    public int getStatus() {
        return status;
    }

    public void setStatus(int status) {
        this.status = status;
    }


    public static HttpResponse parse(InputStream is) throws IOException {
        BufferedReader in = new BufferedReader(new InputStreamReader(is));

        String line = in.readLine();
        String[] parts = line.split(" ");
        int status = Integer.parseInt(parts[1]);
        String statusDescription = parts[2];

        // skip headers
        while (!"".equals(in.readLine())) {}

        List<String> body = HttpPacket.parseBody(in);

        return new HttpResponse(status, statusDescription, body);
    }

    public void dump(OutputStream os) throws IOException {
        BufferedWriter out = new BufferedWriter(new OutputStreamWriter(os, "UTF8"));

        String header = VERSION + " " + status + " " + statusDescription + "\r\n";
        out.write(header);

        out.write("\r\n");

        dumpBody(out);

        out.flush();
    }

    @Override
    public String toString() {
        return "HttpResponse{" +
                "status=" + status +
                ", statusDescription='" + statusDescription + '\'' +
                ", body=" + body +
                '}';
    }
}
