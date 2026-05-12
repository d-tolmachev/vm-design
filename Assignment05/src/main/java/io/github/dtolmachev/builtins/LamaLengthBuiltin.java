package io.github.dtolmachev.builtins;

import com.oracle.truffle.api.dsl.GenerateInline;
import com.oracle.truffle.api.dsl.Specialization;

import io.github.dtolmachev.LamaLanguage;
import io.github.dtolmachev.runtime.LamaArray;
import io.github.dtolmachev.runtime.LamaSExpression;
import io.github.dtolmachev.runtime.LamaString;

@GenerateInline(false)
public abstract class LamaLengthBuiltin extends LamaBuiltinNode {
    @Specialization
    public long stringLength(LamaString string) {
        return string.getString().byteLength(LamaLanguage.STRING_ENCODING);
    }

    @Specialization
    public long sExpressionLength(LamaSExpression sExpression) {
        return sExpression.getExpressions().size();
    }

    @Specialization
    public long arrayLength(LamaArray array) {
        return array.getElements().size();
    }
}
