package ru.spbau.bachelors2015.roulette.server

import org.hamcrest.CoreMatchers.*
import org.hamcrest.MatcherAssert.assertThat
import org.junit.Rule
import org.junit.Test
import ru.spbau.bachelors2015.roulette.protocol.highlevel.BetOnEvenNumbers
import ru.spbau.bachelors2015.roulette.protocol.highlevel.BetOnOddNumbers

class ServerTest {
    @Rule
    @JvmField
    val serverRule = ServerRule()

    @Test(timeout = 1000)
    fun registrationRequest() {
        CroupierClient(ServerRule.serverPort, "croupier")
    }

    @Test(timeout = 1000)
    fun gameStartRequest() {
        val croupierClient = CroupierClient(ServerRule.serverPort, "croupier")
        croupierClient.startGame()
    }

    @Test(timeout = ((Game.roundDurationSeconds + 1) * 1000L))
    fun payoutTest() {
        val croupier = CroupierClient(ServerRule.serverPort, "croupier")
        val player1 = PlayerClient(ServerRule.serverPort, "player1")
        val player2 = PlayerClient(ServerRule.serverPort, "player2")

        val gameId = croupier.startGame()

        val betValue = 100
        player1.bet(gameId, BetOnOddNumbers(betValue))
        player2.bet(gameId, BetOnEvenNumbers(betValue))

        Thread.sleep(Game.roundDurationSeconds * 1000L)
        assertThat(croupier.isGameOver(gameId), `is`(equalTo(true)))

        val balance1 = player1.balance()
        val balance2 = player2.balance()

        val expectedBalance1 = CasinoModel.initialBalance - betValue
        val expectedBalance2 = CasinoModel.initialBalance + betValue
        assertThat(
            Pair(balance1, balance2),
            `is`(anyOf(
                equalTo(Pair(expectedBalance1, expectedBalance2)),
                equalTo(Pair(expectedBalance2, expectedBalance1))
            ))
        )
    }
}
