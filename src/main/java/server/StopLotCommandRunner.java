package server;

import utils.API;
import utils.request.RequestCommand;
import utils.request.StopLotRequestCommand;
import utils.response.ResponseCommand;
import utils.response.StopLotResponseCommand;

public class StopLotCommandRunner implements CommandRunner {
    @Override
    public ResponseCommand run(RequestCommand requestCommand, Context context) {
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
