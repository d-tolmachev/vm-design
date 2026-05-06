package io.github.dtolmachev.nodes.pattern;

import com.oracle.truffle.api.CompilerAsserts;
import com.oracle.truffle.api.dsl.Fallback;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.frame.VirtualFrame;

import io.github.dtolmachev.runtime.LamaArray;

public abstract class LamaArrayLiteralPatternNode extends LamaPatternNode {
    @Children protected LamaPatternNode[] elementNodes;

    public LamaArrayLiteralPatternNode(LamaPatternNode[] elementNodes) {
        this.elementNodes = elementNodes;
    }

    @Specialization(guards = "scrutinee.getElements().size() == elementNodes.length")
    public boolean checkArray(VirtualFrame frame, LamaArray scrutinee) {
        CompilerAsserts.partialEvaluationConstant(elementNodes.length);
        for (int i = 0; i < elementNodes.length; i++) {
            if (!elementNodes[i].execute(frame, scrutinee.getElements().get(i))) {
                return false;
            }
        }
        return true;
    }

    @SuppressWarnings("unused")
    @Fallback
    public boolean checkFalse(VirtualFrame frame, Object scrutinee) {
        return false;
    }
}
