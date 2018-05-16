package ru.spbau.bachelors2015.roulette.protocol.http

/**
 * A class which represents a path to a resource: a sequence of strings that are separated with a
 * special character.
 */
class ResourcePath(elements: List<String>) {
    val elements: List<String> = elements.toList()

    init {
        for (element in elements) {
            if (element.isEmpty()) {
                throw IllegalArgumentException("Path element is empty")
            }

            if (element.contains(elementsSeparator)) {
                throw IllegalArgumentException(
                    "Path element contains forbidden character: $elementsSeparator"
                )
            }
        }
    }

    constructor(vararg elements: String) : this(elements.toList())

    /**
     * Converts this resource path to a string representation.
     */
    override fun toString(): String {
        return buildString {
            val pathString = elements.joinToString(elementsSeparator.toString()) { it }

            if (pathString.isNotEmpty()) {
                append(pathString)
            } else {
                append(elementsSeparator)
            }
        }
    }

    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (javaClass != other?.javaClass) return false

        other as ResourcePath

        if (elements != other.elements) return false

        return true
    }

    override fun hashCode(): Int {
        return elements.hashCode()
    }

    companion object {
        const val elementsSeparator = '/'
    }
}
