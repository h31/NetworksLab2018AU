package utils.request;

import http.HttpRequest;
import org.json.JSONObject;
import utils.API;
import utils.data.User;
import utils.data.UserRole;

import java.net.URI;
import java.nio.file.Paths;

public class RegisterRequestCommand implements RequestCommand {

    private static final API api = API.REGISTER;

    private final User user;


    public RegisterRequestCommand(User user) {
        this.user = user;
    }

    public RegisterRequestCommand(HttpRequest httpRequest) {
        final String[] parts = httpRequest.getURI().getPath().split("/");
        this.user = new User(parts[1], UserRole.valueOf(parts[2]));
    }

    @Override
    public HttpRequest toHttpRequest() {
        return new HttpRequest(buildUri(), api.getHttpMethod(), new JSONObject());
    }

    private URI buildUri() {
        return URI.create(Paths.get(
                "/" + api.getUriStart())
                .resolve(user.getName())
                .resolve(user.getRole().name())
                .toString());
    }

    @Override
    public API getAPI() {
        return api;
    }
}
