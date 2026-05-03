package io.github.dtolmachev.runtime;

import com.oracle.truffle.api.interop.InteropLibrary;
import com.oracle.truffle.api.interop.InvalidArrayIndexException;
import com.oracle.truffle.api.interop.TruffleObject;
import com.oracle.truffle.api.library.ExportLibrary;
import com.oracle.truffle.api.library.ExportMessage;

import java.util.List;

@SuppressWarnings("ClassCanBeRecord")
@ExportLibrary(InteropLibrary.class)
public final class LamaArray implements TruffleObject {
    private final List<Object> elements;

    public LamaArray(List<Object> elements) {
        this.elements = elements;
    }

    public List<Object> getElements() {
        return elements;
    }

    @ExportMessage
    public boolean hasArrayElements() {
        return true;
    }

    @ExportMessage
    public int getArraySize() {
        return elements.size();
    }

    @ExportMessage
    public boolean isArrayElementReadable(long index) {
        return index >= 0 && index < elements.size();
    }

    @ExportMessage
    public boolean isArrayElementModifiable(long index) {
        return index >= 0 && index < elements.size();
    }

    @SuppressWarnings("unused")
    @ExportMessage
    public boolean isArrayElementInsertable(long index) {
        return false;
    }

    @ExportMessage
    public Object readArrayElement(long index) throws InvalidArrayIndexException {
        if (index < 0 || index >= elements.size()) {
            throw InvalidArrayIndexException.create(Math.toIntExact(index));
        }
        return elements.get(Math.toIntExact(index));
    }

    @ExportMessage
    public void writeArrayElement(long index, Object value) throws InvalidArrayIndexException {
        if (index < 0 || index >= elements.size()) {
            throw InvalidArrayIndexException.create(Math.toIntExact(index));
        }
        elements.set(Math.toIntExact(index), value);
    }
}
