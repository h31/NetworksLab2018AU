package ru.spbau.bachelors2015.roulette.protocol

/**
 * A simple URI that consists of path elements and a query line.
 */
class Uri(val path: List<String>, val queryLine: QueryLine?) {
    init {
        for (pathElement in path) {
            if (pathElement.contains(elementsSeparator) ||
                pathElement.contains(queryLineSeparator)
            ) {
                throw IllegalArgumentException(
                    "Path element contains forbidden characters:" +
                    "$elementsSeparator, $queryLineSeparator"
                )
            }
        }
    }

    /**
     * Converts this URI to a string representation. Path elements are separated with a special
     * character and query line is placed at the end of resulting string.
     */
    override fun toString(): String {
        return buildString {
            val pathString = path.joinToString(elementsSeparator.toString()) { it }

            if (pathString.isNotEmpty()) {
                append(pathString)
            } else {
                append(elementsSeparator)
            }

            if (queryLine != null) {
                append(queryLineSeparator)
                append(queryLine)
            }
        }
    }

    companion object {
        const val elementsSeparator = '/'

        const val queryLineSeparator = '?'
    }
}
