package ru.spbau.bachelors2015.roulette.server

import org.junit.Assert.fail
import ru.spbau.bachelors2015.roulette.protocol.highlevel.*
import java.net.Socket

abstract class Client(port: Int, role: ClientRole, nickname: String) {
    protected val socket =
            ClientCommunicationSocket(Socket("localhost", port))

    init {
        socket.send(
            RegistrationRequest(role, nickname),
            object : RegistrationResponseHandler {
                override fun handle(response: OkResponse) { }

                override fun handle(response: ErrorResponse) {
                    fail()
                }
            }
        )
    }

    fun isGameOver(gameId: Int): Boolean {
        var result = false

        socket.send(GameResultsRequest(gameId), object : GameResultsResponseHandler {
            override fun handle(response: GameResultsResponse) {
                result = true
            }

            override fun handle(response: ErrorResponse) {
                result = false
            }

        })

        return result
    }
}

class CroupierClient(
    port: Int,
    nickname: String
) : Client(port, ClientRole.CROUPIER, nickname) {
    fun startGame(): Int {
        var gameId = 0
        socket.send(GameStartRequest(), object : GameStartResponseHandler {
            override fun handle(response: GameStartResponse) {
                gameId = response.gameId
            }

            override fun handle(response: ErrorResponse) {
                fail()
            }
        })

        return gameId
    }
}

class PlayerClient(
    port: Int,
    nickname: String
) : Client(port, ClientRole.PLAYER, nickname) {
    fun bet(gameId: Int, bet: Bet) {
        socket.send(BetRequest(gameId, bet), object : BetResponseHandler {
            override fun handle(response: OkResponse) { }

            override fun handle(response: ErrorResponse) {
                fail()
            }
        })
    }

    fun balance(): Int {
        var result = 0

        socket.send(BalanceRequest(), object : BalanceResponseHandler {
            override fun handle(response: BalanceResponse) {
                result = response.balance
            }

            override fun handle(response: ErrorResponse) {
                fail()
            }
        })

        return result
    }
}
