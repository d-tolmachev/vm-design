package io.github.dtolmachev.runtime;

import com.oracle.truffle.api.interop.TruffleObject;
import com.oracle.truffle.api.strings.TruffleString;

import java.util.List;

@SuppressWarnings("ClassCanBeRecord")
public final class LamaSExpression implements TruffleObject {
    private final TruffleString tag;
    private final List<Object> expressions;

    public LamaSExpression(TruffleString tag, List<Object> expressions) {
        this.tag = tag;
        this.expressions = expressions;
    }

    public TruffleString getTag() {
        return tag;
    }

    public List<Object> getExpressions() {
        return expressions;
    }
}
