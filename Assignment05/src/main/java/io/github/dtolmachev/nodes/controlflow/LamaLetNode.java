package io.github.dtolmachev.nodes.controlflow;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;

import io.github.dtolmachev.LamaException;
import io.github.dtolmachev.nodes.LamaExpressionNode;
import io.github.dtolmachev.nodes.pattern.LamaPatternNode;

@NodeInfo(shortName = "let")
public final class LamaLetNode extends LamaExpressionNode {
    @Child private LamaPatternNode patternNode;
    @Child private LamaExpressionNode scrutineeNode;
    @Child private LamaExpressionNode bodyNode;

    public LamaLetNode(LamaPatternNode patternNode, LamaExpressionNode scrutineeNode, LamaExpressionNode bodyNode) {
        this.patternNode = patternNode;
        this.scrutineeNode = scrutineeNode;
        this.bodyNode = bodyNode;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        Object scrutinee = scrutineeNode.execute(frame);
        if (!patternNode.execute(frame, scrutinee)) {
            throw LamaException.matchFailure(scrutineeNode);
        }
        return bodyNode.execute(frame);
    }
}
