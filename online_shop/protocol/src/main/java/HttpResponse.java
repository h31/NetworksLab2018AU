package src.main.java;

import java.io.*;
import java.net.SocketTimeoutException;
import java.util.ArrayList;
import java.util.List;

public class HttpResponse {
    private static String VERSION = "HTTP/1.1";
    private int status;
    private String statusDescription;
    private List<String> body;

    public HttpResponse(int status, List<String> body) {
        this.status = status;
        this.body = body;
        this.statusDescription = "NO DESCRIPTION";
    }

    public HttpResponse(int status, String statusDescription, List<String> body) {
        this.status = status;
        this.body = body;
        this.statusDescription = statusDescription;
    }

    public int getStatus() {
        return status;
    }

    public void setStatus(int status) {
        this.status = status;
    }

    public List<String> getBody() {
        return body;
    }

    public void setBody(List<String> body) {
        this.body = body;
    }

    public static HttpResponse parse(InputStream is) throws IOException {
        BufferedReader in = new BufferedReader(new InputStreamReader(is));
        String line = in.readLine();
        String[] parts = line.split(" ");
        String status = parts[1];
        int statusInt = Integer.parseInt(status);

        String statusDescription = parts[2];

        // skip headers
        while (!"".equals(in.readLine())) {
        }

        List<String> body = new ArrayList<>();
        try {
            while ((line = in.readLine()) != null) {

                body.add(line);

            }
        } catch (SocketTimeoutException ignored) {
        }

        return new HttpResponse(statusInt, statusDescription, body);
    }

    public void dump(OutputStream os) throws IOException {
        BufferedWriter out = new BufferedWriter(new OutputStreamWriter(os, "UTF8"));
        String header = VERSION + " " + status + " " + statusDescription + "\r\n";
        out.write(header);
        out.write("\r\n");
        for (String line : body) {
            out.write(line + "\r\n");
        }
        out.write("\r\n");
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
