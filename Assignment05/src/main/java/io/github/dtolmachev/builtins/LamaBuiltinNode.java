package io.github.dtolmachev.builtins;

import com.oracle.truffle.api.dsl.GenerateNodeFactory;
import com.oracle.truffle.api.frame.VirtualFrame;

import io.github.dtolmachev.nodes.LamaNode;
import io.github.dtolmachev.runtime.LamaContext;

@GenerateNodeFactory
public abstract class LamaBuiltinNode extends LamaNode {
    public abstract Object execute(VirtualFrame frame, Object... arguments);

    protected LamaContext getContext() {
        return LamaContext.get(this);
    }
}
