package io.github.dtolmachev.nodes.pattern;

import com.oracle.truffle.api.dsl.Cached;
import com.oracle.truffle.api.dsl.Fallback;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.strings.TruffleString;
import com.oracle.truffle.api.strings.TruffleString.EqualNode;

import io.github.dtolmachev.LamaLanguage;
import io.github.dtolmachev.runtime.LamaString;

public abstract class LamaStringLiteralPatternNode extends LamaPatternNode {
    private final TruffleString value;

    public LamaStringLiteralPatternNode(TruffleString value) {
        this.value = value;
    }

    @SuppressWarnings("unused")
    @Specialization
    public boolean checkString(VirtualFrame frame, LamaString scrutinee, @Cached("create()") EqualNode equalNode) {
        return equalNode.execute(scrutinee.getString(), value, LamaLanguage.STRING_ENCODING);
    }

    @SuppressWarnings("unused")
    @Fallback
    public boolean checkFalse(VirtualFrame frame, Object scrutinee) {
        return false;
    }
}
