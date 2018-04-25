package server.commandrunner;

import server.Context;
import utils.API;
import utils.request.RequestCommand;
import utils.response.NotImplementedResponseCommand;
import utils.response.ResponseCommand;

public class NotImplementedCommandRunner implements CommandRunner {
    private final static NotImplementedCommandRunner INSTANCE = new NotImplementedCommandRunner();

    private NotImplementedCommandRunner() {}

    public static NotImplementedCommandRunner getInstance() {
        return INSTANCE;
    }

    public ResponseCommand run() {
        return run(null, null);
    }
    @Override
    public ResponseCommand run(RequestCommand requestCommand, Context context) {
        return NotImplementedResponseCommand.getInstance();
    }

    @Override
    public API getAPI() {
        return null;
    }
}
