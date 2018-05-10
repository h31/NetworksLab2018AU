package ru.spbau.bachelors2015.roulette.client

import ru.spbau.bachelors2015.roulette.protocol.highlevel.ErrorResponse
import ru.spbau.bachelors2015.roulette.protocol.highlevel.OkResponse
import ru.spbau.bachelors2015.roulette.protocol.highlevel.RegistrationResponseHandler

class RegistrationHandler(private val callback: () -> Unit) : RegistrationResponseHandler {
    override fun handle(response: ErrorResponse) {

    }

    override fun handle(response: OkResponse) {
        callback()
    }

}