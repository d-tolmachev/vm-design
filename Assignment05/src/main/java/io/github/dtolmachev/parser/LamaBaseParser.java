package io.github.dtolmachev.parser;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.frame.FrameSlotKind;
import com.oracle.truffle.api.source.Source;
import com.oracle.truffle.api.strings.TruffleString;
import org.antlr.v4.runtime.BaseErrorListener;
import org.antlr.v4.runtime.CharStreams;
import org.antlr.v4.runtime.CommonTokenStream;
import org.antlr.v4.runtime.RecognitionException;
import org.antlr.v4.runtime.Recognizer;
import org.antlr.v4.runtime.Token;

import io.github.dtolmachev.LamaLanguage;
import io.github.dtolmachev.parser.LamaLexer;
import io.github.dtolmachev.parser.LamaParser;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Optional;
import java.util.OptionalInt;
import java.util.function.Supplier;

public abstract class   LamaBaseParser extends io.github.dtolmachev.parser.LamaBaseVisitor<Void> {
    protected static void parseLamaImpl(Source source, LamaBaseParser visitor) {
        LamaLexer lexer = new LamaLexer(CharStreams.fromString(source.getCharacters().toString()));
        LamaParser parser = new LamaParser(new CommonTokenStream(lexer));
        lexer.removeErrorListeners();
        parser.removeErrorListeners();
        BailoutErrorListener listener = new BailoutErrorListener(source);
        lexer.addErrorListener(listener);
        parser.addErrorListener(listener);
        parser.compilationUnit().accept(visitor);
    }

    protected final LamaLanguage language;
    protected final Source source;

    protected LamaBaseParser(LamaLanguage language, Source source) {
        this.language = language;
        this.source = source;
        this.currentScope = new FunctionScope(FrameDescriptor.newBuilder());
        this.rootScope = currentScope;
    }

    protected void semErr(Token token, String message) {
        assert(Objects.nonNull(token));
        throwParseError(source, token.getLine(), token.getCharPositionInLine(), token, message);
    }

    private static final class BailoutErrorListener extends BaseErrorListener {
        private final Source source;

        BailoutErrorListener(Source source) {
            this.source = source;
        }

        @Override
        public void syntaxError(Recognizer<?, ?> recognizer, Object offendingSymbol, int line, int charPositionInLine, String msg, RecognitionException e) {
            throwParseError(source, line, charPositionInLine, (Token) offendingSymbol, msg);
        }
    }

    private static void throwParseError(Source source, int line, int charPositionInLine, Token token, String message) {
        int col = charPositionInLine + 1;
        int length = Objects.isNull(token) ? 1 : Math.max(token.getStopIndex() - token.getStartIndex(), 0);
        throw new LamaParseError(source, line, col, length, "Error(s) parsing script:%n-- line %d col %d: %s".formatted(line, col, message));
    }

    protected TruffleString asTruffleString(Token literalToken) {
        return TruffleString.fromJavaStringUncached(literalToken.getText(), LamaLanguage.STRING_ENCODING);
    }

    protected interface Variable {
        interface Visitor<R> {
            R visitGlobal(Global global);

            R visitLocal(Local local);

            R visitCapture(Capture capture);
        }

        default Global toGlobal() {
            return (Global) this;
        }

        default Local toLocal() {
            return (Local) this;
        }

        default Capture toCapture() {
            return (Capture) this;
        }

        <R> R accept(Visitor<R> visitor);

        default boolean isGlobal() {
            return this instanceof Global;
        }

        default boolean isLocal() {
            return this instanceof Local;
        }

        default boolean isCapture() {
            return this instanceof Capture;
        }
    }

    public record Global(TruffleString name) implements Variable {
        @Override
        public <R> R accept(Visitor<R> visitor) {
            return visitor.visitGlobal(this);
        }
    }

    public record Local(int slot) implements Variable {
        @Override
        public <R> R accept(Visitor<R> visitor) {
            return visitor.visitLocal(this);
        }
    }

    public record Capture(int level, int slot) implements Variable {
        @Override
        public <R> R accept(Visitor<R> visitor) {
            return visitor.visitCapture(this);
        }
    }

    private static class CaptureVisitor implements Variable.Visitor<Capture> {
        @Override
        public Capture visitGlobal(Global global) {
            throw new IllegalArgumentException("Cannot capture globals");
        }

        @Override
        public Capture visitLocal(Local local) {
            return new Capture(0, local.slot());
        }

        @Override
        public Capture visitCapture(Capture capture) {
            return new Capture(capture.level() + 1, capture.slot());
        }
    }

    protected class Scope {
        protected final Scope parent;
        protected FunctionScope function;
        protected final Map<TruffleString, Variable> bindings;

        public Scope(Scope parent) {
            this.parent = parent;
            this.function = Objects.nonNull(parent) ? parent.function : null;
            bindings = new HashMap<>();
        }

        public int declareLocal(Token token, TruffleString name) {
            int slot = function.frameDescriptorBuilder.addSlot(FrameSlotKind.Illegal, name, null);
            Variable prev = bindings.put(name, new Local(slot));
            if (Objects.nonNull(prev)) {
                semErr(token, "`%s` already declared".formatted(name));
            }
            return slot;
        }

        public boolean isDeclared(TruffleString name) {
            return bindings.containsKey(name);
        }

        public Optional<Variable> get(TruffleString name) {
            return Optional.ofNullable(bindings.get(name));
        }

        public OptionalInt getLocal(TruffleString name) {
            return get(name)
                    .map(variable -> OptionalInt.of(variable.toLocal().slot()))
                    .orElseGet(OptionalInt::empty);
        }
    }

    protected class GlobalScope extends Scope {
        public GlobalScope() {
            super(null);
            declareGlobal(TruffleString.fromJavaStringUncached("read", LamaLanguage.STRING_ENCODING));
            declareGlobal(TruffleString.fromJavaStringUncached("write", LamaLanguage.STRING_ENCODING));
            declareGlobal(TruffleString.fromJavaStringUncached("length", LamaLanguage.STRING_ENCODING));
            declareGlobal(TruffleString.fromJavaStringUncached("string", LamaLanguage.STRING_ENCODING));
            declareGlobal(TruffleString.fromJavaStringUncached("println", LamaLanguage.STRING_ENCODING));
        }

        private void declareGlobal(TruffleString name) {
            bindings.put(name, new Global(name));
        }
    }

    protected class FunctionScope extends Scope {
        protected final FrameDescriptor.Builder frameDescriptorBuilder;
        protected final List<Variable> captures;

        public FunctionScope(FrameDescriptor.Builder frameDescriptorBuilder) {
            this(new GlobalScope(), frameDescriptorBuilder);
        }

        public FunctionScope(Scope parent, FrameDescriptor.Builder frameDescriptorBuilder) {
            super(parent);
            this.function = this;
            this.frameDescriptorBuilder = frameDescriptorBuilder;
            this.captures = new ArrayList<>();
        }

        public Capture declareCapture(TruffleString name, Variable outer) {
            captures.add(outer);
            Capture capture =outer.accept(new CaptureVisitor());
            bindings.put(name, capture);
            return capture;
        }
    }

    protected Scope currentScope;
    protected Scope rootScope;

    protected <R> R newScope(Supplier<R> callback) {
        return inScope(new Scope(currentScope), callback);
    }

    protected <R> R inScope(Scope newScope, Supplier<R> callback) {
        Scope previousScope = currentScope;
        currentScope = newScope;
        try {
            return  callback.get();
        } finally {
            currentScope = previousScope;
        }
    }
}
