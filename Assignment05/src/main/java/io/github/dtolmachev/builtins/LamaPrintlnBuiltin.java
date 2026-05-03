package io.github.dtolmachev.builtins;

import com.oracle.truffle.api.CompilerDirectives.TruffleBoundary;
import com.oracle.truffle.api.dsl.Cached;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.strings.TruffleString.ToJavaStringNode;

import io.github.dtolmachev.runtime.LamaNull;
import io.github.dtolmachev.runtime.LamaString;

public abstract class LamaPrintlnBuiltin extends LamaBuiltinNode {
    @Specialization
    @TruffleBoundary
    public LamaNull println(LamaString value, @Cached ToJavaStringNode toJavaStringNode) {
        System.out.println(toJavaStringNode.execute(value.getString()));
        return LamaNull.SINGLETON;
    }
}
