package io.github.dtolmachev.nodes.pattern;

import com.oracle.truffle.api.dsl.Fallback;
import com.oracle.truffle.api.dsl.GenerateInline;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.frame.VirtualFrame;

import io.github.dtolmachev.runtime.LamaArray;

@GenerateInline(false)
public abstract class LamaIsArrayPatternNode extends LamaPatternNode {
    @SuppressWarnings("unused")
    @Specialization
    public boolean checkArray(VirtualFrame frame, LamaArray scrutinee) {
        return true;
    }

    @SuppressWarnings("unused")
    @Fallback
    public boolean checkFalse(VirtualFrame frame, Object scrutinee) {
        return false;
    }
}
