package io.github.dtolmachev.runtime;

import com.oracle.truffle.api.interop.InteropLibrary;
import com.oracle.truffle.api.interop.TruffleObject;
import com.oracle.truffle.api.library.ExportLibrary;
import com.oracle.truffle.api.library.ExportMessage;
import com.oracle.truffle.api.strings.MutableTruffleString;
import com.oracle.truffle.api.strings.TruffleString;

import io.github.dtolmachev.LamaLanguage;

@SuppressWarnings("ClassCanBeRecord")
@ExportLibrary(InteropLibrary.class)
public final class LamaString implements TruffleObject {
    private final MutableTruffleString string;

    public LamaString(MutableTruffleString string) {
        this.string = string;
    }

    public MutableTruffleString getString() {
        return string;
    }

    @ExportMessage
    public boolean isString() {
        return true;
    }

    @ExportMessage
    public String asString() {
        return string.toJavaStringUncached();
    }

    @ExportMessage
    public TruffleString asTruffleString() {
        return string.asTruffleStringUncached(LamaLanguage.STRING_ENCODING);
    }
}
