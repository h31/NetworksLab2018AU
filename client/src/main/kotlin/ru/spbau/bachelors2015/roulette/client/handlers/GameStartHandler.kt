package ru.spbau.bachelors2015.roulette.client.handlers

import ru.spbau.bachelors2015.roulette.protocol.highlevel.ErrorResponse
import ru.spbau.bachelors2015.roulette.protocol.highlevel.GameStartResponse
import ru.spbau.bachelors2015.roulette.protocol.highlevel.GameStartResponseHandler

class GameStartHandler : GameStartResponseHandler {
    override fun handle(response: GameStartResponse) {
    }

    override fun handle(response: ErrorResponse) {
    }
}