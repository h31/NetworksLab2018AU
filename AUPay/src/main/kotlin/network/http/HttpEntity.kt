package network.http

import com.google.common.base.Preconditions
import com.google.common.primitives.Bytes
import network.HTTP_ENCODING
import network.HTTP_SEPARATOR
import java.io.OutputStream

abstract class HttpEntity(val headers: Map<String, String>) {

    abstract fun writeToOutputStream(outputStream: OutputStream)

    protected fun writeHeadersToOutputStream(outputStream: OutputStream) {
        headers.forEach { header ->
            outputStream.write("${header.key}: ${header.value}".toByteArray(HTTP_ENCODING))
            outputStream.write(HTTP_SEPARATOR)
        }
        outputStream.write(HTTP_SEPARATOR)
    }

    companion object {
        fun parseHeadersFromBytes(bytes: ByteArray): Pair<Map<String, String>, Int> {
            var startIndex = 0
            val headers = mutableMapOf<String, String>()
            while (true) {
                val bytesLeft = bytes.sliceArray(startIndex until bytes.size)
                val separatorIndex = Bytes.indexOf(bytesLeft, HTTP_SEPARATOR)
                Preconditions.checkArgument(separatorIndex != -1)
                startIndex += separatorIndex + HTTP_SEPARATOR.size
                if (separatorIndex == 0) {
                    break
                }
                val headerBytes = bytesLeft.sliceArray(0 until separatorIndex)
                val headerLine = String(headerBytes, HTTP_ENCODING)
                val headerColumn = headerLine.indexOf(':')
                Preconditions.checkArgument(headerColumn != -1)
                val headerName = headerLine.substring(0, headerColumn)
                val headerValue = headerLine.substring(headerColumn + 2)
                headers[headerName] = headerValue
            }
            return Pair(headers.toMap(), startIndex)
        }
    }
}
