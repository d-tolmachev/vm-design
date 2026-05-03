package io.github.dtolmachev;

import com.oracle.truffle.api.CallTarget;
import com.oracle.truffle.api.TruffleLanguage;
import com.oracle.truffle.api.nodes.NodeInfo;
import com.oracle.truffle.api.source.Source;
import com.oracle.truffle.api.strings.TruffleString;

import io.github.dtolmachev.nodes.LamaAstRootNode;
import io.github.dtolmachev.parser.LamaNodeParser;
import io.github.dtolmachev.runtime.LamaContext;

import java.util.Objects;

@TruffleLanguage.Registration(id = LamaLanguage.ID, name = "Lama", defaultMimeType = LamaLanguage.MIME_TYPE, characterMimeTypes = LamaLanguage.MIME_TYPE, fileTypeDetectors = LamaFileDetector.class)
public final class LamaLanguage extends TruffleLanguage<LamaContext> {
    public static final String ID = "lama";
    public static final String MIME_TYPE = "application/x-lama";
    public static final TruffleString.Encoding STRING_ENCODING = TruffleString.Encoding.UTF_8;

    @Override
    protected LamaContext createContext(Env env) {
        return new LamaContext(this, env);
    }

    @SuppressWarnings("RedundantThrows")
    @Override
    protected CallTarget parse(ParsingRequest request) throws Exception {
        Source source = request.getSource();
        LamaAstRootNode rootNode = LamaNodeParser.parseLama(this, source);
        return rootNode.getCallTarget();
    }

    public static NodeInfo lookupNodeInfo(Class<?> clazz) {
        if (Objects.isNull(clazz)) {
            return null;
        }
        NodeInfo info = clazz.getAnnotation(NodeInfo.class);
        if (Objects.nonNull(info)) {
            return info;
        } else {
            return lookupNodeInfo(clazz.getSuperclass());
        }
    }
}
