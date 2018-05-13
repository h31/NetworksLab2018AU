package ru.spbau.bachelors2015.roulette.client.handlers

import ru.spbau.bachelors2015.roulette.protocol.highlevel.BetResponseHandler
import ru.spbau.bachelors2015.roulette.protocol.highlevel.ErrorResponse
import ru.spbau.bachelors2015.roulette.protocol.highlevel.OkResponse

class BetHandler: BetResponseHandler {
    override fun handle(response: OkResponse) {

    }

    override fun handle(response: ErrorResponse) {
        println(response.messageBody)
        throw Exception("cannot make bet")
    }
}