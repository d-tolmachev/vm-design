package io.github.dtolmachev.nodes.pattern;

import com.oracle.truffle.api.dsl.Fallback;
import com.oracle.truffle.api.dsl.GenerateInline;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.frame.VirtualFrame;

@GenerateInline(false)
public abstract class LamaIsBoxPatternNode extends LamaPatternNode {
    @SuppressWarnings("unused")
    @Specialization
    public boolean checkLong(VirtualFrame frame, long scrutinee) {
        return false;
    }

    @SuppressWarnings("unused")
    @Fallback
    public boolean checkFalse(VirtualFrame frame, Object scrutinee) {
        return true;
    }
}
