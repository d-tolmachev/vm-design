package io.github.dtolmachev.nodes.expression;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;
import com.oracle.truffle.api.nodes.UnexpectedResultException;

import io.github.dtolmachev.LamaException;
import io.github.dtolmachev.nodes.LamaExpressionNode;

@NodeInfo(shortName = "<")
public final class LamaLessNode extends LamaExpressionNode {
    @Child private LamaExpressionNode leftNode;
    @Child private LamaExpressionNode rightNode;

    public LamaLessNode(LamaExpressionNode leftNode, LamaExpressionNode rightNode) {
        this.leftNode = leftNode;
        this.rightNode = rightNode;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        return executeLong(frame);
    }

    @SuppressWarnings("DuplicatedCode")
    @Override
    public long executeLong(VirtualFrame frame) {
        long left;
        long right;
        try {
            left = leftNode.executeLong(frame);
        } catch (UnexpectedResultException e) {
            throw LamaException.typeError(leftNode, "integer", e.getResult());
        }
        try {
            right = rightNode.executeLong(frame);
        } catch (UnexpectedResultException e) {
            throw LamaException.typeError(rightNode, "integer", e.getResult());
        }
        return left < right ? 1 : 0;
    }
}
