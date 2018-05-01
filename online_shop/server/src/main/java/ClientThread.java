import org.jetbrains.annotations.NotNull;
import org.json.JSONObject;
import protocol.HttpRequest;
import protocol.HttpResponse;

import java.io.IOException;

public abstract class ClientThread implements Runnable {
    protected abstract void handlerHELLO(@NotNull HttpRequest request) throws IOException;

    protected abstract void handlerGET(@NotNull HttpRequest request) throws IOException;

    protected abstract void handlerADD(@NotNull HttpRequest request) throws IOException;

    protected abstract void handlerPAY(@NotNull HttpRequest request) throws IOException;

    protected abstract boolean handlerBYE(@NotNull HttpRequest request) throws IOException;
}
