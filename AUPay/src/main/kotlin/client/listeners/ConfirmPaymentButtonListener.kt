package client.listeners

import client.ClientWindow
import network.requests.ConfirmPaymentHttpRequest
import java.awt.event.ActionEvent
import javax.swing.*

class ConfirmPaymentButtonListener(
    clientWindow: ClientWindow
) : ClientWindowButtonListener(clientWindow) {

    override fun actionPerformed(e: ActionEvent?) {
        try {
            val paymentIdToConfirmField = JTextField("")
            val panel = JPanel()
            panel.layout = BoxLayout(panel, BoxLayout.Y_AXIS)
            panel.add(JLabel("Payment ID to confirm:"))
            panel.add(paymentIdToConfirmField)
            val resultCode =
                JOptionPane.showConfirmDialog(
                    clientWindow,
                    panel,
                    "Please enter the payment ID",
                    JOptionPane.OK_CANCEL_OPTION)
            if (resultCode == JOptionPane.OK_OPTION) {
                val paymentIdToConfirm = paymentIdToConfirmField.text.toInt()
                val request =
                    ConfirmPaymentHttpRequest(paymentIdToConfirm, clientWindow.authToken!!)
                executeRequestInsideClientWindow(request) {
                    clientWindow.showMessageDialog("Payment was successfully confirmed.")
                }
            }
        } catch (_: Exception) {
            clientWindow.showMessageDialog("Incorrect input.")
        }
    }
}
