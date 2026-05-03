package io.github.dtolmachev.nodes.local;

import com.oracle.truffle.api.dsl.Cached;
import com.oracle.truffle.api.dsl.NodeChild;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;
import com.oracle.truffle.api.strings.TruffleString.ReadByteNode;

import io.github.dtolmachev.LamaLanguage;
import io.github.dtolmachev.nodes.LamaExpressionNode;
import io.github.dtolmachev.nodes.util.LamaValidateIndexNode;
import io.github.dtolmachev.runtime.LamaArray;
import io.github.dtolmachev.runtime.LamaSExpression;
import io.github.dtolmachev.runtime.LamaString;

@SuppressWarnings("unused")
@NodeInfo(shortName = "[]")
@NodeChild("baseNode")
@NodeChild("indexNode")
public abstract class LamaLoadSubscriptNode extends LamaExpressionNode {
    @Child private LamaValidateIndexNode validateIndexNode;

    public LamaLoadSubscriptNode() {
        this.validateIndexNode = new LamaValidateIndexNode();
    }

    @Specialization
    public long loadString(VirtualFrame frame, LamaString string, long index, @Cached("create()") ReadByteNode readByteNode) {
        validateIndexNode.execute(this, string.getString().byteLength(LamaLanguage.STRING_ENCODING), Math.toIntExact(index));
        return readByteNode.execute(string.getString(), Math.toIntExact(index), LamaLanguage.STRING_ENCODING);
    }

    @Specialization
    public Object loadSExpression(VirtualFrame frame, LamaSExpression sExpression, long index) {
        validateIndexNode.execute(this, sExpression.getExpressions().size(), Math.toIntExact(index));
        return sExpression.getExpressions().get(Math.toIntExact(index));
    }

    @Specialization
    public Object loadArray(VirtualFrame frame, LamaArray array, long index) {
        validateIndexNode.execute(this, array.getElements().size(), Math.toIntExact(index));
        return array.getElements().get(Math.toIntExact(index));
    }

    protected abstract LamaExpressionNode getBaseNode();

    protected abstract LamaExpressionNode getIndexNode();
}
