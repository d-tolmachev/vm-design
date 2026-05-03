package io.github.dtolmachev.builtins;

import com.oracle.truffle.api.CompilerDirectives.TruffleBoundary;
import com.oracle.truffle.api.dsl.Bind;
import com.oracle.truffle.api.dsl.Specialization;

import io.github.dtolmachev.runtime.LamaContext;
import io.github.dtolmachev.runtime.LamaNull;

public abstract class LamaWriteBuiltin extends LamaBuiltinNode {
    @Specialization
    @TruffleBoundary
    public Object write(long value, @Bind("getContext()") LamaContext ctx) {
        ctx.getOutput().println(value);
        return LamaNull.SINGLETON;
    }
}
