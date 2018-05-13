package ru.spbau.mit;

import java.util.Arrays;
import java.util.List;

public class Protocol {
    public static final String HTTP_VERSION = "1.1";
    public static final String CURRENT_PROTOCOL = "HTTP/" + HTTP_VERSION;
    public static final String VERSION = "1.0";
    public static final String CLIENT_INIT_REQUEST_HEADER = "GET /this_client/role " + CURRENT_PROTOCOL;
    public static final String CLIENT_LIST_REQUEST_HEADER = "GET /lots " + CURRENT_PROTOCOL;
    public static final String HEADER_AND_BODY_DELIMITER = "\n\n";
    public static final String SERVER_OK_RESPONSE_HEADER = CURRENT_PROTOCOL + " 200 OK";
    public static final String SERVER_ERROR_RESPONSE_HEADER = "504";

    public enum ClientRole {
        ADMIN,
        PARTICIPANT;

        String roleString() {
            return toString().toLowerCase();
        }
    }

//    public enum ClientRequest {
//        INITIATE;
//
//        public String requestString() {
//            switch (this) {
//                case INITIATE:
//                    return "init";
//            }
//            throw new RuntimeException("Unknown request");
//        }
//    }

    public static class ProtocolException extends Exception {
        ProtocolException(String msg) {
            super(msg);
        }
        ProtocolException() {
            super("Something failed in protocol.");
        }
    }

//    public static String clientGetIdRequest() {
//        return "GET /client_id HTTP" + HTTP_VERSION;
//    }

    public static String clientInitRequest(ClientRole clientRole) {
        return CLIENT_INIT_REQUEST_HEADER + HEADER_AND_BODY_DELIMITER + clientRole.roleString();
    }

    public static void checkServerOk(String responseHeader) throws ProtocolException {
        if (!responseHeader.equals(SERVER_OK_RESPONSE_HEADER)) {
            throw new ProtocolException("Unexpected response from server: " + responseHeader);
        }
    }

    public static String[] splitOnHeaderAndBody(String request) {
        String[] headerAndBody = request.split(HEADER_AND_BODY_DELIMITER);
        if (headerAndBody.length > 2) {
            throw new RuntimeException("Wrong request");
        }
        return headerAndBody;
    }
}
