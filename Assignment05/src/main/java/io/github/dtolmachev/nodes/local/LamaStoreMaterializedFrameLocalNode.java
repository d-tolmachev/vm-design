package io.github.dtolmachev.nodes.local;

import com.oracle.truffle.api.dsl.NodeChild;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.frame.MaterializedFrame;

import io.github.dtolmachev.nodes.LamaExpressionNode;

@NodeChild("valueNode")
public abstract class LamaStoreMaterializedFrameLocalNode extends LamaExpressionNode {
    private final MaterializedFrame frame;
    private final int slot;

    public LamaStoreMaterializedFrameLocalNode(MaterializedFrame frame, int slot) {
        this.frame = frame;
        this.slot = slot;
    }

    @Specialization
    public long storeLong(long value) {
        frame.setLong(slot, value);
        return value;
    }

    @Specialization
    public Object storeObject(Object value) {
        frame.setObject(slot, value);
        return value;
    }
}
