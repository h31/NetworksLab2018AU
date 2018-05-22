package network.http

import com.google.common.base.Preconditions
import com.google.common.io.ByteStreams
import com.google.common.primitives.Bytes
import network.HTTP_ENCODING
import network.HTTP_SEPARATOR
import network.HTTP_VERSION
import java.io.InputStream
import java.io.OutputStream
import java.util.*

open class HttpRequest(
    val method: HttpRequestMethod,
    val path: String,
    headers: Map<String, String> = emptyMap()
) : HttpEntity(headers) {

    override fun toString(): String = "HttpRequest(method=$method, path='$path', headers=$headers)"

    override fun writeToOutputStream(outputStream: OutputStream) {
        outputStream.write("$method $path $HTTP_VERSION".toByteArray(HTTP_ENCODING))
        outputStream.write(HTTP_SEPARATOR)
        writeHeadersToOutputStream(outputStream)
    }

    companion object {
        fun parseFromInputStream(inputStream: InputStream): HttpRequest {
            val bytes = ByteStreams.toByteArray(inputStream)
            val requestLineEnd = Bytes.indexOf(bytes, HTTP_SEPARATOR)
            Preconditions.checkArgument(requestLineEnd != -1)
            val requestLineBytes = Arrays.copyOf(bytes, requestLineEnd)
            val requestLine = String(requestLineBytes, HTTP_ENCODING)
            val parts = requestLine.split(" ")
            Preconditions.checkArgument(parts.size == 3)
            val method = HttpRequestMethod.valueOf(parts[0])
            val path = parts[1]
            Preconditions.checkArgument(parts[2] == HTTP_VERSION)

            val (headers, _) =
                parseHeadersFromBytes(
                    bytes.sliceArray(requestLineEnd + HTTP_SEPARATOR.size until bytes.size))
            return HttpRequest(method, path, headers)
        }
    }
}
