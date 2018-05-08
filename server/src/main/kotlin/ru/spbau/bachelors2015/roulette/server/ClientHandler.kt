package ru.spbau.bachelors2015.roulette.server

import ru.spbau.bachelors2015.roulette.protocol.highlevel.*
import java.net.Socket

class ClientHandler(
    private val clientSocket: Socket,
    private val gameModel: GameModel
): Runnable {
    private var shouldStop = false

    private var handler: RequestHandler = RegistrationHandler()

    override fun run() {
        val communicator = ServerCommunicationSocket(clientSocket)

        while (!shouldStop) {
            communicator.handleNextRequest(handler)
        }
    }

    private inner class RegistrationHandler: RequestHandler {
        override fun handle(request: RegistrationRequest): Response {
            try {
                when (request.clientRole) {
                    ClientRole.PLAYER -> gameModel.registerPlayer(request.nickname)

                    ClientRole.CROUPIER -> {
                        handler = CroupierHandler(gameModel.registerCroupier(request.nickname))
                    }
                }
            } catch (_: NicknameIsTaken) {
                return ErrorResponse("Nickname is already taken")
            } catch (_: CroupierIsAlreadyRegistered) {
                return ErrorResponse("There is already a croupier in the game")
            }

            return OkResponse()
        }

        override fun handle(request: GameStartRequest): Response {
            return ErrorResponse(unauthorizedRequestErrorMessage)
        }

        override fun handle(request: GameStatusRequest): Response {
            return ErrorResponse(unauthorizedRequestErrorMessage)
        }

        override fun handle(request: BalanceRequest): Response {
            return ErrorResponse(unauthorizedRequestErrorMessage)
        }

        override fun handle(request: BetRequest): Response {
            return ErrorResponse(unauthorizedRequestErrorMessage)
        }

        override fun handle(request: GameResultsRequest): Response {
            return ErrorResponse(unauthorizedRequestErrorMessage)
        }

        override fun handleSocketClosing() {
            shouldStop = true
        }
    }

    private inner class CroupierHandler(val croupier: GameModel.Croupier): RequestHandler {
        override fun handle(request: RegistrationRequest): Response {
            return ErrorResponse(repeatedRegistrationErrorMessage)
        }

        override fun handle(request: GameStartRequest): Response {
            TODO()
        }

        override fun handle(request: GameStatusRequest): Response {
            TODO()
        }

        override fun handle(request: BalanceRequest): Response {
            return ErrorResponse(unsupportedOperationErrorMessage)
        }

        override fun handle(request: BetRequest): Response {
            return ErrorResponse(unsupportedOperationErrorMessage)
        }

        override fun handle(request: GameResultsRequest): Response {
            TODO()
        }

        override fun handleSocketClosing() {
            croupier.destroy()
            shouldStop = true
        }
    }

    private companion object {
        const val unauthorizedRequestErrorMessage = "Unauthorized request"

        const val repeatedRegistrationErrorMessage = "Repeated registration"

        const val unsupportedOperationErrorMessage = "Unsupported operation"
    }
}