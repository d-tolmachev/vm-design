package io.github.dtolmachev.nodes.pattern;

import com.oracle.truffle.api.CompilerAsserts;
import com.oracle.truffle.api.dsl.Fallback;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.ExplodeLoop;
import com.oracle.truffle.api.strings.TruffleString;

import io.github.dtolmachev.runtime.LamaSExpression;

import java.util.stream.IntStream;

public abstract class LamaSExpressionLiteralPatternNode extends LamaPatternNode {
    protected final TruffleString tag;
    @Children protected LamaPatternNode[] expressionNodes;

    public LamaSExpressionLiteralPatternNode(TruffleString tag, LamaPatternNode[] expressionNodes) {
        this.tag = tag;
        this.expressionNodes = expressionNodes;
    }

    @Specialization(guards = {"scrutinee.getTag().equals(tag)", "scrutinee.getExpressions().size() == expressionNodes.length"})
    @ExplodeLoop
    public boolean checkSExpression(VirtualFrame frame, LamaSExpression scrutinee) {
        CompilerAsserts.partialEvaluationConstant(expressionNodes.length);
        return IntStream.iterate(0, i -> i + 1)
                .limit(expressionNodes.length)
                .allMatch(i -> expressionNodes[i].execute(frame, scrutinee.getExpressions().get(i)));
    }

    @SuppressWarnings("unused")
    @Fallback
    public boolean checkFalse(VirtualFrame frame, Object scrutinee) {
        return false;
    }
}
