package server

fun <T: Comparable<T>> merge(l: Iterable<T>, r: Iterable<T>): List<T> {
    if (l.none()) {
        return r.toList()
    } else if (r.none()) {
        return l.toList()
    }

    val result: MutableList<T> = mutableListOf()

    val iteratorR = r.iterator()
    var nextR: T? = iteratorR.next()
    for (nextL in l) {
        while (nextR != null && nextR < nextL) {
            result += nextR
            nextR = if (iteratorR.hasNext()) {
                iteratorR.next()
            } else {
                null
            }
        }
        result += nextL
    }

    while (nextR != null) {
        result += nextR
        nextR = if (iteratorR.hasNext()) {
            iteratorR.next()
        } else {
            null
        }
    }

    return result
}