package io.github.dtolmachev.nodes.pattern;

import com.oracle.truffle.api.frame.VirtualFrame;

import io.github.dtolmachev.nodes.LamaNode;

public abstract class LamaPatternNode extends LamaNode {
    public abstract boolean execute(VirtualFrame frame, Object scrutinee);
}
