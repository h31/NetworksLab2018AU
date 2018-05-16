package ru.spbau.bachelors2015.roulette.client.handlers

import ru.spbau.bachelors2015.roulette.client.GameData
import ru.spbau.bachelors2015.roulette.protocol.highlevel.ErrorResponse
import ru.spbau.bachelors2015.roulette.protocol.highlevel.GameStartResponse
import ru.spbau.bachelors2015.roulette.protocol.highlevel.GameStartResponseHandler

class GameStartHandler : GameStartResponseHandler {
    override fun handle(response: GameStartResponse) {
        GameData.gameId = response.gameId
        println("Game started")
    }

    override fun handle(response: ErrorResponse) {
        println(response.messageBody)
        throw Exception("Cannot start the game")
    }
}