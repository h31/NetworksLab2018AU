package ru.spbau.bachelors2015.roulette.protocol.highlevel

import org.hamcrest.CoreMatchers.`is`
import org.hamcrest.CoreMatchers.equalTo
import org.junit.Assert.*
import org.junit.Test

class ResponseTest {
    @Test
    fun errorResponseTest() {
        val body = "Hello, World"

        val response1 = ErrorResponse(body)
        val response2 = ErrorResponse.fromHttpRepresentation(response1.toHttpRepresentation())

        assertThat(response2.messageBody, `is`(equalTo(response1.messageBody)))
    }

    @Test
    fun okResponseTest() {
        val response1 = OkResponse()
        OkResponse.fromHttpRepresentation(response1.toHttpRepresentation())
    }

    @Test
    fun gameStartResponseTest() {
        val gameId = 42

        val response1 = GameStartResponse(gameId)
        val response2 = GameStartResponse.fromHttpRepresentation(response1.toHttpRepresentation())

        assertThat(response2.gameId, `is`(equalTo(response1.gameId)))
    }

    @Test
    fun gameStatusPositiveResponseTest() {
        val gameId = 42
        val secondsLeft = 9
        val bets = mapOf(Pair("a", 1), Pair("b", 2), Pair("c", 3))

        val response1 = GameStatusPositiveResponse(gameId, secondsLeft, bets)
        val response2 = GameStatusPositiveResponse.fromHttpRepresentation(
                response1.toHttpRepresentation()
        )

        assertThat(response2.gameId, `is`(equalTo(response1.gameId)))
        assertThat(response2.secondsLeft, `is`(equalTo(response1.secondsLeft)))
        assertThat(response2.bets, `is`(equalTo(response1.bets)))
    }

    @Test
    fun gameStatusNegativeResponseTest() {
        val response1 = GameStatusNegativeResponse()
        GameStatusNegativeResponse.fromHttpRepresentation(response1.toHttpRepresentation())
    }

    @Test
    fun balanceResponseTest() {
        val balance = 42

        val response1 = BalanceResponse(balance)
        val response2 = BalanceResponse.fromHttpRepresentation(response1.toHttpRepresentation())

        assertThat(response2.balance, `is`(equalTo(response1.balance)))
    }

    @Test
    fun gameResultsResponseTest() {
        val rouletteValue = 42
        val balanceChanges = mapOf(Pair("a", 1), Pair("b", -1), Pair("c", 0))

        val response1 = GameResultsResponse(rouletteValue, balanceChanges)
        val response2 = GameResultsResponse.fromHttpRepresentation(response1.toHttpRepresentation())

        assertThat(response2.rouletteValue, `is`(equalTo(response1.rouletteValue)))
        assertThat(response2.balanceChanges, `is`(equalTo(response1.balanceChanges)))
    }
}