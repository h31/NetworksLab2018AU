package utils;

public class UnknownStatusCodeException extends Exception {
    public UnknownStatusCodeException(int code) {
        super("Unknown HTTP code: " + String.valueOf(code));
    }
}
