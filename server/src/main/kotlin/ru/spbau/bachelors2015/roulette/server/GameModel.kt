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

            return PlayerImplementation(nickname)
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

            return CroupierImplementation(nickname)
        }
    }

    interface Role {
        val nickname: String

        fun destroy()
    }

    interface Player : Role

    interface Croupier : Role

    private inner class PlayerImplementation(override val nickname: String) : Player {
        override fun destroy() {
            synchronized(this@GameModel) {
                nicknames.remove(nickname)
            }
        }
    }

    private inner class CroupierImplementation(override val nickname: String) : Croupier {
        override fun destroy() {
            synchronized(this@GameModel) {
                isCroupierRegistered = false
                nicknames.remove(nickname)
            }
        }
    }
}
