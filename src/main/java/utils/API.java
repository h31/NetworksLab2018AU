package utils;

import http.HttpMethod;
import http.HttpRequest;
import http.HttpResponse;
import server.commandrunner.*;
import utils.request.*;
import utils.response.*;

public enum API {

    REGISTER(HttpMethod.POST, "user"),
    GET_LOTS(HttpMethod.GET, "lot"),
    BET(HttpMethod.POST, "bet"),
    NEW_LOT(HttpMethod.POST, "lot"),
    STOP_LOT(HttpMethod.DELETE, "lot");

    private final HttpMethod httpMethod;
    private final String uriStart;

    API(HttpMethod httpMethod, String uriStart) {
        this.httpMethod = httpMethod;
        this.uriStart = uriStart;
    }

    public HttpMethod getHttpMethod() {
        return httpMethod;
    }

    public String getUriStart() {
        return uriStart;
    }

    public ResponseCommand buildResponse(HttpResponse httpResponse) {
        switch (this) {
            case REGISTER: return new RegisterResponseCommand(httpResponse);
            case GET_LOTS: return new GetLotsResponseCommand(httpResponse);
            case BET: return new BetResponseCommand(httpResponse);
            case NEW_LOT: return new NewLotResponseCommand(httpResponse);
            case STOP_LOT: return new StopLotResponseCommand(httpResponse);

            default: throw new IllegalStateException("no such API");
        }
    }

    public RequestCommand buildRequest(HttpRequest httpRequest) {
        switch (this) {
            case REGISTER: return new RegisterRequestCommand(httpRequest);
            case GET_LOTS: return new GetLotsRequestCommand(httpRequest);
            case BET: return new BetRequestCommand(httpRequest);
            case NEW_LOT: return new NewLotRequestCommand(httpRequest);
            case STOP_LOT: return new StopLotRequestCommand(httpRequest);

            default: throw new IllegalStateException("no such API");
        }
    }

    public CommandRunner getCommandRunner() {
        switch (this) {
            case REGISTER: return RegisterCommandRunner.getInstance();
            case GET_LOTS: return GetLotsCommandRunner.getInstance();
            case BET: return BetCommandRunner.getInstance();
            case NEW_LOT: return NewLotCommandRunner.getInstance();
            case STOP_LOT: return StopLotCommandRunner.getInstance();

            default: throw new IllegalStateException("no such API");
        }
    }

    public static API buildAPI(HttpMethod httpMethod, String uriStart) throws NotImplementedException {
        for (API api: API.values()) {
            if (api.httpMethod.equals(httpMethod) && api.uriStart.equals(uriStart)) {
                return api;
            }
        }
        throw new NotImplementedException(httpMethod + " " + uriStart);
    }
}
