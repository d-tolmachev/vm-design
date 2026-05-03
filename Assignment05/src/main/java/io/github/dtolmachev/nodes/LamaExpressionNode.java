package io.github.dtolmachev.nodes;

import com.oracle.truffle.api.CompilerDirectives.TruffleBoundary;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.RootNode;
import com.oracle.truffle.api.nodes.UnexpectedResultException;
import com.oracle.truffle.api.source.Source;
import com.oracle.truffle.api.source.SourceSection;

import io.github.dtolmachev.runtime.LamaFunction;

import java.util.Objects;

public abstract class LamaExpressionNode extends LamaNode {
    private static final int SOURCE_UNAVAILABLE = -2;
    private static final int SOURCE_MISSING = -1;
    private int charPositionInSource;
    private int length;

    public LamaExpressionNode() {
        this.charPositionInSource = SOURCE_MISSING;
        this.length = 0;
    }

    public abstract Object execute(VirtualFrame frame);

    public long executeLong(VirtualFrame frame) throws UnexpectedResultException {
        return LamaTypesGen.expectLong(execute(frame));
    }

    public LamaFunction executeFunction(VirtualFrame frame) throws UnexpectedResultException {
        return LamaTypesGen.expectLamaFunction(execute(frame));
    }

    @Override
    @TruffleBoundary
    public SourceSection getSourceSection() {
        if (charPositionInSource == SOURCE_MISSING) {
            return null;
        }
        RootNode rootNode = getRootNode();
        if (Objects.isNull(rootNode)) {
            return null;
        }
        SourceSection sourceSection = rootNode.getSourceSection();
        if (Objects.isNull(sourceSection)) {
            return null;
        }
        Source source = sourceSection.getSource();
        if (charPositionInSource == SOURCE_UNAVAILABLE) {
            return source.createUnavailableSection();
        }
        return source.createSection(charPositionInSource, length);
    }

    @TruffleBoundary
    public void setSourceSection(int charPositionInSource, int length) {
        if (charPositionInSource < 0) {
            throw new IllegalArgumentException("charPositionInSource < 0");
        }
        if (length < 0) {
            throw new IllegalArgumentException("length < 0");
        }
        this.charPositionInSource = charPositionInSource;
        this.length = length;
    }

    @TruffleBoundary
    public void setSourceSectionUnavailable() {
        this.charPositionInSource = SOURCE_UNAVAILABLE;
    }
}
