package ru.spbau.bachelors2015.roulette.protocol

/**
 * A query line that can appear in URI. It consists of key value pairs.
 */
class QueryLine(val keyValuePairs: Map<String, String>) {
    init {
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
