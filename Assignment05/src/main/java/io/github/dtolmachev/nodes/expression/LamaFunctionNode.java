package io.github.dtolmachev.nodes.expression;

import com.oracle.truffle.api.RootCallTarget;
import com.oracle.truffle.api.frame.VirtualFrame;

import io.github.dtolmachev.nodes.LamaExpressionNode;
import io.github.dtolmachev.runtime.LamaFunction;

public final class LamaFunctionNode extends LamaExpressionNode {
    private final RootCallTarget callTarget;
    private final int argumentsCount;
    private final boolean hasCaptures;

    public LamaFunctionNode(RootCallTarget callTarget, int argumentsCount, boolean hasCaptures) {
        this.callTarget = callTarget;
        this.argumentsCount = argumentsCount;
        this.hasCaptures = hasCaptures;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        return executeFunction(frame);
    }

    @Override
    public LamaFunction executeFunction(VirtualFrame frame) {
        return new LamaFunction(callTarget, argumentsCount, hasCaptures ? frame.materialize() : null);
    }
}
