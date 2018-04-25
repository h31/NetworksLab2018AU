package server.commandrunner;

import server.Context;
import utils.API;
import utils.data.User;
import utils.request.RegisterRequestCommand;
import utils.request.RequestCommand;
import utils.response.ForbiddenResponseCommand;
import utils.response.RegisterResponseCommand;
import utils.response.ResponseCommand;

public class RegisterCommandRunner implements CommandRunner {
    private final static RegisterCommandRunner INSTANCE = new RegisterCommandRunner();

    public static RegisterCommandRunner getInstance() {
        return INSTANCE;
    }

    @Override
    public ResponseCommand run(RequestCommand requestCommand, Context context) {
        RegisterRequestCommand registerRequestCommand = (RegisterRequestCommand) requestCommand;
        User user = registerRequestCommand.getUser();
        String executionResult = NO_ERROR;
        if (user != null) {
            return new ForbiddenResponseCommand("User at this connection already registered");
        }
        if (context.containsUser(user)) {
            executionResult = "User already registered: " + user.getName() + " " + user.getRole();
        } else {
            context.addUser(user);
            context.setUser(user);
        }
        return new RegisterResponseCommand(executionResult);
    }
    @Override
    public API getAPI() {
        return API.REGISTER;
    }
}
