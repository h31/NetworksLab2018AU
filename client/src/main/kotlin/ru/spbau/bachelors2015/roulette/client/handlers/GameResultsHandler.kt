package ru.spbau.bachelors2015.roulette.client.handlers

import javafx.scene.control.Label
import ru.spbau.bachelors2015.roulette.protocol.highlevel.ErrorResponse
import ru.spbau.bachelors2015.roulette.protocol.highlevel.GameResultsResponse
import ru.spbau.bachelors2015.roulette.protocol.highlevel.GameResultsResponseHandler

class GameResultsHandler(private val score: Label, rouletteValue: Label, playerPayout: Label) : GameResultsResponseHandler {
    override fun handle(response: GameResultsResponse) {

    }

    override fun handle(response: ErrorResponse) {

    }

}