package ru.spbau.bachelors2015.roulette.client.handlers

import ru.spbau.bachelors2015.roulette.protocol.highlevel.ErrorResponse
import ru.spbau.bachelors2015.roulette.protocol.highlevel.OkResponse
import ru.spbau.bachelors2015.roulette.protocol.highlevel.RegistrationResponseHandler

class RegistrationHandler(private val callback: () -> Unit) : RegistrationResponseHandler {
    override fun handle(response: ErrorResponse) {
        println(response.messageBody)
        throw Exception("Registration failed")
    }

    override fun handle(response: OkResponse) {
        callback()
    }
}