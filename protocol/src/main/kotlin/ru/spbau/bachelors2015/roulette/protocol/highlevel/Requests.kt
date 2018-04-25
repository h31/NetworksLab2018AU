package ru.spbau.bachelors2015.roulette.protocol.highlevel

import ru.spbau.bachelors2015.roulette.protocol.http.*

abstract class Request {
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
