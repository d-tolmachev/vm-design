package io.github.dtolmachev.runtime;

import com.oracle.truffle.api.RootCallTarget;
import com.oracle.truffle.api.frame.MaterializedFrame;
import com.oracle.truffle.api.interop.TruffleObject;

@SuppressWarnings("ClassCanBeRecord")
public final class LamaFunction implements TruffleObject {
    private final RootCallTarget callTarget;
    private final int argumentsCount;
    private final MaterializedFrame frame;

    public LamaFunction(RootCallTarget callTarget, int argumentsCount, MaterializedFrame frame) {
        this.callTarget = callTarget;
        this.argumentsCount = argumentsCount;
        this.frame = frame;
    }

    public RootCallTarget getCallTarget() {
        return callTarget;
    }

    public int getArgumentsCount() {
        return argumentsCount;
    }

    public MaterializedFrame getFrame() {
        return frame;
    }
}
