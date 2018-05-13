package ru.spbau.bachelors2015.roulette.server

import org.junit.Assert.fail
import org.junit.Rule
import org.junit.Test
import ru.spbau.bachelors2015.roulette.protocol.highlevel.*
import java.net.Socket

class ServerTest {
    @Rule
    @JvmField
    val serverRule = ServerRule()

    @Test
    fun registrationRequest() {
        class MyHandler : RegistrationResponseHandler {
            override fun handle(response: ErrorResponse) {
                fail()
            }

            override fun handle(response: OkResponse) {}
        }

        val clientSocket =
            ClientCommunicationSocket(Socket("localhost", ServerRule.serverPort))

        val request = RegistrationRequest(ClientRole.CROUPIER, "nickname")
        clientSocket.send(request, MyHandler())
    }
}