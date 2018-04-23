package server;

import utils.API;
import utils.data.Lot;
import utils.request.BetRequestCommand;
import utils.request.RequestCommand;
import utils.response.BetResponseCommand;
import utils.response.ResponseCommand;

public class BetCommandRunner implements CommandRunner {
    private final static BetCommandRunner INSTANCE = new BetCommandRunner();

    public static BetCommandRunner getInstance() {
        return INSTANCE;
    }

    @Override
    public ResponseCommand run(RequestCommand requestCommand, Context context) {
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
