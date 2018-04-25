package ru.spbau.bachelors2015.roulette.protocol.http

/**
 * A simple URI that consists of resource path and a query line.
 */
class Uri(val path: ResourcePath, val queryLine: QueryLine?) {
    /**
     * Converts this URI to a string representation.
     */
    override fun toString(): String {
        return buildString {
            append(path)

            if (queryLine != null) {
                append(queryLineSeparator)
                append(queryLine)
            }
        }
    }

    companion object {
        const val queryLineSeparator = '?'
    }
}
