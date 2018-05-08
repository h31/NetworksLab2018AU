package ru.spbau.bachelors2015.roulette.server

class NicknameIsTaken: Exception()

class CroupierIsAlreadyRegistered: Exception()

class GameModel {
    private var isCroupierRegistered = false

    private val nicknames = mutableSetOf<String>()

    fun registerPlayer(nickname: String): Player {
        synchronized(this) {
            if (nicknames.contains(nickname)) {
                throw NicknameIsTaken()
            }

            nicknames.add(nickname)

            return Player(nickname)
        }
    }

    fun registerCroupier(nickname: String): Croupier {
        synchronized(this) {
            if (nicknames.contains(nickname)) {
                throw NicknameIsTaken()
            }

            if (isCroupierRegistered) {
                throw CroupierIsAlreadyRegistered()
            }

            isCroupierRegistered = true
            nicknames.add(nickname)

            return Croupier(nickname)
        }
    }
}
