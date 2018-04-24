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
            append(path.joinToString(elementsSeparator.toString()) { it })

            if (queryLine != null) {
                append(queryLineSeparator)
                append(queryLine)
            }
        }
    }

    companion object {
        private const val elementsSeparator = '/'

        private const val queryLineSeparator = '?'
    }
}
