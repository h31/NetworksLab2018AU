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

    @Test(timeout = 1000)
    fun registrationRequest() {
        class RegistrationHandler : RegistrationResponseHandler {
            override fun handle(response: ErrorResponse) {
                fail()
            }

            override fun handle(response: OkResponse) {}
        }

        val clientSocket =
            ClientCommunicationSocket(Socket("localhost", ServerRule.serverPort))

        val request = RegistrationRequest(ClientRole.CROUPIER, "nickname")
        clientSocket.send(request, RegistrationHandler())
    }

    @Test(timeout = 1000)
    fun gameStartRequest() {
        class RegistrationHandler : RegistrationResponseHandler {
            override fun handle(response: ErrorResponse) {
                fail()
            }

            override fun handle(response: OkResponse) {}
        }

        class GameStartHandler : GameStartResponseHandler {
            override fun handle(response: GameStartResponse) {
            }

            override fun handle(response: ErrorResponse) {
                fail()
            }
        }

        val clientSocket =
                ClientCommunicationSocket(Socket("localhost", ServerRule.serverPort))

        clientSocket.send(
            RegistrationRequest(ClientRole.CROUPIER, "nickname"),
            RegistrationHandler()
        )

        clientSocket.send(GameStartRequest(), GameStartHandler())
    }
}