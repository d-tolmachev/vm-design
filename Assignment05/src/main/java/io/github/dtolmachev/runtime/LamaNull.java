package io.github.dtolmachev.runtime;

import com.oracle.truffle.api.interop.InteropLibrary;
import com.oracle.truffle.api.interop.TruffleObject;
import com.oracle.truffle.api.library.ExportLibrary;
import com.oracle.truffle.api.library.ExportMessage;

@ExportLibrary(InteropLibrary.class)
public final class LamaNull implements TruffleObject {
    public static final LamaNull SINGLETON = new LamaNull();

    private LamaNull() {
    }

    @ExportMessage
    public boolean isNull() {
        return true;
    }
}
