package io.github.dtolmachev.nodes.controlflow;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;
import com.oracle.truffle.api.nodes.UnexpectedResultException;

import io.github.dtolmachev.LamaException;
import io.github.dtolmachev.nodes.LamaExpressionNode;
import io.github.dtolmachev.runtime.LamaNull;

@NodeInfo(shortName = "do-while")
public final class LamaDoWhileNode extends LamaExpressionNode {
    @Child private LamaExpressionNode bodyNode;
    @Child private LamaExpressionNode conditionNode;

    public LamaDoWhileNode(LamaExpressionNode bodyNode, LamaExpressionNode conditionNode) {
        this.bodyNode = bodyNode;
        this.conditionNode = conditionNode;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        try {
            do {
                bodyNode.execute(frame);
            } while (conditionNode.executeLong(frame) != 0);
        } catch (UnexpectedResultException e) {
            throw LamaException.typeError(conditionNode, "integer", e.getResult());
        }
        return LamaNull.SINGLETON;
    }
}
