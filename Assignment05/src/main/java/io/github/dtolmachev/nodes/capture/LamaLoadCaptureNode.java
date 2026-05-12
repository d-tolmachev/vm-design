package io.github.dtolmachev.nodes.capture;

import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.frame.FrameSlotTypeException;
import com.oracle.truffle.api.frame.VirtualFrame;

import io.github.dtolmachev.nodes.LamaExpressionNode;
import io.github.dtolmachev.nodes.util.LamaLookupMaterializedFrameNode;

public abstract class LamaLoadCaptureNode extends LamaExpressionNode {
    @Child private LamaLookupMaterializedFrameNode lookupMaterializedFrameNode;
    private final int slot;

    public LamaLoadCaptureNode(int level, int slot) {
        this.lookupMaterializedFrameNode = new LamaLookupMaterializedFrameNode(level);
        this.slot = slot;
    }

    @Specialization(rewriteOn = FrameSlotTypeException.class)
    public long loadLong(VirtualFrame frame) {
        return lookupMaterializedFrameNode.execute(frame).getLong(slot);
    }

    @Specialization(rewriteOn = FrameSlotTypeException.class)
    public Object loadObject(VirtualFrame frame) {
        return lookupMaterializedFrameNode.execute(frame).getObject(slot);
    }

    @Specialization(replaces = {"loadObject", "loadLong"})
    public Object load(VirtualFrame frame) {
        return lookupMaterializedFrameNode.execute(frame).getValue(slot);
    }
}
