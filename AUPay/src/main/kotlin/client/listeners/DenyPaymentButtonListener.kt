package client.listeners

import client.ClientWindow
import network.requests.DenyPaymentHttpRequest
import java.awt.event.ActionEvent
import javax.swing.*

class DenyPaymentButtonListener(
    clientWindow: ClientWindow
) : ClientWindowButtonListener(clientWindow) {

    override fun actionPerformed(e: ActionEvent?) {
        try {
            val paymentIdToDenyField = JTextField("")
            val panel = JPanel()
            panel.layout = BoxLayout(panel, BoxLayout.Y_AXIS)
            panel.add(JLabel("Payment ID to deny:"))
            panel.add(paymentIdToDenyField)
            val resultCode =
                JOptionPane.showConfirmDialog(
                    clientWindow,
                    panel,
                    "Please enter the payment ID",
                    JOptionPane.OK_CANCEL_OPTION)
            if (resultCode == JOptionPane.OK_OPTION) {
                val paymentIdToDeny = paymentIdToDenyField.text.toInt()
                val request = DenyPaymentHttpRequest(paymentIdToDeny, clientWindow.authToken!!)
                executeRequestInsideClientWindow(request) {
                    clientWindow.showMessageDialog("Payment was successfully denied.")
                }
            }
        } catch (_: Exception) {
            clientWindow.showMessageDialog("Incorrect input.")
        }
    }
}
