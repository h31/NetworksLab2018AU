package server.commandrunner;

import server.Context;
import utils.API;
import utils.data.Lot;
import utils.data.User;
import utils.data.UserRole;
import utils.request.NewLotRequestCommand;
import utils.request.RequestCommand;
import utils.response.ForbiddenResponseCommand;
import utils.response.NewLotResponseCommand;
import utils.response.ResponseCommand;

public class NewLotCommandRunner implements CommandRunner {
    private final static NewLotCommandRunner INSTANCE = new NewLotCommandRunner();

    private NewLotCommandRunner() {}

    public static NewLotCommandRunner getInstance() {
        return INSTANCE;
    }

    @Override
    public ResponseCommand run(RequestCommand requestCommand, Context context) {
        User user = context.getUser();
        if (user == null || user.getRole() != UserRole.MANAGER) {
            return ForbiddenResponseCommand.getInstance();
        }
        NewLotRequestCommand newLotRequestCommand = (NewLotRequestCommand) requestCommand;
        String description = newLotRequestCommand.getDescription();
        int startValue = newLotRequestCommand.getStartValue();
        int lotId = context.getLots().size();
        Lot lot = new Lot(description, startValue, lotId);
        context.addLot(lot);
        return new NewLotResponseCommand(NO_ERROR, lotId);
    }

    @Override
    public API getAPI() {
        return API.NEW_LOT;
    }
}
