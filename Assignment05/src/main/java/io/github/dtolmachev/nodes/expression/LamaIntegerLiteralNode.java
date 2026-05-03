package io.github.dtolmachev.nodes.expression;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;
import io.github.dtolmachev.nodes.LamaExpressionNode;

@NodeInfo(shortName = "integerLiteral")
public final class LamaIntegerLiteralNode extends LamaExpressionNode {
    private final long value;

    public LamaIntegerLiteralNode(long value) {
        this.value = value;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        return executeLong(frame);
    }

    @Override
    public long executeLong(VirtualFrame frame) {
        return value;
    }
}
