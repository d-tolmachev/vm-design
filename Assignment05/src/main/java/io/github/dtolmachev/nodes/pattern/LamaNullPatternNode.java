package io.github.dtolmachev.nodes.pattern;

import com.oracle.truffle.api.dsl.Fallback;
import com.oracle.truffle.api.dsl.GenerateInline;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.frame.VirtualFrame;

import io.github.dtolmachev.runtime.LamaNull;

@GenerateInline(false)
public abstract class LamaNullPatternNode extends LamaPatternNode {
    @SuppressWarnings("unused")
    @Specialization
    public boolean checkNull(VirtualFrame frame, LamaNull scrutinee) {
        return true;
    }

    @SuppressWarnings("unused")
    @Fallback
    public boolean checkFalse(VirtualFrame frame, Object scrutinee) {
        return false;
    }
}
