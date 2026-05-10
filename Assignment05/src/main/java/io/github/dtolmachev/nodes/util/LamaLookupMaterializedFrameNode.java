package io.github.dtolmachev.nodes.util;

import com.oracle.truffle.api.frame.MaterializedFrame;
import com.oracle.truffle.api.frame.VirtualFrame;

import com.oracle.truffle.api.nodes.ExplodeLoop;
import io.github.dtolmachev.nodes.LamaNode;
import io.github.dtolmachev.nodes.LamaTypesGen;
import io.github.dtolmachev.runtime.LamaFunction;

public class LamaLookupMaterializedFrameNode extends LamaNode {
    private final int level;

    public LamaLookupMaterializedFrameNode(int level) {
        this.level = level;
    }

    @ExplodeLoop
    public MaterializedFrame execute(VirtualFrame frame) {
        LamaFunction function = LamaTypesGen.asLamaFunction(frame.getArguments()[0]);
        for (int i = 0; i < level; i++) {
            function = LamaTypesGen.asLamaFunction(function.getFrame().getArguments()[0]);
        }
        return function.getFrame();
    }
}
