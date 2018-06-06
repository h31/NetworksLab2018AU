package utils.request;

import http.HttpRequest;
import org.json.JSONException;
import utils.API;

public interface RequestCommand {

    HttpRequest toHttpRequest() throws JSONException;

    API getAPI();
}
