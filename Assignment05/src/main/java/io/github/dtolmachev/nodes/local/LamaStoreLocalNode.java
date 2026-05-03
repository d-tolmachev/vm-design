package io.github.dtolmachev.nodes.local;

import com.oracle.truffle.api.dsl.NodeChild;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.frame.VirtualFrame;

import io.github.dtolmachev.nodes.LamaExpressionNode;

@NodeChild("valueNode")
public abstract class LamaStoreLocalNode extends LamaExpressionNode {
    private final int slot;

    public LamaStoreLocalNode(int slot) {
        this.slot = slot;
    }

    @Specialization
    public long storeLong(VirtualFrame frame, long value) {
        frame.setLong(slot, value);
        return value;
    }

    @Specialization
    public Object storeObject(VirtualFrame frame, Object value) {
        frame.setObject(slot, value);
        return value;
    }
}
