package io.github.dtolmachev.nodes.controlflow;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;
import com.oracle.truffle.api.nodes.UnexpectedResultException;

import io.github.dtolmachev.LamaException;
import io.github.dtolmachev.nodes.LamaExpressionNode;

@NodeInfo(shortName = "if")
public final class LamaIfNode extends LamaExpressionNode {
    @Child private LamaExpressionNode conditionNode;
    @Child private LamaExpressionNode thenNode;
    @Child private LamaExpressionNode elseNode;

    public LamaIfNode(LamaExpressionNode conditionNode, LamaExpressionNode thenNode, LamaExpressionNode elseNode) {
        this.conditionNode = conditionNode;
        this.thenNode = thenNode;
        this.elseNode = elseNode;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        try {
            if (conditionNode.executeLong(frame) != 0) {
                return thenNode.execute(frame);
            } else {
                return elseNode.execute(frame);
            }
        } catch (UnexpectedResultException e) {
            throw LamaException.typeError(conditionNode, "integer", e.getResult());
        }
    }
}
