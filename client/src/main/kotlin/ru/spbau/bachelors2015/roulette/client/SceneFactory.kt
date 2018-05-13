package ru.spbau.bachelors2015.roulette.client

import javafx.geometry.Pos
import javafx.scene.Group
import javafx.scene.Scene
import javafx.scene.control.Button
import javafx.scene.control.Label
import javafx.scene.control.ListView
import javafx.scene.control.TextField
import javafx.scene.layout.HBox
import javafx.scene.layout.VBox
import ru.spbau.bachelors2015.roulette.protocol.highlevel.*
import javafx.scene.control.ChoiceBox
import ru.spbau.bachelors2015.roulette.client.handlers.BetHandler
import ru.spbau.bachelors2015.roulette.client.handlers.GameStartHandler
import ru.spbau.bachelors2015.roulette.client.handlers.RegistrationHandler
import ru.spbau.bachelors2015.roulette.client.updaters.BalanceUpdate
import ru.spbau.bachelors2015.roulette.client.updaters.GameResultsUpdate
import ru.spbau.bachelors2015.roulette.client.updaters.ListUpdate


object SceneFactory {
    private const val width = 800.0
    private const val height = 600.0

    fun getRegistrationScene(client: ClientCommunicationSocket, callback: (scene: Scene) -> Unit): Scene {
        val vbox = VBox()
        val roleChooser = Label(CONNECT_BUTTON_LABEL)
        roleChooser.alignment = Pos.CENTER

        val hbox = HBox()
        val croupier = Button(CROUPIER_CONNECT)

        val nameField = TextField(NICKNAME_DEFAULT_VALUE)
        nameField.alignment = Pos.CENTER
        nameField.prefWidth = 200.0

        croupier.setOnAction {
            val role = ClientRole.CROUPIER
            GameData.role = role
            connect(client, role, nameField.text, {callback(getCroupierGameScene(client))})
        }

        val player = Button(PLAYER_CONNECT)
        player.setOnAction {
            val role = ClientRole.PLAYER
            GameData.role = role
            connect(client, role, nameField.text, {callback(getPlayerGameScene(client))})
        }

        hbox.children.addAll(croupier, player)
        hbox.alignment = Pos.CENTER

        vbox.children.addAll(roleChooser, hbox, Group(nameField))
        vbox.alignment = Pos.CENTER
        return Scene(vbox, width, height)
    }

    private fun connect(client: ClientCommunicationSocket, role: ClientRole, nickname: String, callback: () -> Unit) {
        val request = RegistrationRequest(role, nickname)
        GameData.nickname = nickname
        client.send(request, RegistrationHandler(callback))
    }

    private fun baseGameScene(client: ClientCommunicationSocket): Scene {
        val listView = ListView<String>()
        listView.items = GameData.items
        Thread(ListUpdate(client)).start()

        val vbox = VBox()
        vbox.children.addAll(listView)

        return Scene(vbox, width, height)
    }

    private fun getCroupierGameScene(client: ClientCommunicationSocket): Scene {
        val scene = baseGameScene(client)
        val startButton = Button(START_BUTTON_LABEL)

        startButton.setOnAction {
            val request = GameStartRequest()
            client.send(request, GameStartHandler())
        }
        (scene.root as VBox).children.addAll(startButton)

        return scene
    }

    private fun getPlayerGameScene(client: ClientCommunicationSocket): Scene {
        val scene = baseGameScene(client)
        val hbox = HBox()

        val choiceBox = ChoiceBox<String>()
        val choiceLabel = Label(TYPE_OF_BET_LABEL)
        choiceBox.items.add(ODD_NUMBERS)
        choiceBox.items.add(EVEN_NUMBERS)
        //choiceBox.items.add(EXACT_NUMBER)
        choiceBox.selectionModel.selectFirst()

        val exactNumber = TextField()
        exactNumber.textProperty().addListener { _, _, newValue ->
            if (!newValue.matches("\\d*".toRegex())) {
                exactNumber.text = newValue.replace("[^\\d]".toRegex(), "")
            }
        }

        val textField = TextField()
        textField.textProperty().addListener { _, _, newValue ->
            if (!newValue.matches("\\d*".toRegex())) {
                textField.text = newValue.replace("[^\\d]".toRegex(), "")
            }
        }

        val makeBetButton = Button(BET_BUTTON_LABEL)
        hbox.children.addAll(textField, makeBetButton)

        val balance = Label(BALANCE_PREFIX)
        Thread(BalanceUpdate(client, balance)).start()

        val rouletteValue = Label(ROULETTE_PREFIX)
        val playerPayout = Label(PAYOUT_PREFIX)

        makeBetButton.setOnAction {
            val bet = textField.text.toInt()

            val requestBet: Bet?
            if (choiceBox.value == ODD_NUMBERS) {
                requestBet = BetOnOddNumbers(bet)
            } else {
                if (choiceBox.value == EVEN_NUMBERS) {
                    requestBet = BetOnEvenNumbers(bet)
                } else {
                    requestBet = BetOnExactNumber(exactNumber.text.toInt(), bet)
                }
            }

            val request = BetRequest(GameData.gameId!!, requestBet)
            client.send(request, BetHandler())
            makeBetButton.isDisable = true

            Thread(GameResultsUpdate(client, rouletteValue, playerPayout, makeBetButton)).start()
        }

        (scene.root as VBox).children.addAll(choiceLabel, choiceBox, hbox, balance, rouletteValue, playerPayout)

        return scene
    }
}