package io.github.dtolmachev.nodes.controlflow;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;
import com.oracle.truffle.api.nodes.UnexpectedResultException;

import io.github.dtolmachev.LamaException;
import io.github.dtolmachev.nodes.LamaExpressionNode;
import io.github.dtolmachev.runtime.LamaNull;

@NodeInfo(shortName = "for")
public final class LamaForNode extends LamaExpressionNode {
    @Child private LamaExpressionNode initializerNode;
    @Child private LamaExpressionNode conditionNode;
    @Child private LamaExpressionNode stepNode;
    @Child private LamaExpressionNode bodyNode;

    public LamaForNode(LamaExpressionNode initializerNode, LamaExpressionNode conditionNode, LamaExpressionNode stepNode, LamaExpressionNode bodyNode) {
        this.initializerNode = initializerNode;
        this.conditionNode = conditionNode;
        this.stepNode = stepNode;
        this.bodyNode = bodyNode;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        try {
            for (initializerNode.execute(frame); conditionNode.executeLong(frame) != 0; stepNode.execute(frame)) {
                bodyNode.execute(frame);
            }
        } catch (UnexpectedResultException e) {
            throw LamaException.typeError(conditionNode, "integer", e.getResult());
        }
        return LamaNull.SINGLETON;
    }
}
