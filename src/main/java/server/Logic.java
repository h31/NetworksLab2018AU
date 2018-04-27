package server;

import http.HttpRequest;
import http.HttpResponse;
import org.json.JSONException;
import server.commandrunner.CommandRunner;
import utils.API;
import utils.NotImplementedException;
import utils.request.RequestCommand;
import utils.response.NotImplementedResponseCommand;
import utils.response.ResponseCommand;

import java.io.IOException;
import java.net.Socket;

public class Logic {
    private final Context context;
    private final Network network;
    private volatile boolean running;

    public Logic(Context context, Socket clientSocket) throws IOException {
        running = true;
        this.context = context;
        network = new Network(clientSocket);
        network.start();
    }

    private void stop() {
        running = false;
        context.clear();
        network.terminate();
    }

    private String process(RequestCommand requestCommand) throws JSONException {
        CommandRunner commandRunner = requestCommand.getAPI().getCommandRunner();
        ResponseCommand responseCommand = commandRunner.run(requestCommand, context);
        HttpResponse httpResponse = responseCommand.toHttpResponse();
        network.send(httpResponse);
        return responseCommand.getExecutionResult();
    }

    public void start() {
        while (running) {
            HttpRequest httpRequest;
            try {
                httpRequest = network.receive();
            } catch (IOException | JSONException e) {
                System.out.println(e.getMessage());
                stop();
                break;
            }
            RequestCommand requestCommand;
            try {
                requestCommand = API.buildAPI(
                        httpRequest.getMethod(),
                        httpRequest.getURI().getPath().split("/")[1] //TODO check
                ).buildRequest(httpRequest);
                String executionResult = process(requestCommand);
                System.out.println(executionResult);
            } catch (NotImplementedException e) {
                System.out.println(e.getMessage());
                ResponseCommand responseCommand = new NotImplementedResponseCommand(httpRequest.getStartLine());
                try {
                    network.send(responseCommand.toHttpResponse());
                } catch (JSONException e1) {
                    System.out.println(e1.getMessage());
                }
                System.out.println(responseCommand.getExecutionResult());
                stop();
            } catch (JSONException e) {
                System.out.println(e.getMessage());
                stop();
            }
        }
    }
}
