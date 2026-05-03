package io.github.dtolmachev.nodes.argument;

import com.oracle.truffle.api.frame.VirtualFrame;

import io.github.dtolmachev.nodes.LamaExpressionNode;

public final class LamaLoadArgumentNode extends LamaExpressionNode {
    private final int index;

    public LamaLoadArgumentNode(int index) {
        this.index = index;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        return frame.getArguments()[index];
    }
}
