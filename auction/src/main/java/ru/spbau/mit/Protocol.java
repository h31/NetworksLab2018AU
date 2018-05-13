package ru.spbau.mit;

import java.util.Arrays;
import java.util.List;

public class Protocol {
    public static final String HTTP_VERSION = "1.1";
    public static final String CURRENT_PROTOCOL = "HTTP/" + HTTP_VERSION;
    public static final String VERSION = "1.0";

    public enum ClientRole {
        ADMIN,
        PARTICIPANT;

        public String roleString() {
            if (this == ADMIN) {
                return "admin";
            } else if (this == PARTICIPANT) {
                return "participant";
            } else {
                return null;
            }
        }

        public static List<String> roleStrings = Arrays.asList(ADMIN.roleString(), PARTICIPANT.roleString());
    }

    public enum ClientRequest {
        INITIATE;

        public String requestString() {
            switch (this) {
                case INITIATE:
                    return "init";
            }
            throw new RuntimeException("Unknown request");
        }

    }

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
        return "GET /this_client/role " + CURRENT_PROTOCOL
                + "\n\n" + clientRole.roleString();
    }

    public static void checkServerOk(String serverInitResponse) throws ProtocolException {
        if (!serverInitResponse.equals(CURRENT_PROTOCOL + " 200 OK")) {
            throw new ProtocolException("Unexpected response from server: " + serverInitResponse);
        }
    }
}
