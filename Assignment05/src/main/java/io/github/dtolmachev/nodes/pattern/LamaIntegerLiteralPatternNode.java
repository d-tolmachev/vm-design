package io.github.dtolmachev.nodes.pattern;

import com.oracle.truffle.api.dsl.Fallback;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.frame.VirtualFrame;

public abstract class LamaIntegerLiteralPatternNode extends LamaPatternNode {
    private final long value;

    public LamaIntegerLiteralPatternNode(long value) {
        this.value = value;
    }

    @SuppressWarnings("unused")
    @Specialization
    public boolean checkLong(VirtualFrame frame, long scrutinee) {
        return scrutinee == value;
    }

    @SuppressWarnings("unused")
    @Fallback
    public boolean checkFalse(VirtualFrame frame, Object scrutinee) {
        return false;
    }
}
