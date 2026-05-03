package io.github.dtolmachev.builtins;

import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.strings.MutableTruffleString.AsMutableTruffleStringNode;
import com.oracle.truffle.api.strings.TruffleString.FromJavaStringNode;

import io.github.dtolmachev.LamaLanguage;
import io.github.dtolmachev.nodes.util.LamaToStringNode;
import io.github.dtolmachev.nodes.util.LamaToStringNodeGen;
import io.github.dtolmachev.runtime.LamaString;

public abstract class LamaStringBuiltin extends LamaBuiltinNode {
    @Child private AsMutableTruffleStringNode asMutableTruffleStringNode;
    @Child private FromJavaStringNode fromJavaStringNode;
    @Child private LamaToStringNode lamaToStringNode;

    public LamaStringBuiltin() {
        this.asMutableTruffleStringNode = AsMutableTruffleStringNode.create();
        this.fromJavaStringNode = FromJavaStringNode.create();
        this.lamaToStringNode = LamaToStringNodeGen.create();
    }

    @Specialization
    public LamaString stringify(Object value) {
        return fromJavaString(lamaToStringNode.createString(value));
    }

    private LamaString fromJavaString(String string) {
        return new LamaString(asMutableTruffleStringNode.execute(fromJavaStringNode.execute(string, LamaLanguage.STRING_ENCODING), LamaLanguage.STRING_ENCODING));
    }
}
