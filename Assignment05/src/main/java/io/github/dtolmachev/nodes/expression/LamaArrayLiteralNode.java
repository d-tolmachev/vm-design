package io.github.dtolmachev.nodes.expression;

import com.oracle.truffle.api.frame.VirtualFrame;

import io.github.dtolmachev.nodes.LamaExpressionNode;
import io.github.dtolmachev.runtime.LamaArray;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public final class LamaArrayLiteralNode extends LamaExpressionNode {
    @Children private LamaExpressionNode[] elementNodes;

    public LamaArrayLiteralNode(LamaExpressionNode[] elementNodes) {
        this.elementNodes = elementNodes;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        List<Object> elements = Arrays.stream(elementNodes)
                .map(child -> child.execute(frame))
                .collect(ArrayList::new, ArrayList::add, ArrayList::addAll);
        return new LamaArray(elements);
    }
}
