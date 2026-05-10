package io.github.dtolmachev.runtime;

import com.oracle.truffle.api.CompilerDirectives.CompilationFinal;
import com.oracle.truffle.api.TruffleLanguage;
import com.oracle.truffle.api.TruffleLanguage.Env;
import com.oracle.truffle.api.dsl.NodeFactory;
import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.nodes.Node;
import com.oracle.truffle.api.strings.TruffleString;

import io.github.dtolmachev.LamaLanguage;
import io.github.dtolmachev.builtins.LamaBuiltinNode;
import io.github.dtolmachev.builtins.LamaEvalBuiltin;
import io.github.dtolmachev.builtins.LamaLengthBuiltinFactory;
import io.github.dtolmachev.builtins.LamaPrintlnBuiltinFactory;
import io.github.dtolmachev.builtins.LamaReadBuiltinFactory;
import io.github.dtolmachev.builtins.LamaStringBuiltinFactory;
import io.github.dtolmachev.builtins.LamaWriteBuiltinFactory;
import io.github.dtolmachev.nodes.LamaAstRootNode;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.util.HashMap;
import java.util.Map;

public final class LamaContext {
    private final LamaLanguage language;
    @CompilationFinal private Env env;
    private final BufferedReader input;
    private final PrintWriter output;
    private final Map<TruffleString, Object> functionRegistry;

    public LamaContext(LamaLanguage language, Env env) {
        this.env = env;
        this.input = new BufferedReader(new InputStreamReader(env.in()));
        this.output = new PrintWriter(env.out(), true);
        this.language = language;
        this.functionRegistry = new HashMap<>();
        installBuiltins();
    }

    public BufferedReader getInput() {
        return input;
    }

    public PrintWriter getOutput() {
        return output;
    }

    public Map<TruffleString, Object> getFunctionRegistry() {
        return functionRegistry;
    }

    private void installBuiltins() {
        installBuiltin("read", LamaReadBuiltinFactory.getInstance());
        installBuiltin("write", LamaWriteBuiltinFactory.getInstance());
        installBuiltin("length", LamaLengthBuiltinFactory.getInstance());
        installBuiltin("string", LamaStringBuiltinFactory.getInstance());
        installBuiltin("println", LamaPrintlnBuiltinFactory.getInstance());
    }

    private void installBuiltin(String name, NodeFactory<? extends LamaBuiltinNode> factory) {
        TruffleString encodedName = TruffleString.fromJavaStringUncached(name, LamaLanguage.STRING_ENCODING);
        int argumentsCount = factory.getExecutionSignature().size();
        LamaEvalBuiltin node = new LamaEvalBuiltin(factory.createNode());
        LamaAstRootNode rootNode = new LamaAstRootNode(language, FrameDescriptor.newBuilder().build(), node, null, encodedName);
        functionRegistry.put(encodedName, new LamaFunction(rootNode.getCallTarget(), argumentsCount, null));
    }

    private static final TruffleLanguage.ContextReference<LamaContext> reference = TruffleLanguage.ContextReference.create(LamaLanguage.class);

    public static LamaContext get(Node node) {
        return reference.get(node);
    }
}
