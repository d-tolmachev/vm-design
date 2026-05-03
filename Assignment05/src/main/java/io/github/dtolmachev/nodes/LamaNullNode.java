package io.github.dtolmachev.nodes;

import com.oracle.truffle.api.frame.VirtualFrame;

import io.github.dtolmachev.runtime.LamaNull;

public final class LamaNullNode extends LamaExpressionNode {
    @Override
    public Object execute(VirtualFrame frame) {
        return LamaNull.SINGLETON;
    }
}
