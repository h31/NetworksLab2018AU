package client

import network.http.HttpRequest
import network.http.HttpResponse
import java.net.Socket

class HttpClient(private val serverHostname: String, private val serverPort: Int) {
    fun executeRequest(request: HttpRequest): HttpResponse {
        Socket(serverHostname, serverPort).use { socket ->
            request.writeToOutputStream(socket.getOutputStream())
            socket.shutdownOutput()
            val response = HttpResponse.parseFromInputStream(socket.getInputStream())
            socket.shutdownInput()
            return response
        }
    }
}
