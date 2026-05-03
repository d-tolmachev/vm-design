package io.github.dtolmachev.nodes.pattern;

import com.oracle.truffle.api.dsl.Fallback;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.frame.VirtualFrame;

public abstract class LamaNameReferencePatternNode extends LamaPatternNode {
    private final int slot;
    @Child LamaPatternNode innerNode;

    public LamaNameReferencePatternNode(int slot, LamaPatternNode innerNode) {
        this.slot = slot;
        this.innerNode = innerNode;
    }

    @Specialization
    public boolean checkLong(VirtualFrame frame, long scrutinee) {
        frame.setLong(slot, scrutinee);
        return innerNode.execute(frame, scrutinee);
    }

    @Fallback
    public boolean checkObject(VirtualFrame frame, Object scrutinee) {
        frame.setObject(slot, scrutinee);
        return innerNode.execute(frame, scrutinee);
    }
}
