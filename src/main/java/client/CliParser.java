package client;

import com.beust.jcommander.JCommander;
import com.beust.jcommander.Parameter;
import com.beust.jcommander.Parameters;
import utils.data.User;
import utils.data.UserRole;
import utils.request.*;

import java.util.ArrayList;
import java.util.List;

public class CliParser {

    private final List<ParserCommand> commands = new ArrayList<>();

    public CliParser() {
        buildCommands();
    }

    private void buildCommands() {
        commands.add(new CommandRegister());
        commands.add(new CommandGetLots());
        commands.add(new CommandBet());
        commands.add(new CommandNewLot());
        commands.add(new CommandStopLot());
        commands.add(new CommandTerminate());
    }

    public ParseResult parse(String input) {
        JCommander.Builder builder = JCommander.newBuilder();
        for (ParserCommand command : commands) {
            builder.addCommand(command.name(), command);
        }
        final JCommander jCommander = builder.build();
        jCommander.parse(input.split(" "));
        if (jCommander.getParsedCommand().equals("exit")) {
            return new ParseResult(null, true);
        } else {
            for (ParserCommand command : commands) {
                if (command.name().equals(jCommander.getParsedCommand())) {
                    return new ParseResult(command.buildRequest(), false);
                }
            }
        }
        throw new IllegalStateException("bad command, man");
    }

    interface ParserCommand {
        String name();
        RequestCommand buildRequest();
    }

    @Parameters(commandDescription = "register into system")
    public class CommandRegister implements ParserCommand {

        @Parameter(names = "-name", description = "user name")
        private String name;

        @Parameter(names = "-role", description = "user role")
        private UserRole userRole;

        @Override
        public String name() {
            return "register";
        }

        @Override
        public RequestCommand buildRequest() {
            return new RegisterRequestCommand(new User(name, userRole));
        }
    }

    @Parameters(commandDescription = "get current lots")
    public class CommandGetLots implements ParserCommand {

        @Override
        public String name() {
            return "lots";
        }

        @Override
        public RequestCommand buildRequest() {
            return new GetLotsRequestCommand();
        }
    }

    @Parameters(commandDescription = "make bet")
    public class CommandBet implements ParserCommand {

        @Parameter(names = "-id", description = "lot id")
        private int lotId;

        @Parameter(names = "-value", description = "lot value")
        private int value;

        @Override
        public String name() {
            return "bet";
        }

        @Override
        public RequestCommand buildRequest() {
            return new BetRequestCommand(lotId, value);
        }
    }

    @Parameters(commandDescription = "create new lot")
    public class CommandNewLot implements ParserCommand {

        @Parameter(names = "-desc", description = "lot description")
        private String desc;

        @Parameter(names = "-start", description = "start value for lot")
        private int startValue;

        @Override
        public String name() {
            return "create";
        }

        @Override
        public RequestCommand buildRequest() {
            return new NewLotRequestCommand(desc, startValue);
        }
    }

    @Parameters(commandDescription = "stop lot")
    public class CommandStopLot implements ParserCommand {

        @Parameter(names = "-id", description = "lot id")
        private int lotId;


        @Override
        public String name() {
            return "stop";
        }

        @Override
        public RequestCommand buildRequest() {
            return new StopLotRequestCommand(lotId);
        }
    }

    @Parameters(commandDescription = "stop terminal")
    public class CommandTerminate implements ParserCommand {

        @Override
        public String name() {
            return "exit";
        }

        @Override
        public RequestCommand buildRequest() {
            return null;
        }
    }

    public class ParseResult {

        private final RequestCommand requestCommand;
        private final boolean exit;

        public RequestCommand getRequestCommand() {
            return requestCommand;
        }

        public boolean isExit() {
            return exit;
        }

        public ParseResult(RequestCommand requestCommand, boolean exit) {
            this.requestCommand = requestCommand;
            this.exit = exit;
        }

    }

}
