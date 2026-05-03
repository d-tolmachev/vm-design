package io.github.dtolmachev.nodes.pattern;

import com.oracle.truffle.api.dsl.Fallback;
import com.oracle.truffle.api.dsl.GenerateInline;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.frame.VirtualFrame;

import io.github.dtolmachev.runtime.LamaString;

@GenerateInline(false)
public abstract class LamaIsStringPatternNode extends LamaPatternNode {
    @SuppressWarnings("unused")
    @Specialization
    public boolean checkString(VirtualFrame frame, LamaString scrutinee) {
        return true;
    }

    @SuppressWarnings("unused")
    @Fallback
    public boolean checkFalse(VirtualFrame frame, Object scrutinee) {
        return false;
    }
}
