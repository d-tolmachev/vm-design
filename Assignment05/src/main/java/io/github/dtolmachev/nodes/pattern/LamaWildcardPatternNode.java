package io.github.dtolmachev.nodes.pattern;

import com.oracle.truffle.api.frame.VirtualFrame;

public final class LamaWildcardPatternNode extends LamaPatternNode {
    @Override
    public boolean execute(VirtualFrame frame, Object scrutinee) {
        return true;
    }
}
