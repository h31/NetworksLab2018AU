package server;

import utils.API;
import utils.request.RequestCommand;
import utils.response.NotFoundResponseCommand;
import utils.response.ResponseCommand;

public class NotFoundCommandRunner implements CommandRunner {
    private final static NotFoundCommandRunner INSTANCE = new NotFoundCommandRunner();

    private NotFoundCommandRunner() {}

    public static CommandRunner getInstance() {
        return INSTANCE;
    }

    @Override
    public ResponseCommand run(RequestCommand requestCommand, Context context) {
        return NotFoundResponseCommand.getInstance();
    }

    @Override
    public API getAPI() {
        return API.NOT_FOUND;
    }
}
