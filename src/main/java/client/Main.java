package client;


import org.json.JSONException;
import utils.UnknownStatusCodeException;

import java.io.IOException;
import java.util.Scanner;

public class Main {



    public static void main(String[] args) throws JSONException, UnknownStatusCodeException {

        if (args.length < 2) {
            System.out.println("usage: ./client host port");
            return;
        }

        final Network network = new Network(args[0], Integer.valueOf(args[1]));
        try {
            network.start();
        } catch (IOException e) {
            e.printStackTrace();
            //TODO
        }
        final Logic logic = new Logic(network);
        final CliParser cliParser = new CliParser();

        final Scanner scanner = new Scanner(System.in);
        while (true) {
            System.out.print(">>");
            final String input = scanner.nextLine();

            final CliParser.ParseResult parseResult = cliParser.parse(input);
            if (parseResult.isExit()) {
                try {
                    network.terminate();
                } catch (IOException e) {
                    e.printStackTrace();
                    //TODO
                }
                break;
            } else {
                System.out.println(logic.process(parseResult.getRequestCommand()));
            }

        }

    }


}
