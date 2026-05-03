package io.github.dtolmachev.nodes.expression;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.strings.TruffleString;

import io.github.dtolmachev.nodes.LamaExpressionNode;
import io.github.dtolmachev.runtime.LamaSExpression;

import java.util.ArrayList;
import java.util.Arrays;
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
        List<Object> expressions = Arrays.stream(expressionNodes)
                .map(child -> child.execute(frame))
                .collect(ArrayList::new, ArrayList::add, ArrayList::addAll);
        return new LamaSExpression(tag, expressions);
    }
}
