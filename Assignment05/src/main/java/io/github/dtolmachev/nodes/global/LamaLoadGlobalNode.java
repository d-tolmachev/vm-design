package io.github.dtolmachev.nodes.global;

import com.oracle.truffle.api.CompilerDirectives;
import com.oracle.truffle.api.dsl.Cached;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.strings.TruffleString;

import io.github.dtolmachev.nodes.LamaExpressionNode;
import io.github.dtolmachev.runtime.LamaContext;

import java.util.Objects;

public abstract class LamaLoadGlobalNode extends LamaExpressionNode {
    private final TruffleString name;

    public LamaLoadGlobalNode(TruffleString name) {
        this.name = name;
    }

    @Specialization
    public Object load(@Cached(value = "lookupGlobal()", neverDefault = true) Object value) {
        if (Objects.isNull(value)) {
            CompilerDirectives.shouldNotReachHere();
        }
        return value;
    }

    protected Object lookupGlobal() {
        return LamaContext.get(this).getFunctionRegistry().get(name);
    }
}
