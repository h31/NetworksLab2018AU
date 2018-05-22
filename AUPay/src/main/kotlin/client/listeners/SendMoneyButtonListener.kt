package client.listeners

import client.ClientWindow
import network.requests.PayHttpRequest
import java.awt.Dimension
import java.awt.event.ActionEvent
import javax.swing.*

class SendMoneyButtonListener(
    clientWindow: ClientWindow
) : ClientWindowButtonListener(clientWindow) {

    override fun actionPerformed(e: ActionEvent?) {
        try {
            val amountToSendField = JTextField("")
            val toAccountIdField = JTextField("")
            val panel = JPanel()
            panel.minimumSize = Dimension(300, 80)
            panel.preferredSize = panel.minimumSize
            panel.layout = BoxLayout(panel, BoxLayout.Y_AXIS)
            panel.add(JLabel("Amount to send:"))
            panel.add(amountToSendField)
            panel.add(JLabel("To account ID:"))
            panel.add(toAccountIdField)
            val resultCode =
                JOptionPane.showConfirmDialog(
                    clientWindow,
                    panel,
                    "Please enter the amount and the receiver account",
                    JOptionPane.OK_CANCEL_OPTION)
            if (resultCode == JOptionPane.OK_OPTION) {
                val amountToSend = amountToSendField.text.toInt()
                val toAccountId = toAccountIdField.text.toInt()
                val request =
                    PayHttpRequest(
                        amountToSend,
                        toAccountId,
                        clientWindow.authToken!!)
                executeRequestInsideClientWindow(request) { response ->
                    val paymentId = response.body.getInt("paymentId")
                    clientWindow.showMessageDialog(
                        "Payment order was placed. Its ID is $paymentId.")
                }
            }
        } catch (_: Exception) {
            clientWindow.showMessageDialog("Incorrect input")
        }
    }
}
