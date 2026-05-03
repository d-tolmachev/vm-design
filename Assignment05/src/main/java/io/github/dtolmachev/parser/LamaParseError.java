package io.github.dtolmachev.parser;

import com.oracle.truffle.api.exception.AbstractTruffleException;
import com.oracle.truffle.api.source.Source;

public class LamaParseError extends AbstractTruffleException {
    private final Source source;
    private final int line;
    private final int column;
    private final int length;

    public LamaParseError(Source source, int line, int column, int length, String message) {
        super(message);
        this.source = source;
        this.line = line;
        this.column = column;
        this.length = length;
    }
}
