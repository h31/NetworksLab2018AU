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

    private void stop() throws IOException {
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
                try {
                    stop();
                } catch (IOException e1) {
                    e1.printStackTrace();
                }
                System.out.println("CLOSING");
                e.printStackTrace();
                break;
            }
            RequestCommand requestCommand = null;
            try {
                requestCommand = API.buildAPI(
                        httpRequest.getMethod(),
                        httpRequest.getURI().getPath().split("/")[1] //TODO check
                ).buildRequest(httpRequest);
            } catch (NotImplementedException e) {
                ResponseCommand responseCommand = new NotImplementedResponseCommand("Not implemented");
                try {
                    HttpResponse httpResponse = responseCommand.toHttpResponse();
                    network.send(httpResponse);
                } catch (JSONException e1) {
                    e1.printStackTrace();
                }
                System.out.println(responseCommand.getExecutionResult());
                continue;
            } catch (JSONException e) {
                e.printStackTrace();
            }
            String executionResult = null;
            try {
                executionResult = process(requestCommand);
            } catch (JSONException e) {
                e.printStackTrace();
            }
            System.out.println(executionResult);
        }
    }
}
