package ru.spbau.mit;

import java.util.Arrays;
import java.util.List;

public class Protocol {
    public static final String HTTP_VERSION = "1.1";
    public static final String CURRENT_PROTOCOL = "HTTP/" + HTTP_VERSION;
    public static final String VERSION = "1.0";
    public static final String CLIENT_INIT_REQUEST_HEADER = "GET /this_client/role " + CURRENT_PROTOCOL;
    public static final String CLIENT_LIST_REQUEST_HEADER = "GET /lots " + CURRENT_PROTOCOL;
    public static final String CLIENT_EXIT_REQUEST_HEADER = "PUT /this_client/exit " + CURRENT_PROTOCOL;
    public static final String CLIENT_FINISH_REQUEST_HEADER = "PUT /finish " + CURRENT_PROTOCOL;
    public static final String CLIENT_ADD_REQUEST_HEADER = "PUT /add_lot " + CURRENT_PROTOCOL;
    public static final String CLIENT_BET_REQUEST_HEADER = "PUT /bet_lot " + CURRENT_PROTOCOL;

    public static final String HEADER_AND_BODY_DELIMITER = "\n\n";
    public static final String LINE_DELIMITER = "\n";
    public static final String INLINE_DELIMITER = "&";
    public static final String SERVER_OK_RESPONSE_HEADER = CURRENT_PROTOCOL + " 200 OK";

    public enum ClientRole {
        ADMIN,
        PARTICIPANT;

        String roleString() {
            return toString().toLowerCase();
        }
    }

    public static class ProtocolException extends Exception {
        ProtocolException(String msg) {
            super(msg);
        }
//        ProtocolException() {
//            super("Something failed in protocol.");
//        }
    }

    public static String clientInitRequest(ClientRole clientRole) {
        return CLIENT_INIT_REQUEST_HEADER + HEADER_AND_BODY_DELIMITER + clientRole.roleString();
    }

    public static void checkServerOk(String responseHeader) throws ProtocolException {
        if (!responseHeader.equals(SERVER_OK_RESPONSE_HEADER)) {
            throw new ProtocolException("Unexpected response from server: " + responseHeader);
        }
    }

    public static String serverFailResponse(String message, boolean isServerFault) {
        String header = CURRENT_PROTOCOL + (isServerFault ? " 504 FAIL" : " 200 FAIL");
        return header + HEADER_AND_BODY_DELIMITER + message;
    }

    public static String[] splitOnHeaderAndBody(String request) {
        String[] headerAndBody = request.split(HEADER_AND_BODY_DELIMITER);
        if (headerAndBody.length > 2) {
            throw new RuntimeException("Wrong request");
        }
        return headerAndBody;
    }

    public static String getErrorMessageFromResponse(String response) {
        String[] headerAndBody = response.split(HEADER_AND_BODY_DELIMITER);
        if (headerAndBody.length > 2) {
            return response;
        } else if (headerAndBody.length == 2) {
            return headerAndBody[1];
        } else {
            return "Empty error message";
        }
    }
}
