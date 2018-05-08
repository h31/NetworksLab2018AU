package ru.spbau.bachelors2015.roulette.server

abstract class Role(val nickname: String)

class Player(nickname: String): Role(nickname)

class Croupier(nickname: String): Role(nickname)
