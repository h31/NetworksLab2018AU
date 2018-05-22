package client

import javax.swing.SwingUtilities

fun main(args: Array<String>) {
    val client = ClientWindow()
    SwingUtilities.invokeLater {
        client.isVisible = true
    }
}