package io.github.dtolmachev.parser;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.source.Source;
import com.oracle.truffle.api.source.SourceSection;
import com.oracle.truffle.api.strings.TruffleString;
import org.antlr.v4.runtime.Token;
import org.antlr.v4.runtime.tree.ParseTree;

import io.github.dtolmachev.LamaLanguage;
import io.github.dtolmachev.nodes.LamaAstRootNode;
import io.github.dtolmachev.nodes.LamaBinaryNode;
import io.github.dtolmachev.nodes.LamaExpressionNode;
import io.github.dtolmachev.nodes.LamaNullNode;
import io.github.dtolmachev.nodes.LamaSequenceNode;
import io.github.dtolmachev.nodes.argument.LamaLoadArgumentNode;
import io.github.dtolmachev.nodes.capture.LamaLoadCaptureNodeGen;
import io.github.dtolmachev.nodes.capture.LamaStoreCaptureNode;
import io.github.dtolmachev.nodes.controlflow.LamaCaseNode;
import io.github.dtolmachev.nodes.controlflow.LamaDoWhileNode;
import io.github.dtolmachev.nodes.controlflow.LamaForNode;
import io.github.dtolmachev.nodes.controlflow.LamaIfNode;
import io.github.dtolmachev.nodes.controlflow.LamaLetNode;
import io.github.dtolmachev.nodes.controlflow.LamaWhileDoNode;
import io.github.dtolmachev.nodes.expression.LamaAddNodeGen;
import io.github.dtolmachev.nodes.expression.LamaArrayLiteralNode;
import io.github.dtolmachev.nodes.expression.LamaCallNode;
import io.github.dtolmachev.nodes.expression.LamaDivNodeGen;
import io.github.dtolmachev.nodes.expression.LamaEqualNode;
import io.github.dtolmachev.nodes.expression.LamaFunctionNode;
import io.github.dtolmachev.nodes.expression.LamaGreaterNode;
import io.github.dtolmachev.nodes.expression.LamaGreaterOrEqualNode;
import io.github.dtolmachev.nodes.expression.LamaIntegerLiteralNode;
import io.github.dtolmachev.nodes.expression.LamaLessNode;
import io.github.dtolmachev.nodes.expression.LamaLessOrEqualNode;
import io.github.dtolmachev.nodes.expression.LamaLogicalAndNode;
import io.github.dtolmachev.nodes.expression.LamaLogicalOrNode;
import io.github.dtolmachev.nodes.expression.LamaModNodeGen;
import io.github.dtolmachev.nodes.expression.LamaMulNodeGen;
import io.github.dtolmachev.nodes.expression.LamaNegNode;
import io.github.dtolmachev.nodes.expression.LamaNotEqualNode;
import io.github.dtolmachev.nodes.expression.LamaSExpressionLiteralNode;
import io.github.dtolmachev.nodes.expression.LamaStringLiteralNode;
import io.github.dtolmachev.nodes.expression.LamaStringLiteralNodeGen;
import io.github.dtolmachev.nodes.expression.LamaSubNodeGen;
import io.github.dtolmachev.nodes.global.LamaLoadGlobalNodeGen;
import io.github.dtolmachev.nodes.local.LamaLoadLocalNode;
import io.github.dtolmachev.nodes.local.LamaLoadSubscriptNode;
import io.github.dtolmachev.nodes.local.LamaLoadSubscriptNodeGen;
import io.github.dtolmachev.nodes.local.LamaStoreLocalNode;
import io.github.dtolmachev.nodes.local.LamaStoreLocalNodeGen;
import io.github.dtolmachev.nodes.local.LamaStoreSubscriptNode;
import io.github.dtolmachev.nodes.local.LamaStoreSubscriptNodeGen;
import io.github.dtolmachev.nodes.pattern.LamaArrayLiteralPatternNodeGen;
import io.github.dtolmachev.nodes.pattern.LamaIntegerLiteralPatternNodeGen;
import io.github.dtolmachev.nodes.pattern.LamaIsArrayPatternNodeGen;
import io.github.dtolmachev.nodes.pattern.LamaIsBoxPatternNodeGen;
import io.github.dtolmachev.nodes.pattern.LamaIsFunctionPatternNodeGen;
import io.github.dtolmachev.nodes.pattern.LamaIsSExpressionPatternNodeGen;
import io.github.dtolmachev.nodes.pattern.LamaIsStringPatternNodeGen;
import io.github.dtolmachev.nodes.pattern.LamaIsValPatternNodeGen;
import io.github.dtolmachev.nodes.pattern.LamaNameReferencePatternNodeGen;
import io.github.dtolmachev.nodes.pattern.LamaNullPatternNodeGen;
import io.github.dtolmachev.nodes.pattern.LamaPatternNode;
import io.github.dtolmachev.nodes.pattern.LamaSExpressionLiteralPatternNodeGen;
import io.github.dtolmachev.nodes.pattern.LamaStringLiteralPatternNodeGen;
import io.github.dtolmachev.nodes.pattern.LamaWildcardPatternNode;
import io.github.dtolmachev.parser.LamaParser.AdditiveExpressionContext;
import io.github.dtolmachev.parser.LamaParser.ArrayExpressionContext;
import io.github.dtolmachev.parser.LamaParser.ArrayLiteralContext;
import io.github.dtolmachev.parser.LamaParser.ArrayPatternContext;
import io.github.dtolmachev.parser.LamaParser.AssignmentExpressionContext;
import io.github.dtolmachev.parser.LamaParser.BasicExpressionContext;
import io.github.dtolmachev.parser.LamaParser.BinaryExpressionContext;
import io.github.dtolmachev.parser.LamaParser.CallExpressionContext;
import io.github.dtolmachev.parser.LamaParser.CaseBranchesContext;
import io.github.dtolmachev.parser.LamaParser.CaseExpressionContext;
import io.github.dtolmachev.parser.LamaParser.CasePrimaryContext;
import io.github.dtolmachev.parser.LamaParser.CharLiteralContext;
import io.github.dtolmachev.parser.LamaParser.CharLiteralPatternContext;
import io.github.dtolmachev.parser.LamaParser.ComparisonExpressionContext;
import io.github.dtolmachev.parser.LamaParser.CompilationUnitContext;
import io.github.dtolmachev.parser.LamaParser.ConjunctionExpressionContext;
import io.github.dtolmachev.parser.LamaParser.ConsPatternContext;
import io.github.dtolmachev.parser.LamaParser.DefinitionContext;
import io.github.dtolmachev.parser.LamaParser.DisjunctionExpressionContext;
import io.github.dtolmachev.parser.LamaParser.DoWhileExpressionContext;
import io.github.dtolmachev.parser.LamaParser.DoWhilePrimaryContext;
import io.github.dtolmachev.parser.LamaParser.ElifExpressionContext;
import io.github.dtolmachev.parser.LamaParser.ElseExpressionContext;
import io.github.dtolmachev.parser.LamaParser.ExpressionContext;
import io.github.dtolmachev.parser.LamaParser.FalseLiteralContext;
import io.github.dtolmachev.parser.LamaParser.FalseLiteralPatternContext;
import io.github.dtolmachev.parser.LamaParser.ForExpressionContext;
import io.github.dtolmachev.parser.LamaParser.ForPrimaryContext;
import io.github.dtolmachev.parser.LamaParser.FunctionArgumentsContext;
import io.github.dtolmachev.parser.LamaParser.FunctionBodyContext;
import io.github.dtolmachev.parser.LamaParser.FunctionDefinitionContext;
import io.github.dtolmachev.parser.LamaParser.FunctionPrimaryContext;
import io.github.dtolmachev.parser.LamaParser.HashArrayPatternContext;
import io.github.dtolmachev.parser.LamaParser.HashBoxPatternContext;
import io.github.dtolmachev.parser.LamaParser.HashFunPatternContext;
import io.github.dtolmachev.parser.LamaParser.HashSexpPatternContext;
import io.github.dtolmachev.parser.LamaParser.HashStrPatternContext;
import io.github.dtolmachev.parser.LamaParser.HashValPatternContext;
import io.github.dtolmachev.parser.LamaParser.IfExpressionContext;
import io.github.dtolmachev.parser.LamaParser.IfPrimaryContext;
import io.github.dtolmachev.parser.LamaParser.ImportStatementContext;
import io.github.dtolmachev.parser.LamaParser.InfixAdditionContext;
import io.github.dtolmachev.parser.LamaParser.InfixEqualContext;
import io.github.dtolmachev.parser.LamaParser.InfixGreaterContext;
import io.github.dtolmachev.parser.LamaParser.InfixGreaterOrEqualContext;
import io.github.dtolmachev.parser.LamaParser.InfixLessContext;
import io.github.dtolmachev.parser.LamaParser.InfixLessOrEqualContext;
import io.github.dtolmachev.parser.LamaParser.InfixMultiplicationContext;
import io.github.dtolmachev.parser.LamaParser.InfixNotEqualContext;
import io.github.dtolmachev.parser.LamaParser.InfixQuotientContext;
import io.github.dtolmachev.parser.LamaParser.InfixRemainderContext;
import io.github.dtolmachev.parser.LamaParser.InfixSubtractionContext;
import io.github.dtolmachev.parser.LamaParser.IntegerLiteralContext;
import io.github.dtolmachev.parser.LamaParser.IntegerLiteralPatternContext;
import io.github.dtolmachev.parser.LamaParser.LetExpressionContext;
import io.github.dtolmachev.parser.LamaParser.LetPrimaryContext;
import io.github.dtolmachev.parser.LamaParser.ListConstructorExpressionContext;
import io.github.dtolmachev.parser.LamaParser.ListExpressionContext;
import io.github.dtolmachev.parser.LamaParser.ListLiteralContext;
import io.github.dtolmachev.parser.LamaParser.ListPatternContext;
import io.github.dtolmachev.parser.LamaParser.MethodCallExpressionContext;
import io.github.dtolmachev.parser.LamaParser.MultiplicativeExpressionContext;
import io.github.dtolmachev.parser.LamaParser.NameReferenceContext;
import io.github.dtolmachev.parser.LamaParser.NameReferencePatternContext;
import io.github.dtolmachev.parser.LamaParser.ParenthesizedExpressionContext;
import io.github.dtolmachev.parser.LamaParser.ParenthesizedPatternContext;
import io.github.dtolmachev.parser.LamaParser.PatternContext;
import io.github.dtolmachev.parser.LamaParser.PostfixExpressionContext;
import io.github.dtolmachev.parser.LamaParser.PrimaryContext;
import io.github.dtolmachev.parser.LamaParser.PrimaryExpressionContext;
import io.github.dtolmachev.parser.LamaParser.SExpressionContext;
import io.github.dtolmachev.parser.LamaParser.SExpressionLiteralContext;
import io.github.dtolmachev.parser.LamaParser.SExpressionPatternContext;
import io.github.dtolmachev.parser.LamaParser.ScopeExpressionContext;
import io.github.dtolmachev.parser.LamaParser.SimpleArrayPatternContext;
import io.github.dtolmachev.parser.LamaParser.SimpleListPatternContext;
import io.github.dtolmachev.parser.LamaParser.SimpleSExpressionPatternContext;
import io.github.dtolmachev.parser.LamaParser.SimpleWildcardPatternContext;
import io.github.dtolmachev.parser.LamaParser.SkipPrimaryContext;
import io.github.dtolmachev.parser.LamaParser.StringLiteralContext;
import io.github.dtolmachev.parser.LamaParser.StringLiteralPatternContext;
import io.github.dtolmachev.parser.LamaParser.SubscriptExpressionContext;
import io.github.dtolmachev.parser.LamaParser.TrueLiteralContext;
import io.github.dtolmachev.parser.LamaParser.TrueLiteralPatternContext;
import io.github.dtolmachev.parser.LamaParser.UnaryExpressionContext;
import io.github.dtolmachev.parser.LamaParser.VariableDefinitionContext;
import io.github.dtolmachev.parser.LamaParser.VariableDefinitionItemContext;
import io.github.dtolmachev.parser.LamaParser.VariableDefinitionSequenceContext;
import io.github.dtolmachev.parser.LamaParser.WhileDoExpressionContext;
import io.github.dtolmachev.parser.LamaParser.WhileDoPrimaryContext;
import io.github.dtolmachev.parser.LamaParser.WildcardPatternContext;

