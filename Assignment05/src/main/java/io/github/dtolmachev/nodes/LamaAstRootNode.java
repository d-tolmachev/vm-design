package io.github.dtolmachev.nodes;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.source.SourceSection;
import com.oracle.truffle.api.strings.TruffleString;

import io.github.dtolmachev.LamaLanguage;

public final class LamaAstRootNode extends LamaRootNode {
    @Child private LamaExpressionNode bodyNode;
    private final SourceSection sourceSection;
    private final TruffleString name;

    public LamaAstRootNode(LamaLanguage language, FrameDescriptor frameDescriptor, LamaExpressionNode bodyNode, SourceSection sourceSection, TruffleString name) {
        super(language, frameDescriptor);
        this.bodyNode = bodyNode;
        this.sourceSection = sourceSection;
        this.name = name;
    }

    @Override
    public SourceSection getSourceSection() {
        return sourceSection;
    }

    @Override
    public LamaExpressionNode getBodyNode() {
        return bodyNode;
    }

    @Override
    public String getName() {
        return name.toJavaStringUncached();
    }

    @Override
    public Object execute(VirtualFrame frame) {
        return bodyNode.execute(frame);
    }
}
