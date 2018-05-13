package ru.spbau.bachelors2015.roulette.protocol.http

/**
 * A query line that can appear in URI. It consists of key value pairs.
 */
class QueryLine(keyValuePairs: Map<String, String>) {
    constructor(
        firstPair: Pair<String, String>,
        vararg keyValuePairs: Pair<String, String>
    ) : this(keyValuePairs.union(listOf(firstPair)).toMap())

    val keyValuePairs: Map<String, String> = keyValuePairs.toMap()

    init {
        if (keyValuePairs.isEmpty()) {
            throw IllegalArgumentException("Empty query line is forbidden")
        }

        for (entry in keyValuePairs) {
            if (entry.key.contains(pairsSeparator) ||
                entry.key.contains(keyValuePairSeparator) ||
                entry.value.contains(pairsSeparator) ||
                entry.value.contains(keyValuePairSeparator)
            ) {
                throw IllegalArgumentException(
                    "Key value pair contains forbidden characters:" +
                    "$pairsSeparator, $keyValuePairSeparator"
                )
            }
        }
    }

    /**
     * Converts this query line to a string representation. Pairs are separated with a special
     * character. Key and value inside each pair are also separated with a special character.
     */
    override fun toString(): String {
        return keyValuePairs.entries.joinToString(pairsSeparator.toString()) {
            "${it.key}$keyValuePairSeparator${it.value}"
        }
    }

    companion object {
        const val pairsSeparator = '&'

        const val keyValuePairSeparator = '='
    }
}
