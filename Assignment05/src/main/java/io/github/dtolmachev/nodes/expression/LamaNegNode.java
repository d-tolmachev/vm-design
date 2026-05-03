package io.github.dtolmachev.nodes.expression;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;
import com.oracle.truffle.api.nodes.UnexpectedResultException;

import io.github.dtolmachev.LamaException;
import io.github.dtolmachev.nodes.LamaExpressionNode;

@NodeInfo(shortName = "-")
public final class LamaNegNode extends LamaExpressionNode {
    @Child private LamaExpressionNode valueNode;

    public LamaNegNode(LamaExpressionNode valueNode) {
        this.valueNode = valueNode;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        return executeLong(frame);
    }

    @Override
    public long executeLong(VirtualFrame frame) {
        long value;
        try {
            value = valueNode.executeLong(frame);
        } catch (UnexpectedResultException e) {
            throw LamaException.typeError(valueNode, "integer", e.getResult());
        }
        return -value;
    }
}
