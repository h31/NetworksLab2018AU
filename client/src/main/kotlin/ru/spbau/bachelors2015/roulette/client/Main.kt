package ru.spbau.bachelors2015.roulette.client

import javafx.application.Application
import javafx.stage.Stage
import ru.spbau.bachelors2015.roulette.client.SceneFactory.getRegistrationScene
import ru.spbau.bachelors2015.roulette.protocol.highlevel.ClientCommunicationSocket
import java.net.Socket

class UI(private val client: ClientCommunicationSocket) : Application() {
    private var primaryStage: Stage? = null

    override fun start(primaryStage: Stage) {
        primaryStage.title = WINDOW_TITLE
        val startScene = getRegistrationScene(client, {
            this.primaryStage!!.scene = it
        }
)
        primaryStage.scene = startScene
        this.primaryStage = primaryStage
        primaryStage.show()
    }

    fun main(args: Array<String>) {
        Application.launch(*args)
    }
}

fun main(args: Array<String>) {
    val server = args[0]
    val port = args[1].toInt()
    val socket = Socket(server, port)
    val client = ClientCommunicationSocket(socket)

    UI(client).main(emptyArray())
}