import java.util.ArrayList;
import java.util.IdentityHashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Optional;
import java.util.function.Function;

public class LamaNodeParser extends LamaBaseParser {
    public static LamaAstRootNode parseLama(LamaLanguage language, Source source) {
        LamaNodeParser visitor = new LamaNodeParser(language, source);
        parseLamaImpl(source, visitor);
        return visitor.main;
    }

    private LamaAstRootNode main;
    private final ExpressionVisitor expressionVisitor;
    private final PatternVisitor patternVisitor;
    private final Map<ScopeExpressionContext, Scope> scopeExpressions;

    protected LamaNodeParser(LamaLanguage language, Source source) {
        super(language, source);
        this.main = null;
        this.expressionVisitor = new ExpressionVisitor();
        this.patternVisitor = new PatternVisitor();
        this.scopeExpressions = new IdentityHashMap<>();
    }

    private static class LoadSymbolVisitor implements Variable.Visitor<LamaExpressionNode> {
        @Override
        public LamaExpressionNode visitGlobal(Global global) {
            return LamaLoadGlobalNodeGen.create(global.name());
        }

        @Override
        public LamaExpressionNode visitLocal(Local local) {
            return new LamaLoadLocalNode(local.slot());
        }

        @Override
        public LamaExpressionNode visitCapture(Capture capture) {
            return LamaLoadCaptureNodeGen.create(capture.level(), capture.slot());
        }
    }

