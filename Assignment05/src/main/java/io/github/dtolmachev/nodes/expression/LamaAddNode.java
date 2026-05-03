package io.github.dtolmachev.nodes.expression;

import com.oracle.truffle.api.dsl.Fallback;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.nodes.NodeInfo;

import io.github.dtolmachev.LamaException;
import io.github.dtolmachev.nodes.LamaBinaryNode;

@NodeInfo(shortName = "+")
public abstract class LamaAddNode extends LamaBinaryNode {
    @Specialization
    public long evaluate(long left, long right) {
        return left + right;
    }

    @Fallback
    public Object typeError(Object left, Object right) {
        throw LamaException.typeErrorOperator(this, left, right);
    }
}
