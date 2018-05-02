package protocol;

import com.sun.istack.internal.NotNull;

public enum Header {
    SERVER("server.ServerApplication"),
    CONNECTION("Connection"),
    CONTENT_LENGTH("Content-Length"),
    CONTENT_TYPE("Content-Type"),
    HOST("Host");

    @NotNull
    private final String value;

    Header(@NotNull final String value) {
        this.value = value;
    }

    @Override
    public String toString() {
        return value;
    }
}