    private class StoreSymbolVisitor implements Variable.Visitor<LamaExpressionNode> {
        private final LamaExpressionNode right;
        private final Token name;

        public StoreSymbolVisitor(LamaExpressionNode right, Token name) {
            this.right = right;
            this.name = name;
        }

        @Override
        public LamaExpressionNode visitGlobal(Global global) {
            semErr(name, "cannot assign to global");
            return null;
        }

        @Override
        public LamaExpressionNode visitLocal(Local local) {
            return LamaStoreLocalNodeGen.create(local.slot(), right);
        }

        @Override
        public LamaExpressionNode visitCapture(Capture capture) {
            return new LamaStoreCaptureNode(capture.level(), capture.slot(), right);
        }
    }

    @Override
    public Void visitCompilationUnit(CompilationUnitContext ctx) {
        ctx.importStatement().forEach(this::visit);
        LamaExpressionNode scopeExpression = visitExpression(ctx.scopeExpression());
        SourceSection section = createSection(ctx.start, ctx.stop);
        main = new LamaAstRootNode(language, currentScope.function.frameDescriptorBuilder.build(), scopeExpression, section, TruffleString.fromJavaStringUncached("main", LamaLanguage.STRING_ENCODING));
        return null;
    }

    private LamaExpressionNode visitExpression(ParseTree parseTree) {
        return expressionVisitor.visit(parseTree);
    }

    @Override
    public Void visitImportStatement(ImportStatementContext ctx) {
        semErr(ctx.KEYWORD_IMPORT().getSymbol(), "imports are not supported");
        return null;
    }

    private class ExpressionVisitor extends LamaBaseVisitor<LamaExpressionNode> {
        @Override
        public LamaExpressionNode visitScopeExpression(ScopeExpressionContext ctx) {
            return newScope(() -> visitScopeExpression(ctx, this::visitExpression));
        }

        private LamaExpressionNode visitScopeExpression(ScopeExpressionContext ctx, Function<ExpressionContext, LamaExpressionNode> visitor) {
            scopeExpressions.put(ctx, currentScope);
            for (DefinitionContext definition : ctx.definition()) {
                if (Objects.nonNull(definition.variableDefinition())) {
                    definition.variableDefinition().variableDefinitionSequence().variableDefinitionItem().forEach(variableDefinitionItem -> currentScope.declareLocal(definition.getStart(), asTruffleString(variableDefinitionItem.LIDENT().getSymbol())));
                } else if (Objects.nonNull(definition.functionDefinition())) {
                    currentScope.declareLocal(definition.getStart(), asTruffleString(definition.functionDefinition().LIDENT().getSymbol()));
                }
            }
            List<LamaExpressionNode> children = ctx.definition().stream()
                    .map(this::visit)
                    .filter(Objects::nonNull)
                    .collect(ArrayList::new, ArrayList::add, ArrayList::addAll);
            children.add(visitor.apply(ctx.expression()));
            return toLamaSequenceNode(children, ctx.start, ctx.stop);
        }

        @Override
        public LamaExpressionNode visitVariableDefinition(VariableDefinitionContext ctx) {
            if (Objects.nonNull(ctx.KEYWORD_PUBLIC()) && currentScope != rootScope) {
                semErr(ctx.KEYWORD_PUBLIC().getSymbol(), "public variables are only allowed at the top level");
            }
            return visit(ctx.variableDefinitionSequence());
        }

        @Override
        public LamaExpressionNode visitVariableDefinitionSequence(VariableDefinitionSequenceContext ctx) {
            List<LamaExpressionNode> result = ctx.variableDefinitionItem().stream()
                    .map(this::visit)
                    .filter(Objects::nonNull)
                    .collect(ArrayList::new, ArrayList::add, ArrayList::addAll);
            return toLamaSequenceNode(result, ctx.start, ctx.stop);
        }

