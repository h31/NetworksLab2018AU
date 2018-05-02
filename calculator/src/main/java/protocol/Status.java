package protocol;

import com.sun.istack.internal.NotNull;

public enum Status {
    HTTP_200("HTTP/1.1 200 OK"),
    HTTP_404("HTTP/1.1 404 Not Found");

    @NotNull
    private final String value;

    Status(@NotNull final String value) {
        this.value = value;
    }

    @Override
    public String toString() {
        return value;
    }
}
