package server.commandrunner;

import server.Context;
import utils.API;
import utils.data.Lot;
import utils.data.User;
import utils.request.BetRequestCommand;
import utils.request.RequestCommand;
import utils.response.BetResponseCommand;
import utils.response.ForbiddenResponseCommand;
import utils.response.ResponseCommand;

public class BetCommandRunner implements CommandRunner {
    private final static BetCommandRunner INSTANCE = new BetCommandRunner();

    private BetCommandRunner() {}

    public static BetCommandRunner getInstance() {
        return INSTANCE;
    }

    @Override
    public ResponseCommand run(RequestCommand requestCommand, Context context) {
        User user = context.getUser();
        if (user == null) {
            return new ForbiddenResponseCommand("User not registered");
        }
        BetRequestCommand betRequestCommand = (BetRequestCommand) requestCommand;
        int lotId = betRequestCommand.getLotId();
        int value = betRequestCommand.getValue();
        String executionResult = NO_ERROR;
        if (context.containsLot(lotId)) {
            Lot lot = context.getLot(lotId);
            if (lot.getCurrentPrice() < value) {
                lot.setCurrentPrice(value);
                lot.setCurrentLeader(context.getUser());
            } else {
                executionResult = "Current price is " + lot.getCurrentPrice() + ", your bet is " + value;
            }
        } else {
            executionResult = "No lot with such lotId: " + lotId;
        }
        return new BetResponseCommand(executionResult);
    }

    @Override
    public API getAPI() {
        return API.BET;
    }

}
