package server;

import utils.API;
import utils.request.RequestCommand;
import utils.response.ResponseCommand;

public interface CommandRunner {
    String NO_ERROR = "OK";

    ResponseCommand run(RequestCommand requestCommand, Context context);

    API getAPI();
}
