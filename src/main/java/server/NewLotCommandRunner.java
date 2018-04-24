package server;

import utils.API;
import utils.data.Lot;
import utils.request.NewLotRequestCommand;
import utils.request.RequestCommand;
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
