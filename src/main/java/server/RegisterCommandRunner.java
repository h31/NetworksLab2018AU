package server;

import utils.API;
import utils.data.User;
import utils.request.RegisterRequestCommand;
import utils.request.RequestCommand;
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
