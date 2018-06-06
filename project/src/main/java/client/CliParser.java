package client;

import com.beust.jcommander.JCommander;
import com.beust.jcommander.MissingCommandException;
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
        commands.add(new CommandHelp());
    }

    public ParseResult parse(String input) {
        JCommander.Builder builder = JCommander.newBuilder();
        for (ParserCommand command : commands) {
            builder.addCommand(command.name(), command);
        }
        final JCommander jCommander = builder.build();
        try {
            jCommander.parse(input.split(" "));
        } catch (Exception e) {
            System.out.println("bad command");
            return new ParseResult(null, true, false);

        }
        if (jCommander.getParsedCommand().equals("exit")) {
            return new ParseResult(null, false, true);
        }
        if (jCommander.getParsedCommand().equals("help")) {
            jCommander.usage();
            return new ParseResult(null, true, false);
        } else {
            for (ParserCommand command : commands) {
                if (command.name().equals(jCommander.getParsedCommand())) {
                    return new ParseResult(command.buildRequest(), false, false);
                }
            }
        }
        throw new IllegalStateException("bad command, man"); //TODO WTF
    }

    interface ParserCommand {
        String name();
        RequestCommand buildRequest();
    }

    @Parameters(commandDescription = "register into system")
    public class CommandRegister implements ParserCommand {

        @Parameter(names = "-name", description = "user name", required = true)
        private String name;

        @Parameter(names = "-role", description = "user role", required = true)
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

        @Parameter(names = "-id", description = "lot id", required = true)
        private int lotId;

        @Parameter(names = "-value", description = "lot value", required = true)
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

        @Parameter(names = "-desc", description = "lot description", required = true)
        private String desc;

        @Parameter(names = "-start", description = "start value for lot", required = true)
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

        @Parameter(names = "-id", description = "lot id", required = true)
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

    @Parameters(commandDescription = "help")
    public class CommandHelp implements ParserCommand {


        @Override
        public String name() {
            return "help";
        }

        @Override
        public RequestCommand buildRequest() {
            return null;
        }
    }

    public class ParseResult {

        private final RequestCommand requestCommand;
        private final boolean skip;
        private final boolean exit;

        public RequestCommand getRequestCommand() {
            return requestCommand;
        }

        public boolean isExit() {
            return exit;
        }

        public boolean isSkip() {
            return skip;
        }

        public ParseResult(RequestCommand requestCommand, boolean skip, boolean exit) {
            this.requestCommand = requestCommand;
            this.skip = skip;
            this.exit = exit;
        }
    }

}
