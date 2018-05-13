package ru.spbau.bachelors2015.roulette.client.handlers

import javafx.application.Platform
import javafx.scene.control.Button
import javafx.scene.control.Label
import ru.spbau.bachelors2015.roulette.client.GameData
import ru.spbau.bachelors2015.roulette.client.PAYOUT_PREFIX
import ru.spbau.bachelors2015.roulette.client.ROULETTE_PREFIX
import ru.spbau.bachelors2015.roulette.protocol.highlevel.ErrorResponse
import ru.spbau.bachelors2015.roulette.protocol.highlevel.GameResultsResponse
import ru.spbau.bachelors2015.roulette.protocol.highlevel.GameResultsResponseHandler

class GameResultsHandler(
        private val rouletteValue: Label,
        private val playerPayout: Label,
        private val makeBetButton: Button) : GameResultsResponseHandler {
    override fun handle(response: GameResultsResponse) {
        Platform.runLater(object : Runnable {
            override fun run() {
                rouletteValue.text = ROULETTE_PREFIX + response.rouletteValue
                val payout = response.payout[GameData.nickname]!!
                playerPayout.text = PAYOUT_PREFIX + payout
                makeBetButton.isDisable = false
            }
        })
    }

    override fun handle(response: ErrorResponse) {
        println(response.messageBody)
        throw Exception("cannot get game results")
    }
}