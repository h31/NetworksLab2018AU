package ru.spbau.bachelors2015.roulette.protocol.highlevel

import ru.spbau.bachelors2015.roulette.protocol.http.*

/**
 * Abstract class which represents a request from a client.
 */
abstract class Request {
    /**
     * Returns http representation of this request.
     */
    fun toHttpRepresentation(): HttpRequest {
        return HttpRequest(
            requestMethod,
            null,
            Uri(resourcePath, queryLine()),
            null
        )
    }

    protected abstract val resourcePath: ResourcePath

    protected abstract val requestMethod: HttpRequestMethod

    protected abstract fun queryLine(): QueryLine
}

enum class ClientRole(val stringRepresentation: String) {
    PLAYER("player"), CROUPIER("croupier");
}

class RegistrationRequest(
    val clientRole: ClientRole,
    val nickname: String
): Request() {
    override val resourcePath: ResourcePath = RegistrationRequest.resourcePath

    override val requestMethod: HttpRequestMethod = RegistrationRequest.requestMethod

    override fun queryLine(): QueryLine = QueryLine(
        Pair(roleQuerlyLineKey, clientRole.stringRepresentation),
        Pair(nicknameQueryLineKey, nickname)
    )

    companion object {
        val resourcePath: ResourcePath = ResourcePath()

        val requestMethod: HttpRequestMethod = HttpRequestMethod.CONNECT

        private val roleQuerlyLineKey = "role"

        private val nicknameQueryLineKey = "nickname"
    }
}

class GameStartRequest: Request() {
    override val resourcePath = GameStartRequest.resourcePath

    override val requestMethod = GameStartRequest.requestMethod

    override fun queryLine(): QueryLine {
        return QueryLine()
    }

    companion object {
        val resourcePath: ResourcePath = ResourcePath("start-game")

        val requestMethod: HttpRequestMethod = HttpRequestMethod.PUT
    }
}

class GameStatusRequest: Request() {
    override val resourcePath = GameStatusRequest.resourcePath

    override val requestMethod = GameStatusRequest.requestMethod

    override fun queryLine(): QueryLine {
        return QueryLine()
    }

    companion object {
        val resourcePath: ResourcePath = ResourcePath("game-status")

        val requestMethod: HttpRequestMethod = HttpRequestMethod.GET
    }
}

class BalanceRequest: Request() {
    override val resourcePath = BalanceRequest.resourcePath

    override val requestMethod = BalanceRequest.requestMethod

    override fun queryLine(): QueryLine {
        return QueryLine()
    }

    companion object {
        val resourcePath: ResourcePath = ResourcePath("balance")

        val requestMethod: HttpRequestMethod = HttpRequestMethod.GET
    }
}

class BetRequest(val gameId: Int, val value: Int): Request() {
    override val resourcePath = BetRequest.resourcePath

    override val requestMethod = BetRequest.requestMethod

    override fun queryLine(): QueryLine {
        return QueryLine(
            Pair(gameIdQueryLineKey, gameId.toString()),
            Pair(valueQueryLineKey, value.toString())
        )
    }

    companion object {
        val resourcePath: ResourcePath = ResourcePath("bet")

        val requestMethod: HttpRequestMethod = HttpRequestMethod.PUT

        private val gameIdQueryLineKey = "id"

        private val valueQueryLineKey = "value"
    }
}

class GameResultsRequest(val gameId: Int): Request() {
    override val resourcePath = GameResultsRequest.resourcePath

    override val requestMethod = GameResultsRequest.requestMethod

    override fun queryLine(): QueryLine {
        return QueryLine(Pair(gameIdQueryLineKey, gameId.toString()))
    }

    companion object {
        val resourcePath: ResourcePath = ResourcePath("game-results")

        val requestMethod: HttpRequestMethod = HttpRequestMethod.GET

        private val gameIdQueryLineKey = "id"
    }
}
