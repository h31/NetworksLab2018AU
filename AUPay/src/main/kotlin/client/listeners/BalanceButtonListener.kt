package client.listeners

import client.ClientWindow
import network.requests.BalanceHttpRequest
import java.awt.event.ActionEvent

class BalanceButtonListener(
    clientWindow: ClientWindow
) : ClientWindowButtonListener(clientWindow) {

    override fun actionPerformed(e: ActionEvent?) {
        val request = BalanceHttpRequest(clientWindow.authToken!!)
        executeRequestInsideClientWindow(request) { response ->
            val balance = response.body.getInt("balance")
            clientWindow.showMessageDialog("Your account balance: $balance")
        }
    }
}
