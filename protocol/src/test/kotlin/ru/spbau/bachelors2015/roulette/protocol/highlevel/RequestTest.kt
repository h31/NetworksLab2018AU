package ru.spbau.bachelors2015.roulette.protocol.highlevel

import org.hamcrest.CoreMatchers.`is`
import org.hamcrest.CoreMatchers.equalTo
import org.junit.Assert.*
import org.junit.Test

class RequestTest {
    @Test
    fun registrationRequestTest() {
        val request1 = RegistrationRequest(ClientRole.CROUPIER, "vasya")
        val request2 = RegistrationRequest.fromHttpRepresentation(
            request1.toHttpRepresentation()
        )

        assertThat(request2.clientRole, `is`(equalTo(request1.clientRole)))
        assertThat(request2.nickname, `is`(equalTo(request1.nickname)))
    }

    @Test
    fun gameStartRequestTest() {
        val request1 = GameStartRequest()
        GameStartRequest.fromHttpRepresentation(request1.toHttpRepresentation())
    }

    @Test
    fun gameStatusRequestTest() {
        val request1 = GameStatusRequest()
        GameStatusRequest.fromHttpRepresentation(request1.toHttpRepresentation())
    }

    @Test
    fun balanceRequestTest() {
        val request1 = BalanceRequest()
        BalanceRequest.fromHttpRepresentation(request1.toHttpRepresentation())
    }

    @Test
    fun betRequestTest() {
        val request1 = BetRequest(1, 100)
        val request2 = BetRequest.fromHttpRepresentation(request1.toHttpRepresentation())

        assertThat(request2.gameId, `is`(equalTo(request1.gameId)))
        assertThat(request2.value, `is`(equalTo(request1.value)))
    }

    @Test
    fun gameResultsRequestTest() {
        val request1 = GameResultsRequest(2)
        val request2 = GameResultsRequest.fromHttpRepresentation(request1.toHttpRepresentation())

        assertThat(request2.gameId, `is`(equalTo(request1.gameId)))
    }
}