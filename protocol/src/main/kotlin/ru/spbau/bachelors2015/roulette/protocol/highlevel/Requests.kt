package ru.spbau.bachelors2015.roulette.protocol.highlevel

import ru.spbau.bachelors2015.roulette.protocol.http.*

class InvalidHttpRequest : Exception()

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

        fun fromHttpRepresentation(request: HttpRequest): RegistrationRequest {
            if (request.uri.queryLine == null) {
                throw InvalidHttpRequest()
            }

            val clientRoleString = request.uri.queryLine.keyValuePairs[roleQuerlyLineKey]
            val nickname = request.uri.queryLine.keyValuePairs[nicknameQueryLineKey]

            if (clientRoleString == null || nickname == null) {
                throw InvalidHttpRequest()
            }

            val clientRole = try {
                ClientRole.values().first { it.stringRepresentation == clientRoleString }
            } catch (_: NoSuchElementException) {
                throw InvalidHttpRequest()
            }

            return RegistrationRequest(clientRole, nickname)
        }
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

        fun fromHttpRepresentation(request: HttpRequest): GameStartRequest {
            return GameStartRequest()
        }
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

        fun fromHttpRepresentation(request: HttpRequest): GameStatusRequest {
            return GameStatusRequest()
        }
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

        fun fromHttpRepresentation(request: HttpRequest): BalanceRequest {
            return BalanceRequest()
        }
    }
}

class BetRequest(val gameId: Int, val bet: Bet): Request() {
    override val resourcePath = BetRequest.resourcePath

    override val requestMethod = BetRequest.requestMethod

    override fun queryLine(): QueryLine {
        val queryLinePairs = mutableListOf(
            Pair(gameIdQueryLineKey, gameId.toString()),
            Pair(valueQueryLineKey, bet.value.toString())
        )

        bet.accept(object : BetVisitor<Unit> {
            override fun visit(bet: BetOnEvenNumbers) {
                queryLinePairs.add(Pair(typeQueryLineKey, BetTypeEnumeration.EVEN.typeName))
            }

            override fun visit(bet: BetOnOddNumbers) {
                queryLinePairs.add(Pair(typeQueryLineKey, BetTypeEnumeration.ODD.typeName))
            }

            override fun visit(bet: BetOnExactNumber) {
                queryLinePairs.add(Pair(typeQueryLineKey, BetTypeEnumeration.EXACT.typeName))
                queryLinePairs.add(Pair(numberQueryLineKey, bet.number.toString()))
            }

        })

        return QueryLine(*queryLinePairs.toTypedArray())
    }

    companion object {
        val resourcePath: ResourcePath = ResourcePath("bet")

        val requestMethod: HttpRequestMethod = HttpRequestMethod.PUT

        private const val gameIdQueryLineKey = "id"

        private const val valueQueryLineKey = "value"

        private const val typeQueryLineKey = "bet"

        private const val numberQueryLineKey = "number"

        private enum class BetTypeEnumeration(val typeName: String) {
            EVEN("even"), ODD("odd"), EXACT("exact");

            companion object {
                fun fromTypeName(typeName: String): BetTypeEnumeration {
                    for (element in BetTypeEnumeration.values()) {
                        if (element.typeName == typeName) {
                            return element
                        }
                    }

                    throw NoSuchElementException()
                }
            }
        }

        fun fromHttpRepresentation(request: HttpRequest): BetRequest {
            if (request.uri.queryLine == null) {
                throw InvalidHttpRequest()
            }

            val gameIdString = request.uri.queryLine.keyValuePairs[gameIdQueryLineKey]
            val valueString = request.uri.queryLine.keyValuePairs[valueQueryLineKey]
            val typeString = request.uri.queryLine.keyValuePairs[typeQueryLineKey]

            if (gameIdString == null || valueString == null || typeString == null) {
                throw InvalidHttpRequest()
            }

            val gameId = try {
                gameIdString.toInt()
            } catch (_: NumberFormatException) {
                throw InvalidHttpRequest()
            }

            val value = try {
                valueString.toInt()
            } catch (_: NumberFormatException) {
                throw InvalidHttpRequest()
            }

            val bet = try {
                when (BetTypeEnumeration.fromTypeName(typeString)) {
                    BetRequest.Companion.BetTypeEnumeration.EVEN -> BetOnEvenNumbers(value)

                    BetRequest.Companion.BetTypeEnumeration.ODD -> BetOnOddNumbers(value)

                    BetRequest.Companion.BetTypeEnumeration.EXACT -> {
                        val numberString =
                            request.uri.queryLine.keyValuePairs[numberQueryLineKey] ?:
                                throw InvalidHttpRequest()

                        val number = try {
                            numberString.toInt()
                        } catch (_: NumberFormatException) {
                            throw InvalidHttpRequest()
                        }

                        BetOnExactNumber(number, value)
                    }
                }
            } catch (_: NoSuchElementException) {
                throw InvalidHttpRequest()
            }

            return BetRequest(gameId, bet)
        }
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

        fun fromHttpRepresentation(request: HttpRequest): GameResultsRequest {
            if (request.uri.queryLine == null) {
                throw InvalidHttpRequest()
            }

            val gameIdString =
                request.uri.queryLine.keyValuePairs[gameIdQueryLineKey] ?:throw InvalidHttpRequest()

            val gameId = try {
                gameIdString.toInt()
            } catch (_: NumberFormatException) {
                throw InvalidHttpRequest()
            }

            return GameResultsRequest(gameId)
        }
    }
}
