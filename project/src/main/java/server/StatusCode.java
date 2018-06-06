package server;

import http.HttpResponse;
import org.json.JSONException;
import utils.UnknownStatusCodeException;
import utils.response.ForbiddenResponseCommand;
import utils.response.NotImplementedResponseCommand;
import utils.response.ResponseCommand;

public enum StatusCode {
    OK(200),
    FORBIDDEN(403),
    NOT_IMPLEMENTED(501);
    private final int code;

    StatusCode(int code) {
        this.code = code;
    }

    public static StatusCode buildStatusCode(int code) throws UnknownStatusCodeException {
        for (StatusCode statusCode: StatusCode.values()) {
            if (statusCode.getCode() == code) {
                return statusCode;
            }
        }
        throw new UnknownStatusCodeException(code);
    }

    public ResponseCommand buildResponse(HttpResponse httpResponse) throws JSONException {
        StatusCode statusCode = httpResponse.getStatusCode();
        switch (this) {
            case FORBIDDEN: return new ForbiddenResponseCommand(httpResponse);
            case NOT_IMPLEMENTED: return new NotImplementedResponseCommand(httpResponse);
        }
        throw new IllegalStateException("Can't build response with status code: " + statusCode.getCode());
    }

    public int getCode() {
        return code;
    }
}
