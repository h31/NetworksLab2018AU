package server.commandrunner;

import server.Context;
import utils.API;
import utils.request.RequestCommand;
import utils.response.ResponseCommand;

//TODO implement
public class DisconnectCommandRunner implements CommandRunner {
    @Override
    public ResponseCommand run(RequestCommand requestCommand, Context context) {
        return null;
    }

    @Override
    public API getAPI() {
        return API.DISCONNECT;
    }
}
