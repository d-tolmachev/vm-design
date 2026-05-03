package io.github.dtolmachev.nodes;

import com.oracle.truffle.api.CompilerDirectives.TruffleBoundary;
import com.oracle.truffle.api.dsl.TypeCast;
import com.oracle.truffle.api.dsl.TypeCheck;
import com.oracle.truffle.api.dsl.TypeSystem;

import io.github.dtolmachev.runtime.LamaArray;
import io.github.dtolmachev.runtime.LamaFunction;
import io.github.dtolmachev.runtime.LamaNull;
import io.github.dtolmachev.runtime.LamaSExpression;
import io.github.dtolmachev.runtime.LamaString;

@TypeSystem({long.class, LamaString.class, LamaSExpression.class, LamaArray.class, LamaFunction.class})
public abstract class LamaTypes {
    @TypeCheck(LamaNull.class)
    public static boolean isLamaNull(Object value) {
        return value == LamaNull.SINGLETON;
    }

    @TypeCast(LamaNull.class)
    public static LamaNull asLamaNull(Object value) {
        assert(isLamaNull(value));
        return LamaNull.SINGLETON;
    }

    @TruffleBoundary
    public static String typeOf(Object value) {
        if (LamaTypesGen.isLong(value)) {
            return "integer";
        }
        if (LamaTypesGen.isLamaNull(value)) {
            return "<null>";
        }
        if (LamaTypesGen.isLamaString(value)) {
            return "string";
        }
        if (LamaTypesGen.isLamaSExpression(value)) {
            return "s-expression";
        }
        if (LamaTypesGen.isLamaArray(value)) {
            return "array";
        }
        if (LamaTypesGen.isLamaFunction(value)) {
            return "function";
        }
        return "<%s>".formatted(value.getClass().getTypeName());
    }
}
