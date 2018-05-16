package ru.spbau.bachelors2015.roulette.server

import java.net.ServerSocket
import java.net.Socket

class Server(private val port: Int) {
    private val serverSocket = ServerSocket(port)

    private var shouldRun = true

    private val serverThread = Thread(Runnable {
        val casinoModel = CasinoModel()

        serverSocket.use { serverSocket ->
            while (shouldRun) {
                val socket = serverSocket.accept()

                val clientThread = Thread(ClientHandler(socket, casinoModel))
                clientThread.start()
            }
        }
    })

    fun start() {
        serverThread.start()
    }

    fun stop() {
        shouldRun = false

        val poisonSocket = Socket("localhost", port)
        poisonSocket.close()

        serverThread.join()
    }
}
