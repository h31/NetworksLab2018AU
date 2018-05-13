package ru.spbau.bachelors2015.roulette.client.handlers

import javafx.application.Platform
import ru.spbau.bachelors2015.roulette.client.GameData
import ru.spbau.bachelors2015.roulette.client.GameData.items
import ru.spbau.bachelors2015.roulette.protocol.highlevel.ErrorResponse
import ru.spbau.bachelors2015.roulette.protocol.highlevel.GameStatusNegativeResponse
import ru.spbau.bachelors2015.roulette.protocol.highlevel.GameStatusPositiveResponse
import ru.spbau.bachelors2015.roulette.protocol.highlevel.GameStatusResponseHandler

class GameStatusHandler : GameStatusResponseHandler {
    override fun handle(response: GameStatusPositiveResponse) {
        Platform.runLater(object : Runnable {
            override fun run() {
               updateGameStatus(response)
            }
        })
    }

    private fun updateGameStatus(response: GameStatusPositiveResponse) {
        GameData.gameId = response.gameId
        val seconds = "Seconds left : " + response.secondsLeft

        items.clear()
        items.add(seconds)
        for (entry in response.bets) {
            val label = entry.key + " with bet: " + entry.value
            items.add(label)
            println(label)
        }

        println(seconds)
    }

    override fun handle(response: GameStatusNegativeResponse) {
    }

    override fun handle(response: ErrorResponse) {
        println(response.messageBody)
        throw Exception("cannot get game status")
    }
}