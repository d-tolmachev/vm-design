package io.github.dtolmachev.nodes.controlflow;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;

import io.github.dtolmachev.LamaException;
import io.github.dtolmachev.nodes.LamaExpressionNode;
import io.github.dtolmachev.nodes.pattern.LamaPatternNode;

import java.util.stream.IntStream;

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
    public Object execute(VirtualFrame frame) {
        Object scrutinee = scrutineeNode.execute(frame);
        return IntStream.iterate(0, i -> i + 1)
                .limit(patternNodes.length)
                .dropWhile(i -> !patternNodes[i].execute(frame, scrutinee))
                .mapToObj(i -> branchNodes[i].execute(frame))
                .findFirst()
                .orElseThrow(() -> LamaException.matchFailure(scrutineeNode));
    }
}
