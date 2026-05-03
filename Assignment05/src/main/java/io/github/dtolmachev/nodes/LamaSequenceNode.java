package io.github.dtolmachev.nodes;

import com.oracle.truffle.api.CompilerAsserts;
import com.oracle.truffle.api.frame.VirtualFrame;

import java.util.Arrays;

public final class LamaSequenceNode extends LamaExpressionNode {
    @Children private LamaExpressionNode[] nodes;

    public LamaSequenceNode(LamaExpressionNode[] nodes) {
        this.nodes = nodes;
    }

    public LamaExpressionNode[] getNodes() {
        return nodes;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        CompilerAsserts.partialEvaluationConstant(nodes.length - 1);
        Arrays.stream(nodes)
                .limit(nodes.length - 1)
                .forEach(node -> node.execute(frame));
        return nodes[nodes.length - 1].execute(frame);
    }
}
