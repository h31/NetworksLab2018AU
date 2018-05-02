package protocol;

import com.sun.istack.internal.NotNull;

public class HttpResponse {

    private final String data;

    private HttpResponse(@NotNull final String data) {
        this.data = data;
    }

    @Override
    public String toString() {
        return data;
    }
}
