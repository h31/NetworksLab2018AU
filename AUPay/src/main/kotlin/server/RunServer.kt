package server

import kotlinx.coroutines.experimental.runBlocking
import network.SERVER_PORT
import network.http.HttpRequest
import network.http.HttpResponse
import org.json.JSONObject
import java.net.ServerSocket

private suspend fun ServerDatabase.processRequest(
        request: HttpRequest
): HttpResponse = try {
    val path = request.path
    when {
        path.startsWith("/register") -> processRegister()
        path.startsWith("/accounts") -> processAccounts()
        path.startsWith("/balance") -> request.withAuthToken { processBalance() }
        path.startsWith("/pay/") -> request.withAuthToken {
            val pathParts = path.split(Regex.fromLiteral("/"))
            val amount = pathParts[2].toInt()
            val toId = pathParts[3].toInt()

            return@withAuthToken processPay(amount, toId)
        }
        path.startsWith("/request/") -> request.withAuthToken {
            val pathParts = path.split(Regex.fromLiteral("/"))
            val amount = pathParts[2].toInt()
            val fromId = pathParts[3].toInt()

            return@withAuthToken processRequestPayment(amount, fromId)
        }
        path.startsWith("/payments") -> request.withAuthToken { processPayments() }
        path.startsWith("/cancel") -> request.withAuthToken {
            val pathParts = path.split(Regex.fromLiteral("/"))
            val paymentId = pathParts[2].toInt()

            return@withAuthToken processCancel(paymentId)
        }
        path.startsWith("/confirm") -> request.withAuthToken {
            val pathParts = path.split(Regex.fromLiteral("/"))
            val paymentId = pathParts[2].toInt()

            return@withAuthToken processConfirm(paymentId)
        }
        path.startsWith("/deny") -> request.withAuthToken {
            val pathParts = path.split(Regex.fromLiteral("/"))
            val paymentId = pathParts[2].toInt()

            return@withAuthToken processDeny(paymentId)
        }
        else -> throw IllegalArgumentException()
    }
} catch (_: Exception) {
    HttpResponse(
            HttpResponse.BAD_REQUEST_STATUS_CODE,
            "Bad Request",
            emptyMap(),
            JSONObject())
}

fun main(args: Array<String>) {
    ServerSocket(SERVER_PORT).use { serverSocket ->
        val serverDatabase = ServerDatabase()

        println("SERVER IS UP AND RUNNING")

        runBlocking {
            while (true) {
                try {
                    serverSocket.accept().use { socket ->
                        println("NEW REQUEST")
                        val request = HttpRequest.parseFromInputStream(socket.getInputStream())
                        socket.shutdownInput()
                        println(request)

                        val response = serverDatabase.processRequest(request)
                        response.writeToOutputStream(socket.getOutputStream())
                        println(response)
                        socket.shutdownOutput()
                    }
                } catch (e: Exception) {
                    println("EXCEPTION DURING HANDLING: ${e.message}")
                }
            }
        }
    }
}