package io.github.dtolmachev.builtins;

import com.oracle.truffle.api.CompilerDirectives.TruffleBoundary;
import com.oracle.truffle.api.dsl.Bind;
import com.oracle.truffle.api.dsl.Specialization;

import io.github.dtolmachev.runtime.LamaContext;

import java.io.IOException;

public abstract class LamaReadBuiltin extends LamaBuiltinNode {
    @Specialization
    @TruffleBoundary
    public long read(@Bind("getContext()") LamaContext ctx) {
        ctx.getOutput().print(" > ");
        try {
            return Long.parseLong(ctx.getInput().readLine());
        } catch (IOException e) {
            return 0;
        }
    }
}
