package io.github.dtolmachev.nodes;

import com.oracle.truffle.api.CompilerAsserts;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.ExplodeLoop;

public final class LamaSequenceNode extends LamaExpressionNode {
    @Children private LamaExpressionNode[] nodes;

    public LamaSequenceNode(LamaExpressionNode[] nodes) {
        this.nodes = nodes;
    }

    public LamaExpressionNode[] getNodes() {
        return nodes;
    }

    @Override
    @ExplodeLoop
    public Object execute(VirtualFrame frame) {
        CompilerAsserts.partialEvaluationConstant(nodes.length - 1);
        for (int i = 0; i < nodes.length - 1; i++) {
            nodes[i].execute(frame);
        }
        return nodes[nodes.length - 1].execute(frame);
    }
}
