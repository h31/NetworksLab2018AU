package client.listeners

import client.ClientWindow
import network.http.HttpRequest
import network.http.HttpResponse
import java.awt.event.ActionListener
import kotlin.concurrent.thread

abstract class ClientWindowButtonListener(
    protected val clientWindow: ClientWindow
) : ActionListener {

    protected fun executeRequestInsideClientWindow(
        request: HttpRequest, callbackRoutine: (HttpResponse) -> Unit) {
        thread {
            try {
                val response = clientWindow.httpClient.executeRequest(request)
                if (response.statusCode != network.http.HttpResponse.OK_STATUS_CODE) {
                    clientWindow
                        .showUnsuccessfulRequest(response.statusCode, response.statusMessage)
                    return@thread
                }
                callbackRoutine(response)
            } catch (e: Exception) {
                clientWindow.showMessageDialog("Exception: ${e.message}")
            }
        }
    }
}
