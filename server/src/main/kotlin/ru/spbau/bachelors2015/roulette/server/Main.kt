package ru.spbau.bachelors2015.roulette.server

fun main(args: Array<String>) {
    val port = args[0].toInt()

    val server = Server(port)
    server.start()
    while (true);
}
