package io.github.dtolmachev.nodes.capture;

import com.oracle.truffle.api.CompilerDirectives;
import com.oracle.truffle.api.frame.MaterializedFrame;
import com.oracle.truffle.api.frame.VirtualFrame;

import io.github.dtolmachev.nodes.LamaExpressionNode;
import io.github.dtolmachev.nodes.local.LamaStoreMaterializedFrameLocalNode;
import io.github.dtolmachev.nodes.local.LamaStoreMaterializedFrameLocalNodeGen;
import io.github.dtolmachev.nodes.util.LamaLookupMaterializedFrameNode;

public final class LamaStoreCaptureNode extends LamaExpressionNode {
    @Child private LamaLookupMaterializedFrameNode lookupMaterializedFrameNode;
    private final int slot;
    @Child private LamaExpressionNode valueNode;

    public LamaStoreCaptureNode(int level, int slot, LamaExpressionNode valueNode) {
        this.lookupMaterializedFrameNode = new LamaLookupMaterializedFrameNode(level);
        this.slot = slot;
        this.valueNode = valueNode;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        MaterializedFrame materializedFrame = lookupMaterializedFrameNode.execute(frame);
        CompilerDirectives.transferToInterpreterAndInvalidate();
        LamaStoreMaterializedFrameLocalNode node = LamaStoreMaterializedFrameLocalNodeGen.create(materializedFrame, slot, valueNode);
        replace(node);
        return node.execute(frame);
    }
}
