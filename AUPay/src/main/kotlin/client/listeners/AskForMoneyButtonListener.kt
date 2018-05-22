package client.listeners

import client.ClientWindow
import network.requests.PaymentRequestHttpRequest
import java.awt.Dimension
import java.awt.event.ActionEvent
import javax.swing.*

class AskForMoneyButtonListener(
    clientWindow: ClientWindow
) : ClientWindowButtonListener(clientWindow) {

    override fun actionPerformed(e: ActionEvent?) {
        try {
            val amountToAskForField = JTextField("")
            val fromAccountIdField = JTextField("")
            val panel = JPanel()
            panel.minimumSize = Dimension(300, 80)
            panel.preferredSize = panel.minimumSize
            panel.layout = BoxLayout(panel, BoxLayout.Y_AXIS)
            panel.add(JLabel("Amount to ask for:"))
            panel.add(amountToAskForField)
            panel.add(JLabel("From account ID:"))
            panel.add(fromAccountIdField)
            val resultCode =
                JOptionPane.showConfirmDialog(
                    clientWindow,
                    panel,
                    "Please enter the amount and the receiver account",
                    JOptionPane.OK_CANCEL_OPTION)
            if (resultCode == JOptionPane.OK_OPTION) {
                val amountToAskFor = amountToAskForField.text.toInt()
                val fromAccountId = fromAccountIdField.text.toInt()
                val request =
                    PaymentRequestHttpRequest(
                        amountToAskFor,
                        fromAccountId,
                        clientWindow.authToken!!)
                executeRequestInsideClientWindow(request) { response ->
                    val paymentId = response.body.getInt("paymentId")
                    clientWindow.showMessageDialog(
                        "Payment order was placed. Its ID is $paymentId. " +
                        "Now the sender should confirm it.")
                }
            }
        } catch (_: Exception) {
            clientWindow.showMessageDialog("Incorrect input")
        }
    }
}
