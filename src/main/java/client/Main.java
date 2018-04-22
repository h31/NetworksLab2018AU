package client;


import java.util.Scanner;

public class Main {



    public static void main(String[] args) {

        final Network network = new Network(args[0], Integer.valueOf(args[1]));
        final Logic logic = new Logic(network);
        final CliParser cliParser = new CliParser();

        final Scanner scanner = new Scanner(System.in);
        while (true) {
            System.out.print(">>");
            final String input = scanner.nextLine();

            final CliParser.ParseResult parseResult = cliParser.parse(input);
            if (parseResult.isExit()) {
                break;
            } else {
                System.out.println(logic.process(parseResult.getRequestCommand()));
            }

        }

    }


}
