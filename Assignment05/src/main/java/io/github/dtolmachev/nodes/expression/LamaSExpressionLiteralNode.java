package io.github.dtolmachev.nodes.expression;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.strings.TruffleString;

import io.github.dtolmachev.nodes.LamaExpressionNode;
import io.github.dtolmachev.runtime.LamaSExpression;

import java.util.ArrayList;
import java.util.List;

public final class LamaSExpressionLiteralNode extends LamaExpressionNode {
    private final TruffleString tag;
    @Children private LamaExpressionNode[] expressionNodes;

    public LamaSExpressionLiteralNode(TruffleString tag, LamaExpressionNode[] expressionNodes) {
        this.tag = tag;
        this.expressionNodes = expressionNodes;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        List<Object> expressions = new ArrayList<>();
        for (LamaExpressionNode expressionNode : expressionNodes) {
            expressions.add(expressionNode.execute(frame));
        }
        return new LamaSExpression(tag, expressions);
    }
}
