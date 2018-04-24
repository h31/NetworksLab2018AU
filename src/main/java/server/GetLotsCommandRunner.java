package server;

import utils.API;
import utils.request.RequestCommand;
import utils.response.GetLotsResponseCommand;
import utils.response.ResponseCommand;

import java.util.ArrayList;

public class GetLotsCommandRunner implements CommandRunner {
    private final static GetLotsCommandRunner INSTANCE = new GetLotsCommandRunner();

    private GetLotsCommandRunner() {}

    public static GetLotsCommandRunner getInstance() {
        return INSTANCE;
    }

    @Override
    public ResponseCommand run(RequestCommand requestCommand, Context context) {
        return new GetLotsResponseCommand(NO_ERROR, new ArrayList<>(context.getLots().values()));
    }

    @Override
    public API getAPI() {
        return API.GET_LOTS;
    }
}
