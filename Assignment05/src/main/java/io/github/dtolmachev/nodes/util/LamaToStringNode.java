package io.github.dtolmachev.nodes.util;

import com.oracle.truffle.api.CompilerDirectives.TruffleBoundary;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.strings.TruffleString.ToJavaStringNode;

import io.github.dtolmachev.nodes.LamaNode;
import io.github.dtolmachev.runtime.LamaArray;
import io.github.dtolmachev.runtime.LamaFunction;
import io.github.dtolmachev.runtime.LamaSExpression;
import io.github.dtolmachev.runtime.LamaString;

public abstract class LamaToStringNode extends LamaNode {
    @Child private ToJavaStringNode toJavaStringNode;

    public LamaToStringNode() {
        this.toJavaStringNode = ToJavaStringNode.create();
    }

    public abstract void execute(StringBuilder stringBuilder, Object value);

    @TruffleBoundary
    public String createString(Object value) {
        StringBuilder stringBuilder = new StringBuilder();
        execute(stringBuilder, value);
        return stringBuilder.toString();
    }

    @Specialization
    @TruffleBoundary
    public void stringInteger(StringBuilder stringBuilder, long integer) {
        stringBuilder.append(integer);
    }

    @Specialization
    @TruffleBoundary
    public void stringString(StringBuilder stringBuilder, LamaString string) {
        stringBuilder.append('\"')
                .append(toJavaStringNode.execute(string.getString()))
                .append('\"');
    }

    @Specialization
    @TruffleBoundary
    public void stringSExpression(StringBuilder stringBuilder, LamaSExpression sExpression) {
        stringBuilder.append(toJavaStringNode.execute(sExpression.getTag()));
        if (sExpression.getExpressions().isEmpty()) {
            return;
        }
        stringBuilder.append(" (");
        boolean isFirst = true;
        for (Object expression : sExpression.getExpressions()) {
            if (!isFirst) {
                stringBuilder.append(", ");
            }
            isFirst = false;
            execute(stringBuilder, expression);
        }
        stringBuilder.append(')');
    }

    @Specialization
    @TruffleBoundary
    public void stringArray(StringBuilder stringBuilder, LamaArray array) {
        stringBuilder.append('[');
        boolean isFirst = true;
        for (Object element : array.getElements()) {
            if (!isFirst) {
                stringBuilder.append(", ");
            }
            isFirst = false;
            execute(stringBuilder, element);
        }
        stringBuilder.append(']');
    }

    @SuppressWarnings("unused")
    @Specialization
    @TruffleBoundary
    public void stringFunction(StringBuilder stringBuilder, LamaFunction function) {
        stringBuilder.append("<function>");
    }

    @TruffleBoundary
    public static String stringify(Object value) {
        return LamaToStringNodeGen.create().createString(value);
    }
}
