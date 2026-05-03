package io.github.dtolmachev.builtins;

import com.oracle.truffle.api.dsl.UnsupportedSpecializationException;
import com.oracle.truffle.api.frame.VirtualFrame;

import io.github.dtolmachev.LamaException;
import io.github.dtolmachev.nodes.LamaExpressionNode;

import java.util.Arrays;

public final class LamaEvalBuiltin extends LamaExpressionNode {
    @Child private LamaBuiltinNode lamaBuiltin;

    public LamaEvalBuiltin(LamaBuiltinNode lamaBuiltin) {
        this.lamaBuiltin = lamaBuiltin;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        Object[] arguments = Arrays.copyOfRange(frame.getArguments(), 1, frame.getArguments().length);
        try {
            return lamaBuiltin.execute(frame, arguments);
        } catch (UnsupportedSpecializationException e) {
            throw LamaException.typeErrorCall(this, arguments);
        }
    }
}
