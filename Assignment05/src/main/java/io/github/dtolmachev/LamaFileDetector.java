package io.github.dtolmachev;

import com.oracle.truffle.api.TruffleFile;

import java.io.IOException;
import java.nio.charset.Charset;
import java.util.Objects;

public final class LamaFileDetector implements TruffleFile.FileTypeDetector {
    @SuppressWarnings("RedundantThrows")
    @Override
    public String findMimeType(TruffleFile file) throws IOException {
        String name = file.getName();
        if (Objects.nonNull(name) && name.endsWith(".lama")) {
            return LamaLanguage.MIME_TYPE;
        }
        return null;
    }

    @Override
    public Charset findEncoding(TruffleFile file) {
        return null;
    }
}
