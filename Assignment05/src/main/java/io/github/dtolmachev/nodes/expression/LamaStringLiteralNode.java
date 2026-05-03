package io.github.dtolmachev.nodes.expression;

import com.oracle.truffle.api.dsl.Cached;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;
import com.oracle.truffle.api.strings.MutableTruffleString.AsMutableTruffleStringNode;
import com.oracle.truffle.api.strings.TruffleString;

import io.github.dtolmachev.LamaLanguage;
import io.github.dtolmachev.nodes.LamaExpressionNode;
import io.github.dtolmachev.runtime.LamaString;

@NodeInfo(shortName = "string-literal")
public abstract class LamaStringLiteralNode extends LamaExpressionNode {
    private final TruffleString value;

    public LamaStringLiteralNode(TruffleString value) {
        this.value = value;
    }

    @SuppressWarnings("unused")
    @Specialization
    public LamaString createString(VirtualFrame frame, @Cached("create()") AsMutableTruffleStringNode asMutableTruffleStringNode) {
        return new LamaString(asMutableTruffleStringNode.execute(value, LamaLanguage.STRING_ENCODING));
    }
}
