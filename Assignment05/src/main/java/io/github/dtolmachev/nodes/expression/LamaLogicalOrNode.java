package io.github.dtolmachev.nodes.expression;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;
import com.oracle.truffle.api.nodes.UnexpectedResultException;

import io.github.dtolmachev.LamaException;
import io.github.dtolmachev.nodes.LamaExpressionNode;

@NodeInfo(shortName = "!!")
public final class LamaLogicalOrNode extends LamaExpressionNode {
    @Child private LamaExpressionNode leftNode;
    @Child private LamaExpressionNode rightNode;

    public LamaLogicalOrNode(LamaExpressionNode leftNode, LamaExpressionNode rightNode) {
        this.leftNode = leftNode;
        this.rightNode = rightNode;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        return executeLong(frame);
    }

    @Override
    public long executeLong(VirtualFrame frame) {
        try {
            if (leftNode.executeLong(frame) != 0) {
                return 1;
            }
        } catch (UnexpectedResultException e) {
            throw LamaException.typeError(leftNode, "integer", e.getResult());
        }
        try {
            if (rightNode.executeLong(frame) != 0) {
                return 1;
            }
        } catch (UnexpectedResultException e) {
            throw LamaException.typeError(rightNode, "integer", e.getResult());
        }
        return 0;
    }
}
