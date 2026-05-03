package io.github.dtolmachev.nodes.local;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.UnexpectedResultException;

import io.github.dtolmachev.nodes.LamaExpressionNode;

public final class LamaLoadLocalNode extends LamaExpressionNode {
    private final int slot;

    public LamaLoadLocalNode(int slot) {
        this.slot = slot;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        return frame.getValue(slot);
    }

    @SuppressWarnings("RedundantThrows")
    @Override
    public long executeLong(VirtualFrame frame) throws UnexpectedResultException {
        return frame.getLong(slot);
    }
}
