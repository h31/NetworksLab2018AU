package utils;

import http.HttpRequest;
import http.HttpResponse;
import utils.request.*;
import utils.response.*;

public enum API {
    REGISTER,
    GET_LOTS,
    BET,
    NEW_LOT,
    STOP_LOT;


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
}
