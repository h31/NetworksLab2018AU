package protocol;

import com.sun.istack.internal.NotNull;

public class HttpRequest {
    private final String data;

    private HttpRequest(@NotNull final String data) {
        this.data = data;
    }

    @Override
    public String toString() {
        return data;
    }
}
