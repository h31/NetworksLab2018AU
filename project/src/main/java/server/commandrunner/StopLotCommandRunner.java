package server.commandrunner;

import server.Context;
import utils.API;
import utils.data.User;
import utils.data.UserRole;
import utils.request.RequestCommand;
import utils.request.StopLotRequestCommand;
import utils.response.ForbiddenResponseCommand;
import utils.response.ResponseCommand;
import utils.response.StopLotResponseCommand;

public class StopLotCommandRunner implements CommandRunner {
    private final static StopLotCommandRunner INSTANCE = new StopLotCommandRunner();

    public static StopLotCommandRunner getInstance() {
        return INSTANCE;
    }

    @Override
    public ResponseCommand run(RequestCommand requestCommand, Context context) {
        User user = context.getUser();
        if (user == null) {
            return new ForbiddenResponseCommand("User not registered");
        }
        if (user.getRole() != UserRole.MANAGER) {
            return new ForbiddenResponseCommand("User " + user.getName() + " has role " + user.getRole() + " (MANAGER role required)");
        }
        StopLotRequestCommand stopLotRequestCommand = (StopLotRequestCommand) requestCommand;
        int lotId = stopLotRequestCommand.getLotId();
        String executionResult = NO_ERROR;
        if (context.containsLot(lotId)) {
            context.stopLot(lotId);
        } else {
            executionResult = "No lot with such lotId: " + lotId;
        }
        return new StopLotResponseCommand(executionResult);

    }

    @Override
    public API getAPI() {
        return API.STOP_LOT;
    }
}
