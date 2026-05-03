package io.github.dtolmachev.nodes.controlflow;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;
import com.oracle.truffle.api.nodes.UnexpectedResultException;

import io.github.dtolmachev.LamaException;
import io.github.dtolmachev.nodes.LamaExpressionNode;
import io.github.dtolmachev.runtime.LamaNull;

@NodeInfo(shortName = "while-do")
public final class LamaWhileDoNode extends LamaExpressionNode {
    @Child private LamaExpressionNode conditionNode;
    @Child private LamaExpressionNode bodyNode;

    public LamaWhileDoNode(LamaExpressionNode conditionNode, LamaExpressionNode bodyNode) {
        this.conditionNode = conditionNode;
        this.bodyNode = bodyNode;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        try {
            while (conditionNode.executeLong(frame) != 0) {
                bodyNode.execute(frame);
            }
        } catch (UnexpectedResultException e) {
            throw LamaException.typeError(conditionNode, "integer", e.getResult());
        }
        return LamaNull.SINGLETON;
    }
}
