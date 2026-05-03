package io.github.dtolmachev.nodes;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.nodes.RootNode;

import io.github.dtolmachev.LamaLanguage;

public abstract class LamaRootNode extends RootNode {
    public LamaRootNode(LamaLanguage language, FrameDescriptor frameDescriptor) {
        super(language, frameDescriptor);
    }

    public abstract LamaExpressionNode getBodyNode();
}