        @SuppressWarnings("OptionalGetWithoutIsPresent")
        @Override
        public LamaExpressionNode visitVariableDefinitionItem(VariableDefinitionItemContext ctx) {
            if (Objects.isNull(ctx.basicExpression())) {
                return null;
            }
            int slot = currentScope.getLocal(asTruffleString(ctx.LIDENT().getSymbol())).getAsInt();
            LamaExpressionNode initializer = visit(ctx.basicExpression());
            LamaStoreLocalNode result = LamaStoreLocalNodeGen.create(slot, initializer);
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        @SuppressWarnings("OptionalGetWithoutIsPresent")
        @Override
        public LamaExpressionNode visitFunctionDefinition(FunctionDefinitionContext ctx) {
            if (Objects.nonNull(ctx.KEYWORD_PUBLIC()) && currentScope != rootScope) {
                semErr(ctx.KEYWORD_PUBLIC().getSymbol(), "public functions are only allowed at the top level");
            }
            int slot = currentScope.getLocal(asTruffleString(ctx.LIDENT().getSymbol())).getAsInt();
            FunctionScope functionScope = new FunctionScope(currentScope, FrameDescriptor.newBuilder());
            LamaFunctionNode function = inScope(functionScope, () -> visitFunctionDefinition(functionScope, createSection(ctx.start, ctx.stop), asTruffleString(ctx.LIDENT().getSymbol()), ctx.functionArguments(), ctx.functionBody()));
            LamaStoreLocalNode result = LamaStoreLocalNodeGen.create(slot, function);
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        private LamaFunctionNode visitFunctionDefinition(FunctionScope functionScope, SourceSection sourceSection, TruffleString name, FunctionArgumentsContext functionArgumentsContext, FunctionBodyContext functionBodyContext) {
            List<LamaPatternNode> functionArguments = functionArgumentsContext.pattern().stream()
                    .map(patternVisitor::visit)
                    .collect(ArrayList::new, ArrayList::add, ArrayList::addAll);
            LamaExpressionNode functionBody = newScope(() -> visit(functionBodyContext.scopeExpression()));
            for (int i = functionArgumentsContext.pattern().size() - 1; i >= 0; i--) {
                LamaExpressionNode scrutinee = new LamaLoadArgumentNode(i + 1);
                LamaPatternNode[] patterns = new LamaPatternNode[]{functionArguments.get(i)};
                LamaExpressionNode[] branches = new LamaExpressionNode[]{functionBody};
                functionBody = new LamaCaseNode(scrutinee, patterns, branches);
            }
            LamaAstRootNode rootNode = new LamaAstRootNode(language, functionScope.frameDescriptorBuilder.build(), functionBody, sourceSection, name);
            return new LamaFunctionNode(rootNode.getCallTarget(), functionArgumentsContext.pattern().size(), !functionScope.captures.isEmpty());
        }

        @Override
        public LamaExpressionNode visitExpression(ExpressionContext ctx) {
            return visitExpression(ctx, this::visitBasicExpression);
        }

        private LamaExpressionNode visitExpression(ExpressionContext ctx, Function<BasicExpressionContext, LamaExpressionNode> visitor) {
            List<LamaExpressionNode> expressions = ctx.basicExpression().stream()
                    .limit(ctx.basicExpression().size() - 1)
                    .map(this::visit)
                    .collect(ArrayList::new, ArrayList::add, ArrayList::addAll);
            expressions.add(visitor.apply(ctx.basicExpression().getLast()));
            return toLamaSequenceNode(expressions, ctx.start, ctx.stop);
        }

        @Override
        public LamaExpressionNode visitBasicExpression(BasicExpressionContext ctx) {
            return visit(ctx.binaryExpression());
        }

        @Override
        public LamaExpressionNode visitUnaryExpression(UnaryExpressionContext ctx) {
            LamaExpressionNode result = visit(ctx.postfixExpression());
            if (Objects.nonNull(ctx.INFIX_SUBTRACTION())) {
                result = new LamaNegNode(result);
            }
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        @Override
        public LamaExpressionNode visitMultiplicativeExpression(MultiplicativeExpressionContext ctx) {
            LamaExpressionNode left = visit(ctx.binaryExpression().getFirst());
            LamaExpressionNode right = visit(ctx.binaryExpression().getLast());
            LamaBinaryNode result = switch (ctx.infixMultiplicative()) {
                case InfixMultiplicationContext ignored -> LamaMulNodeGen.create(left, right);
                case InfixQuotientContext ignored -> LamaDivNodeGen.create(left, right);
                case InfixRemainderContext ignored -> LamaModNodeGen.create(left, right);
                default -> throw new IllegalArgumentException("Unknown operator: " + ctx.infixMultiplicative().getText());
            };
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        @Override
        public LamaExpressionNode visitAdditiveExpression(AdditiveExpressionContext ctx) {
            LamaExpressionNode left = visit(ctx.binaryExpression().getFirst());
            LamaExpressionNode right = visit(ctx.binaryExpression().getLast());
            LamaBinaryNode result = switch (ctx.infixAdditive()) {
                case InfixAdditionContext ignored -> LamaAddNodeGen.create(left, right);
                case InfixSubtractionContext ignored -> LamaSubNodeGen.create(left, right);
                default -> throw new IllegalArgumentException("Unknown operator: " + ctx.infixAdditive().getText());
            };
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        @Override
        public LamaExpressionNode visitComparisonExpression(ComparisonExpressionContext ctx) {
            LamaExpressionNode left = visit(ctx.binaryExpression().getFirst());
            LamaExpressionNode right = visit(ctx.binaryExpression().getLast());
            LamaExpressionNode result = switch (ctx.infixComparison()) {
                case InfixEqualContext ignored -> new LamaEqualNode(left, right);
                case InfixNotEqualContext ignored -> new LamaNotEqualNode(left, right);
                case InfixLessOrEqualContext ignored -> new LamaLessOrEqualNode(left, right);
                case InfixLessContext ignored -> new LamaLessNode(left, right);
                case InfixGreaterOrEqualContext ignored -> new LamaGreaterOrEqualNode(left, right);
                case InfixGreaterContext ignored -> new LamaGreaterNode(left, right);
                default -> throw new IllegalArgumentException("Unknown operator: " + ctx.infixComparison().getText());
            };
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        @Override
        public LamaExpressionNode visitConjunctionExpression(ConjunctionExpressionContext ctx) {
            LamaExpressionNode left = visit(ctx.binaryExpression().getFirst());
            LamaExpressionNode right = visit(ctx.binaryExpression().getLast());
            LamaLogicalAndNode result = new LamaLogicalAndNode(left, right);
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        @Override
        public LamaExpressionNode visitDisjunctionExpression(DisjunctionExpressionContext ctx) {
            LamaExpressionNode left = visit(ctx.binaryExpression().getFirst());
            LamaExpressionNode right = visit(ctx.binaryExpression().getLast());
            LamaLogicalOrNode result = new LamaLogicalOrNode(left, right);
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        @Override
        public LamaExpressionNode visitListConstructorExpression(ListConstructorExpressionContext ctx) {
            TruffleString tag = TruffleString.fromJavaStringUncached("cons", LamaLanguage.STRING_ENCODING);
            LamaExpressionNode[] expressions = new LamaExpressionNode[]{visit(ctx.binaryExpression().getFirst()), visit(ctx.binaryExpression().getLast())};
            LamaSExpressionLiteralNode result = new LamaSExpressionLiteralNode(tag, expressions);
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        @Override
        public LamaExpressionNode visitAssignmentExpression(AssignmentExpressionContext ctx) {
            return new AssignmentExpressionVisitor(currentScope, ctx, ctx.binaryExpression().getLast()).visitBinaryExpression(ctx.binaryExpression().getFirst());
        }

        @Override
        public LamaExpressionNode visitPrimaryExpression(PrimaryExpressionContext ctx) {
            return visit(ctx.primary());
        }

        @Override
        public LamaExpressionNode visitCallExpression(CallExpressionContext ctx) {
            LamaExpressionNode callee = visit(ctx.postfixExpression());
            LamaExpressionNode[] arguments = ctx.expression().stream()
                    .map(this::visit)
                    .toArray(LamaExpressionNode[]::new);
            LamaCallNode result = new LamaCallNode(callee, arguments);
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        @Override
        public LamaExpressionNode visitMethodCallExpression(MethodCallExpressionContext ctx) {
            LamaExpressionNode callee = visitNameReference(ctx.LIDENT().getSymbol());
            LamaExpressionNode[] arguments = new LamaExpressionNode[ctx.expression().size() + 1];
            arguments[0] = visit(ctx.postfixExpression());
            for (int i = 0; i < ctx.expression().size(); i++) {
                arguments[i + 1] = visit(ctx.expression().get(i));
            }
            LamaCallNode result = new LamaCallNode(callee, arguments);
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        @Override
        public LamaExpressionNode visitSubscriptExpression(SubscriptExpressionContext ctx) {
            LamaExpressionNode base = visit(ctx.postfixExpression());
            LamaExpressionNode index = visit(ctx.expression());
            LamaLoadSubscriptNode result = LamaLoadSubscriptNodeGen.create(base, index);
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        @Override
        public LamaExpressionNode visitIntegerLiteral(IntegerLiteralContext ctx) {
            LamaIntegerLiteralNode result = new LamaIntegerLiteralNode(parseIntegerLiteral(ctx.DECIMAL().getSymbol(), false));
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        @Override
        public LamaExpressionNode visitStringLiteral(StringLiteralContext ctx) {
            LamaStringLiteralNode result = LamaStringLiteralNodeGen.create(parseStringLiteral(ctx.STRING().getSymbol()));
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        @Override
        public LamaExpressionNode visitCharLiteral(CharLiteralContext ctx) {
            LamaIntegerLiteralNode result = new LamaIntegerLiteralNode(parseCharLiteral(ctx.CHAR().getSymbol()));
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        @Override
        public LamaExpressionNode visitNameReference(NameReferenceContext ctx) {
            LamaExpressionNode result = visitNameReference(ctx.LIDENT().getSymbol());
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        private LamaExpressionNode visitNameReference(Token name) {
            return resolve(name, asTruffleString(name)).accept(new LoadSymbolVisitor());
        }

        @Override
        public LamaExpressionNode visitTrueLiteral(TrueLiteralContext ctx) {
            LamaIntegerLiteralNode result = new LamaIntegerLiteralNode(1);
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        @Override
        public LamaExpressionNode visitFalseLiteral(FalseLiteralContext ctx) {
            LamaIntegerLiteralNode result = new LamaIntegerLiteralNode(0);
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        @Override
        public LamaExpressionNode visitFunctionPrimary(FunctionPrimaryContext ctx) {
            FunctionScope functionScope = new FunctionScope(currentScope, FrameDescriptor.newBuilder());
            LamaFunctionNode result = inScope(functionScope, () -> visitFunctionDefinition(functionScope, createSection(ctx.start, ctx.stop), TruffleString.fromJavaStringUncached("<lambda>", LamaLanguage.STRING_ENCODING), ctx.functionArguments(), ctx.functionBody()));
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        @Override
        public LamaExpressionNode visitSkipPrimary(SkipPrimaryContext ctx) {
            LamaNullNode result = new LamaNullNode();
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        @Override
        public LamaExpressionNode visitParenthesizedExpression(ParenthesizedExpressionContext ctx) {
            LamaExpressionNode result = visit(ctx.scopeExpression());
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        @Override
        public LamaExpressionNode visitListLiteral(ListLiteralContext ctx) {
            return visit(ctx.listExpression());
        }

        @Override
        public LamaExpressionNode visitArrayLiteral(ArrayLiteralContext ctx) {
            return visit(ctx.arrayExpression());
        }

        @Override
        public LamaExpressionNode visitSExpressionLiteral(SExpressionLiteralContext ctx) {
            return visit(ctx.sExpression());
        }

        @Override
        public LamaExpressionNode visitIfPrimary(IfPrimaryContext ctx) {
            return visit(ctx.ifExpression());
        }

        @Override
        public LamaExpressionNode visitWhileDoPrimary(WhileDoPrimaryContext ctx) {
            return visit(ctx.whileDoExpression());
        }

        @Override
        public LamaExpressionNode visitDoWhilePrimary(DoWhilePrimaryContext ctx) {
            return visit(ctx.doWhileExpression());
        }

        @Override
        public LamaExpressionNode visitForPrimary(ForPrimaryContext ctx) {
            return visit(ctx.forExpression());
        }

        @Override
        public LamaExpressionNode visitCasePrimary(CasePrimaryContext ctx) {
            return visit(ctx.caseExpression());
        }

        @Override
        public LamaExpressionNode visitLetPrimary(LetPrimaryContext ctx) {
            return visit(ctx.letExpression());
        }

        @Override
        public LamaExpressionNode visitListExpression(ListExpressionContext ctx) {
            LamaExpressionNode listNode = new LamaNullNode();
            listNode.setSourceSectionUnavailable();
            for (ExpressionContext listElementContext : ctx.expression().reversed()) {
                TruffleString tag = TruffleString.fromJavaStringUncached("cons", LamaLanguage.STRING_ENCODING);
                LamaExpressionNode[] expressions = new LamaExpressionNode[]{visit(listElementContext), listNode};
                listNode = new LamaSExpressionLiteralNode(tag, expressions);
                setSourceSection(listNode, listElementContext.start, ctx.stop);
            }
            return listNode;
        }

        @Override
        public LamaExpressionNode visitArrayExpression(ArrayExpressionContext ctx) {
            LamaExpressionNode[] elements = ctx.expression().stream()
                    .map(this::visit)
                    .toArray(LamaExpressionNode[]::new);
            LamaArrayLiteralNode result = new LamaArrayLiteralNode(elements);
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        @Override
        public LamaExpressionNode visitSExpression(SExpressionContext ctx) {
            TruffleString tag = asTruffleString(ctx.UIDENT().getSymbol());
            LamaExpressionNode[] expressions = ctx.expression().stream()
                    .map(this::visit)
                    .toArray(LamaExpressionNode[]::new);
            LamaSExpressionLiteralNode result = new LamaSExpressionLiteralNode(tag, expressions);
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        @Override
        public LamaExpressionNode visitIfExpression(IfExpressionContext ctx) {
            return visitIfExpression(ctx, this::visit);
        }

        @SuppressWarnings("DuplicatedCode")
        private LamaIfNode visitIfExpression(IfExpressionContext ctx, Function<ScopeExpressionContext, LamaExpressionNode> visitor) {
            LamaExpressionNode condition = visit(ctx.expression());
            LamaExpressionNode thenBranch = visitor.apply(ctx.scopeExpression());
            LamaExpressionNode elseBranch;
            if (ctx.elsePart() instanceof ElifExpressionContext elifExpressionContext) {
                elseBranch = visitElifExpression(elifExpressionContext, visitor);
            } else if (ctx.elsePart() instanceof ElseExpressionContext elseExpressionContext) {
                elseBranch = visitElseExpression(elseExpressionContext, visitor);
            } else {
                elseBranch = new LamaNullNode();
                elseBranch.setSourceSectionUnavailable();
            }
            LamaIfNode result = new LamaIfNode(condition, thenBranch, elseBranch);
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        @Override
        public LamaExpressionNode visitElifExpression(ElifExpressionContext ctx) {
            return visitElifExpression(ctx, this::visit);
        }

        @SuppressWarnings("DuplicatedCode")
        private LamaIfNode visitElifExpression(ElifExpressionContext ctx, Function<ScopeExpressionContext, LamaExpressionNode> visitor) {
            LamaExpressionNode condition = visit(ctx.expression());
            LamaExpressionNode thenBranch = visitor.apply(ctx.scopeExpression());
            LamaExpressionNode elseBranch;
            if (ctx.elsePart() instanceof ElifExpressionContext elifExpressionContext) {
                elseBranch = visitElifExpression(elifExpressionContext, visitor);
            } else if (ctx.elsePart() instanceof ElseExpressionContext elseExpressionContext) {
                elseBranch = visitElseExpression(elseExpressionContext, visitor);
            } else {
                elseBranch = new LamaNullNode();
                elseBranch.setSourceSectionUnavailable();
            }
            LamaIfNode result = new LamaIfNode(condition, thenBranch, elseBranch);
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        @Override
        public LamaExpressionNode visitElseExpression(ElseExpressionContext ctx) {
            return visitElseExpression(ctx, this::visit);
        }

        private LamaExpressionNode visitElseExpression(ElseExpressionContext ctx, Function<ScopeExpressionContext, LamaExpressionNode> visitor) {
            LamaExpressionNode result = visitor.apply(ctx.scopeExpression());
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        @Override
        public LamaExpressionNode visitWhileDoExpression(WhileDoExpressionContext ctx) {
            LamaExpressionNode condition = visit(ctx.expression());
            LamaExpressionNode body = visit(ctx.scopeExpression());
            LamaWhileDoNode result = new LamaWhileDoNode(condition, body);
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        @Override
        public LamaExpressionNode visitDoWhileExpression(DoWhileExpressionContext ctx) {
            LamaExpressionNode body = visit(ctx.scopeExpression());
            LamaExpressionNode condition = inScope(scopeExpressions.get(ctx.scopeExpression()), () -> visit(ctx.expression()));
            LamaDoWhileNode result = new LamaDoWhileNode(body, condition);
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        @Override
        public LamaExpressionNode visitForExpression(ForExpressionContext ctx) {
            LamaExpressionNode initializer = visit(ctx.scopeExpression().getFirst());
            LamaForNode result = inScope(scopeExpressions.get(ctx.scopeExpression().getFirst()), () -> visitForExpression(initializer, ctx.expression().getFirst(), ctx.expression().getLast(), ctx.scopeExpression().getLast()));
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        private LamaForNode visitForExpression(LamaExpressionNode initializer, ExpressionContext conditionContext, ExpressionContext stepContext, ScopeExpressionContext bodyContext) {
            LamaExpressionNode condition = visit(conditionContext);
            LamaExpressionNode step = visit(stepContext);
            LamaExpressionNode body = visit(bodyContext);
            return new LamaForNode(initializer, condition, step, body);
        }

        @Override
        public LamaExpressionNode visitCaseExpression(CaseExpressionContext ctx) {
            return visitCaseExpression(ctx, this::visit);
        }

        private LamaCaseNode visitCaseExpression(CaseExpressionContext ctx, Function<ScopeExpressionContext, LamaExpressionNode> visitor) {
            LamaExpressionNode scrutinee = visit(ctx.expression());
            LamaPatternNode[] patterns = new LamaPatternNode[ctx.caseBranches().caseBranch().size()];
            LamaExpressionNode[] branches = new LamaExpressionNode[ctx.caseBranches().caseBranch().size()];
            for (int i = 0; i < ctx.caseBranches().caseBranch().size(); i++) {
                visitCaseBranch(patterns, branches, ctx, i, visitor);
            }
            LamaCaseNode result = new LamaCaseNode(scrutinee, patterns, branches);
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        private void visitCaseBranch(LamaPatternNode[] patterns, LamaExpressionNode[] branches, CaseExpressionContext ctx, int i, Function<ScopeExpressionContext, LamaExpressionNode> visitor) {
            newScope(() -> visitCaseBranch(patterns, branches, ctx.caseBranches(), i, visitor));
        }

        private Void visitCaseBranch(LamaPatternNode[] patterns, LamaExpressionNode[] branches, CaseBranchesContext ctx, int i, Function<ScopeExpressionContext, LamaExpressionNode> visitor) {
            patterns[i] = patternVisitor.visit(ctx.caseBranch().get(i).pattern());
            branches[i] = visitor.apply(ctx.caseBranch().get(i).scopeExpression());
            return null;
        }

        @Override
        public LamaExpressionNode visitLetExpression(LetExpressionContext ctx) {
            return newScope(() -> visitLetExpression(ctx, this::visit));
        }

        private LamaLetNode visitLetExpression(LetExpressionContext ctx, Function<ExpressionContext, LamaExpressionNode> visitor) {
            LamaPatternNode pattern = patternVisitor.visit(ctx.pattern());
            LamaExpressionNode scrutinee = visitor.apply(ctx.expression().getFirst());
            LamaExpressionNode body = visitor.apply(ctx.expression().getLast());
            LamaLetNode result = new LamaLetNode(pattern, scrutinee, body);
            setSourceSection(result, ctx.start, ctx.stop);
            return result;
        }

        private List<LamaExpressionNode> flatten(List<LamaExpressionNode> nodes) {
            List<LamaExpressionNode> result = new ArrayList<>();
            for (LamaExpressionNode node : nodes) {
                if (node instanceof LamaSequenceNode sequenceNode) {
                    result.addAll(flatten(List.of(sequenceNode.getNodes())));
                } else {
                    result.add(node);
                }
            }
            return result;
        }

        private LamaExpressionNode toLamaSequenceNode(List<LamaExpressionNode> nodes, Token start, Token stop) {
            nodes = flatten(nodes);
            if (nodes.size() == 1) {
                return nodes.getFirst();
            }
            LamaSequenceNode result = new LamaSequenceNode(nodes.toArray(new LamaExpressionNode[0]));
            setSourceSection(result, start, stop);
            return result;
        }
    }

    private class AssignmentExpressionVisitor {
        private final Scope scope;
        private final Token start;
        private final Token stop;
        private final BinaryExpressionContext rightContext;

        public AssignmentExpressionVisitor(Scope scope, AssignmentExpressionContext assignmentExpressionContext, BinaryExpressionContext rightContext) {
            this.scope = scope;
            this.start = assignmentExpressionContext.start;
            this.stop = assignmentExpressionContext.stop;
            this.rightContext = rightContext;
        }

        public LamaExpressionNode visitScopeExpression(ScopeExpressionContext ctx) {
            return expressionVisitor.visitScopeExpression(ctx, this::visitExpression);
        }

        public LamaExpressionNode visitExpression(ExpressionContext ctx) {
            return expressionVisitor.visitExpression(ctx, this::visitBasicExpression);
        }

        public LamaExpressionNode visitBasicExpression(BasicExpressionContext ctx) {
            return visitBinaryExpression(ctx.binaryExpression());
        }

        public LamaExpressionNode visitBinaryExpression(BinaryExpressionContext ctx) {
            if (ctx instanceof UnaryExpressionContext unaryExpressionContext) {
                return visitUnaryExpression(unaryExpressionContext);
            }
            semErr(ctx.start, "illegal assignment");
            return null;
        }

        public LamaExpressionNode visitUnaryExpression(UnaryExpressionContext ctx) {
            return visitPostfixExpression(ctx.postfixExpression());
        }

        public LamaExpressionNode visitPostfixExpression(PostfixExpressionContext ctx) {
            if (ctx instanceof PrimaryExpressionContext primaryExpressionContext) {
                return visitPrimaryExpression(primaryExpressionContext);
            }
            if (ctx instanceof SubscriptExpressionContext subscriptExpressionContext) {
                return visitSubscriptExpression(subscriptExpressionContext);
            }
            semErr(ctx.start, "illegal assignment");
            return null;
        }

        public LamaExpressionNode visitPrimaryExpression(PrimaryExpressionContext ctx) {
            return visitPrimary(ctx.primary());
        }

        public LamaExpressionNode visitSubscriptExpression(SubscriptExpressionContext ctx) {
            LamaExpressionNode base = expressionVisitor.visit(ctx.postfixExpression());
            LamaExpressionNode index = expressionVisitor.visit(ctx.expression());
            LamaStoreSubscriptNode result = LamaStoreSubscriptNodeGen.create(base, index, visitRight());
            setSourceSection(result, start, stop);
            return result;
        }

        public LamaExpressionNode visitPrimary(PrimaryContext ctx) {
            if (ctx instanceof NameReferenceContext nameReferenceContext) {
                return visitNameReference(nameReferenceContext);
            }
            if (ctx instanceof ParenthesizedExpressionContext parenthesizedExpressionContext) {
                return visitParenthesizedExpression(parenthesizedExpressionContext);
            }
            if (ctx instanceof IfPrimaryContext ifPrimaryContext) {
                return visitIfPrimary(ifPrimaryContext);
            }
            if (ctx instanceof CasePrimaryContext casePrimaryContext) {
                return visitCasePrimary(casePrimaryContext);
            }
            semErr(ctx.start, "illegal assignment");
            return null;
        }

        public LamaExpressionNode visitNameReference(NameReferenceContext ctx) {
            LamaExpressionNode result = visitNameReference(ctx.LIDENT().getSymbol());
            setSourceSection(result, start, stop);
            return result;
        }

        private LamaExpressionNode visitNameReference(Token name) {
            return resolve(name, asTruffleString(name)).accept(new StoreSymbolVisitor(visitRight(), name));
        }

        public LamaExpressionNode visitParenthesizedExpression(ParenthesizedExpressionContext ctx) {
            return visitScopeExpression(ctx.scopeExpression());
        }

        public LamaExpressionNode visitIfPrimary(IfPrimaryContext ctx) {
            return visitIfExpression(ctx.ifExpression());
        }

        public LamaExpressionNode visitCasePrimary(CasePrimaryContext ctx) {
            return visitCaseExpression(ctx.caseExpression());
        }

        public LamaExpressionNode visitIfExpression(IfExpressionContext ctx) {
            return expressionVisitor.visitIfExpression(ctx,this::visitScopeExpression);
        }

        public LamaExpressionNode visitCaseExpression(CaseExpressionContext ctx) {
            return expressionVisitor.visitCaseExpression(ctx, this::visitScopeExpression);
        }

        private LamaExpressionNode visitRight() {
            return inScope(scope, () -> expressionVisitor.visit(rightContext));
        }
    }

    private class PatternVisitor extends LamaBaseVisitor<LamaPatternNode> {
        @Override
        public LamaPatternNode visitPattern(PatternContext ctx) {
            LamaPatternNode result = null;
            if (Objects.nonNull(ctx.consPattern())) {
                result = visit(ctx.consPattern());
            } else if (Objects.nonNull(ctx.simplePattern())) {
                result = visit(ctx.simplePattern());
            }
            return result;
        }

        @Override
        public LamaPatternNode visitConsPattern(ConsPatternContext ctx) {
            TruffleString tag = TruffleString.fromJavaStringUncached("cons", LamaLanguage.STRING_ENCODING);
            LamaPatternNode[] expressions = new LamaPatternNode[]{visit(ctx.simplePattern()), visit(ctx.pattern())};
            return LamaSExpressionLiteralPatternNodeGen.create(tag, expressions);
        }

        @Override
        public LamaPatternNode visitSimpleWildcardPattern(SimpleWildcardPatternContext ctx) {
            return visit(ctx.wildcardPattern());
        }

        @Override
        public LamaPatternNode visitSimpleSExpressionPattern(SimpleSExpressionPatternContext ctx) {
            return visit(ctx.sExpressionPattern());
        }

        @Override
        public LamaPatternNode visitSimpleArrayPattern(SimpleArrayPatternContext ctx) {
            return visit(ctx.arrayPattern());
        }

        @Override
        public LamaPatternNode visitSimpleListPattern(SimpleListPatternContext ctx) {
            return visit(ctx.listPattern());
        }

        @Override
        public LamaPatternNode visitNameReferencePattern(NameReferencePatternContext ctx) {
            int slot = currentScope.declareLocal(ctx.LIDENT().getSymbol(), asTruffleString(ctx.LIDENT().getSymbol()));
            LamaPatternNode inner = Objects.nonNull(ctx.pattern()) ? visit(ctx.pattern()) : new LamaWildcardPatternNode();
            return LamaNameReferencePatternNodeGen.create(slot, inner);
        }

        @Override
        public LamaPatternNode visitIntegerLiteralPattern(IntegerLiteralPatternContext ctx) {
            return LamaIntegerLiteralPatternNodeGen.create(parseIntegerLiteral(ctx.DECIMAL().getSymbol(), Objects.nonNull(ctx.INFIX_SUBTRACTION())));
        }

        @Override
        public LamaPatternNode visitStringLiteralPattern(StringLiteralPatternContext ctx) {
            return LamaStringLiteralPatternNodeGen.create(parseStringLiteral(ctx.STRING().getSymbol()));
        }

        @Override
        public LamaPatternNode visitCharLiteralPattern(CharLiteralPatternContext ctx) {
            return LamaIntegerLiteralPatternNodeGen.create(parseCharLiteral(ctx.CHAR().getSymbol()));
        }

        @Override
        public LamaPatternNode visitTrueLiteralPattern(TrueLiteralPatternContext ctx) {
            return LamaIntegerLiteralPatternNodeGen.create(1);
        }

        @Override
        public LamaPatternNode visitFalseLiteralPattern(FalseLiteralPatternContext ctx) {
            return LamaIntegerLiteralPatternNodeGen.create(0);
        }

        @Override
        public LamaPatternNode visitHashBoxPattern(HashBoxPatternContext ctx) {
            return LamaIsBoxPatternNodeGen.create();
        }

        @Override
        public LamaPatternNode visitHashValPattern(HashValPatternContext ctx) {
            return LamaIsValPatternNodeGen.create();
        }

        @Override
        public LamaPatternNode visitHashStrPattern(HashStrPatternContext ctx) {
            return LamaIsStringPatternNodeGen.create();
        }

        @Override
        public LamaPatternNode visitHashArrayPattern(HashArrayPatternContext ctx) {
            return LamaIsArrayPatternNodeGen.create();
        }

        @Override
        public LamaPatternNode visitHashSexpPattern(HashSexpPatternContext ctx) {
            return LamaIsSExpressionPatternNodeGen.create();
        }

        @Override
        public LamaPatternNode visitHashFunPattern(HashFunPatternContext ctx) {
            return LamaIsFunctionPatternNodeGen.create();
        }

        @Override
        public LamaPatternNode visitParenthesizedPattern(ParenthesizedPatternContext ctx) {
            return visit(ctx.pattern());
        }

        @Override
        public LamaPatternNode visitWildcardPattern(WildcardPatternContext ctx) {
            return new LamaWildcardPatternNode();
        }

        @Override
        public LamaPatternNode visitSExpressionPattern(SExpressionPatternContext ctx) {
            TruffleString tag = asTruffleString(ctx.UIDENT().getSymbol());
            LamaPatternNode[] expressions = ctx.pattern().stream()
                    .map(this::visit)
                    .toArray(LamaPatternNode[]::new);
            return LamaSExpressionLiteralPatternNodeGen.create(tag, expressions);
        }

        @Override
        public LamaPatternNode visitArrayPattern(ArrayPatternContext ctx) {
            LamaPatternNode[] arrayElements = ctx.pattern().stream()
                    .map(this::visit)
                    .toArray(LamaPatternNode[]::new);
            return LamaArrayLiteralPatternNodeGen.create(arrayElements);
        }

        @Override
        public LamaPatternNode visitListPattern(ListPatternContext ctx) {
            LamaPatternNode listNode = LamaNullPatternNodeGen.create();
            for (PatternContext listElementContext : ctx.pattern().reversed()) {
                TruffleString tag = TruffleString.fromJavaStringUncached("cons", LamaLanguage.STRING_ENCODING);
                LamaPatternNode[] expressions = new LamaPatternNode[]{visit(listElementContext), listNode};
                listNode = LamaSExpressionLiteralPatternNodeGen.create(tag, expressions);
            }
            return listNode;
        }
    }

    private SourceSection createSection(Token start, Token stop) {
        return source.createSection(start.getStartIndex(), stop.getStopIndex() - start.getStartIndex());
    }

    private void setSourceSection(LamaExpressionNode node, Token start, Token stop) {
        node.setSourceSection(start.getStartIndex(), stop.getStopIndex() - start.getStartIndex());
    }

    private Variable resolve(Token token, TruffleString name) {
        return resolveOrCapture(currentScope, token, name);
    }

    private Variable resolveOrCapture(Scope scope, Token token, TruffleString name) {
        while (Objects.nonNull(scope)) {
            Optional<Variable> result = scope.get(name);
            if (result.isPresent()) {
                return result.get();
            }
            if (scope instanceof FunctionScope functionScope) {
                return capture(functionScope, token, name);
            }
            scope = scope.parent;
        }
        semErr(token, "undefined variable: %s".formatted(name));
        return null;
    }

    private Variable capture(FunctionScope functionScope, Token token, TruffleString name) {
        if (Objects.isNull(functionScope.parent)) {
            semErr(token, "undefined variable: %s".formatted(name));
        }
        Variable variable = resolveOrCapture(functionScope.parent, token, name);
        return Objects.nonNull(variable) && !variable.isGlobal() ? functionScope.declareCapture(name, variable) : variable;
    }

    private static long parseIntegerLiteral(Token ctx, boolean negative) {
        String s = negative ? "-" + ctx.getText() : ctx.getText();
        return Long.parseLong(s);
    }

    private static TruffleString parseStringLiteral(Token ctx) {
        String s = ctx.getText();
        s = s.substring(1, s.length() - 1);
        s = s.replace("\"\"", "\"");
        return TruffleString.fromJavaStringUncached(s, LamaLanguage.STRING_ENCODING);
    }

    private static int parseCharLiteral(Token ctx) {
        String s = ctx.getText();
        return switch(s) {
            case "''''" -> '\'';
            case "\\n" -> '\n';
            case "\\t" -> '\t';
            default -> s.codePointAt(1);
        };
    }
}
