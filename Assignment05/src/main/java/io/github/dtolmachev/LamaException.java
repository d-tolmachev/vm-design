package io.github.dtolmachev;

import com.oracle.truffle.api.CompilerDirectives.TruffleBoundary;
import com.oracle.truffle.api.exception.AbstractTruffleException;
import com.oracle.truffle.api.nodes.Node;
import com.oracle.truffle.api.nodes.NodeInfo;
import com.oracle.truffle.api.source.SourceSection;

import io.github.dtolmachev.nodes.LamaTypes;

import java.util.Objects;

public final class LamaException extends AbstractTruffleException {
    public LamaException(String message) {
        super(message);
    }

    @TruffleBoundary
    public static AbstractTruffleException typeErrorOperator(Node operation, Object... values) {
        NodeInfo nodeInfo = LamaLanguage.lookupNodeInfo(operation.getClass());
        String operationName = Objects.nonNull(nodeInfo) ? nodeInfo.shortName() : null;
        return typeError(operation, operationName, values);
    }

    @SuppressWarnings("DuplicatedCode")
    @TruffleBoundary
    public static AbstractTruffleException typeErrorOperator(Node location, String operationName, Object... values) {
        StringBuilder result = new StringBuilder();
        result.append("Type error")
                .append(nodeLocation(location))
                .append(": operation \"")
                .append(operationName)
                .append("\" not defined for ");
        boolean isFirst = true;
        for (Object value : values) {
            if (!isFirst) {
                result.append(", ");
            }
            isFirst = false;
            result.append(LamaTypes.typeOf(value));
        }
        return new LamaException(result.toString());
    }

    @TruffleBoundary
    public static AbstractTruffleException typeError(Node location, String expected, Object actual) {
        return new LamaException("Type error%s: expected %s, but got %s".formatted(nodeLocation(location), expected, LamaTypes.typeOf(actual)));
    }

    @SuppressWarnings("DuplicatedCode")
    @TruffleBoundary
    public static AbstractTruffleException typeErrorCall(Node location, Object... values) {
        StringBuilder result = new StringBuilder();
        result.append("Type error")
                .append(nodeLocation(location))
                .append(": cannot be applied to ");
        boolean isFirst = true;
        for (Object value : values) {
            if (!isFirst) {
                result.append(", ");
            }
            isFirst = false;
            result.append(LamaTypes.typeOf(value));
        }
        return new LamaException(result.toString());
    }

    @TruffleBoundary
    public static AbstractTruffleException matchFailure(Node location) {
        return new LamaException("Match failure%s: no case branch matches scrutinee".formatted(nodeLocation(location)));
    }

    @TruffleBoundary
    public static AbstractTruffleException argumentCountMismatch(Node location, int expected, int actual) {
        return new LamaException("Argument count mismatch%s: expected %d, but got %d".formatted(nodeLocation(location), expected, actual));
    }

    @TruffleBoundary
    public static AbstractTruffleException outOfRange(Node location, int size, int index) {
        return new LamaException("Index out of range%s: index is %d, while the size is %d".formatted(nodeLocation(location), index, size));
    }

    @TruffleBoundary
    public static AbstractTruffleException illegalChar(Node location, int chr) {
        return new LamaException("Illegal char%s: must be between 0 and 255, but got %d".formatted(nodeLocation(location), chr));
    }

    private static String nodeLocation(Node location) {
        if (Objects.nonNull(location)) {
            SourceSection ss = location.getEncapsulatingSourceSection();
            if (Objects.nonNull(ss)) {
                return " at %s line %d column %d".formatted(ss.getSource().getName(), ss.getStartLine(), ss.getStartColumn());
            }
        }
        return "";
    }
}
