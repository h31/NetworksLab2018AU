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
import javafx.scene.control.CheckBox
import javafx.scene.control.ChoiceBox






object SceneFactory {
    private const val width = 800.0
    private const val height = 600.0
    private var nickname: String? = null
    var gameId: Int? = null

    fun getRegistrationScene(client: ClientCommunicationSocket, callback: (scene: Scene) -> Unit): Scene {
        val vbox = VBox()
        val roleChooser = Label("Connect to server:")
        roleChooser.alignment = Pos.CENTER

        val hbox = HBox()
        val croupier = Button("As croupier")

        val nameField = TextField("nickname")
        nameField.alignment = Pos.CENTER
        nameField.prefWidth = 200.0

        croupier.setOnAction {
            val role = ClientRole.CROUPIER
            connect(client, role, nameField.text, {callback(getCroupierGameScene(client))})
        }

        val player = Button("As player")
        player.setOnAction {
            val role = ClientRole.PLAYER
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
        this.nickname = nickname
        client.send(request, RegistrationHandler(callback))
    }

    private fun baseGameScene(client: ClientCommunicationSocket): Scene {
        val listView = ListView<String>()

        Thread(ListUpdate(client, listView, this)).start()
        val vbox = VBox()
        vbox.children.addAll(listView)

        return Scene(vbox, width, height)
    }

    private fun getCroupierGameScene(client: ClientCommunicationSocket): Scene {
        val scene = baseGameScene(client)
        val startButton = Button("Start game!")

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
        val choiceLabel = Label("Choose type of bet:")
        choiceBox.items.add("odd")
        choiceBox.items.add("even")
        choiceBox.items.add("exact number")
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

        val makeBetButton = Button("Bet")
        hbox.children.addAll(textField, makeBetButton)

        val balanceBox = HBox()
        val balanceLabel = Label("Your balance:")
        val balance = Label("")
        balanceBox.children.addAll(balanceLabel, balance)
        Thread(BalanceUpdate(client, balance)).start()

        makeBetButton.setOnAction {
            val bet = textField.text.toInt()
            balance.text = (balance.text.toInt() - bet).toString()

            val requestBet: Bet?
            if (choiceBox.value == "odd") {
                requestBet = BetOnOddNumbers(bet)
            } else {
                if (choiceBox.value == "even") {
                    requestBet = BetOnEvenNumbers(bet)
                } else {
                    requestBet = BetOnExactNumber(exactNumber.text.toInt(), bet)
                }
            }

            val request = BetRequest(this.gameId!!, requestBet)
            client.send(request, BetHandler())
            makeBetButton.isDisable = true
        }


        (scene.root as VBox).children.addAll(choiceLabel, choiceBox, hbox, balanceBox)

        return scene
    }
}