package io.github.dtolmachev.nodes.controlflow;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.ExplodeLoop;
import com.oracle.truffle.api.nodes.NodeInfo;

import io.github.dtolmachev.LamaException;
import io.github.dtolmachev.nodes.LamaExpressionNode;
import io.github.dtolmachev.nodes.pattern.LamaPatternNode;

@NodeInfo(shortName = "case")
public final class LamaCaseNode extends LamaExpressionNode {
    @Child private LamaExpressionNode scrutineeNode;
    @Children private LamaPatternNode[] patternNodes;
    @Children private LamaExpressionNode[] branchNodes;

    public LamaCaseNode(LamaExpressionNode scrutineeNode, LamaPatternNode[] patternNodes, LamaExpressionNode[] branchNodes) {
        this.scrutineeNode = scrutineeNode;
        this.patternNodes = patternNodes;
        this.branchNodes = branchNodes;
    }

    @Override
    @ExplodeLoop
    public Object execute(VirtualFrame frame) {
        Object scrutinee = scrutineeNode.execute(frame);
        for (int i = 0; i < patternNodes.length; i++) {
            if (patternNodes[i].execute(frame, scrutinee)) {
                return branchNodes[i].execute(frame);
            }
        }
        throw LamaException.matchFailure(scrutineeNode);
    }
}
