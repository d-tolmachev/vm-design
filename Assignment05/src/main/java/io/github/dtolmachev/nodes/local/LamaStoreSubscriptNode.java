package io.github.dtolmachev.nodes.local;

import com.oracle.truffle.api.CompilerDirectives.TruffleBoundary;
import com.oracle.truffle.api.dsl.Cached;
import com.oracle.truffle.api.dsl.Fallback;
import com.oracle.truffle.api.dsl.NodeChild;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.strings.MutableTruffleString.WriteByteNode;

import io.github.dtolmachev.LamaException;
import io.github.dtolmachev.LamaLanguage;
import io.github.dtolmachev.nodes.LamaExpressionNode;
import io.github.dtolmachev.nodes.util.LamaValidateIndexNode;
import io.github.dtolmachev.runtime.LamaArray;
import io.github.dtolmachev.runtime.LamaSExpression;
import io.github.dtolmachev.runtime.LamaString;

@SuppressWarnings("unused")
@NodeChild("baseNode")
@NodeChild("indexNode")
@NodeChild("valueNode")
public abstract class LamaStoreSubscriptNode extends LamaExpressionNode {
    @Child private LamaValidateIndexNode validateIndexNode;

    public LamaStoreSubscriptNode() {
        this.validateIndexNode = new LamaValidateIndexNode();
    }

    @Specialization
    public long storeString(VirtualFrame frame, LamaString string, long index, long chr, @Cached("create()") WriteByteNode writeByteNode) {
        validateIndexNode.execute(this, string.getString().byteLength(LamaLanguage.STRING_ENCODING), Math.toIntExact(index));
        if (chr < 0 || chr > 255) {
            throw LamaException.illegalChar(getValueNode(), Math.toIntExact(chr));
        }
        writeByteNode.execute(string.getString(), Math.toIntExact(index), (byte) chr, LamaLanguage.STRING_ENCODING);
        return chr;
    }

    @Specialization
    public Object typeErrorStringValue(LamaString string, long index, Object value) {
        throw LamaException.typeError(getValueNode(), "integer", value);
    }

    @Specialization
    public Object typeErrorStringIndex(LamaString string, Object index, Object value) {
        throw LamaException.typeError(getIndexNode(), "integer", index);
    }

    @Specialization
    public Object storeSExpression(VirtualFrame frame, LamaSExpression sExpression, long index, Object value) {
        validateIndexNode.execute(this, sExpression.getExpressions().size(), Math.toIntExact(index));
        sExpression.getExpressions().set(Math.toIntExact(index), value);
        return value;
    }

    @Specialization
    public Object typeErrorSExpressionIndex(LamaSExpression sExpression, Object index, Object value) {
        throw LamaException.typeError(getIndexNode(), "integer", index);
    }

    @Specialization
    public Object storeArray(VirtualFrame frame, LamaArray array, long index, Object value) {
        validateIndexNode.execute(this, array.getElements().size(), Math.toIntExact(index));
        array.getElements().set(Math.toIntExact(index), value);
        return value;
    }

    @Specialization
    public Object typeErrorArrayIndex(LamaArray array, Object index, Object value) {
        throw LamaException.typeError(getIndexNode(), "integer", index);
    }

    @Fallback
    @TruffleBoundary
    public Object typeError(Object aggregate, Object index, Object value) {
        throw LamaException.typeError(getBaseNode(), "aggregate", aggregate);
    }

    protected abstract LamaExpressionNode getBaseNode();

    protected abstract LamaExpressionNode getIndexNode();

    protected abstract LamaExpressionNode getValueNode();
}
