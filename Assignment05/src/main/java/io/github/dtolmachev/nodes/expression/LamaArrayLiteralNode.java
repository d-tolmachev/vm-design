package io.github.dtolmachev.nodes.expression;

import com.oracle.truffle.api.frame.VirtualFrame;

import com.oracle.truffle.api.nodes.ExplodeLoop;
import io.github.dtolmachev.nodes.LamaExpressionNode;
import io.github.dtolmachev.runtime.LamaArray;

import java.util.ArrayList;
import java.util.List;

public final class LamaArrayLiteralNode extends LamaExpressionNode {
    @Children private LamaExpressionNode[] elementNodes;

    public LamaArrayLiteralNode(LamaExpressionNode[] elementNodes) {
        this.elementNodes = elementNodes;
    }

    @Override
    @ExplodeLoop
    public Object execute(VirtualFrame frame) {
        List<Object> elements = new ArrayList<>();
        for (LamaExpressionNode elementNode : elementNodes) {
            elements.add(elementNode.execute(frame));
        }
        return new LamaArray(elements);
    }
}
