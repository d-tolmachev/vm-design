package io.github.dtolmachev.nodes.expression;

import com.oracle.truffle.api.CompilerAsserts;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.ExplodeLoop;
import com.oracle.truffle.api.nodes.IndirectCallNode;
import com.oracle.truffle.api.nodes.NodeInfo;
import com.oracle.truffle.api.nodes.UnexpectedResultException;

import io.github.dtolmachev.LamaException;
import io.github.dtolmachev.nodes.LamaExpressionNode;
import io.github.dtolmachev.runtime.LamaFunction;

@NodeInfo(shortName = "()")
public final class LamaCallNode extends LamaExpressionNode {
    @Child private LamaExpressionNode callableNode;
    @Children private LamaExpressionNode[] argumentNodes;
    @Child private IndirectCallNode callNode;

    public LamaCallNode(LamaExpressionNode callableNode, LamaExpressionNode[] argumentNodes) {
        this.callableNode = callableNode;
        this.argumentNodes = argumentNodes;
        this.callNode = IndirectCallNode.create();
    }

    @Override
    @ExplodeLoop
    public Object execute(VirtualFrame frame) {
        CompilerAsserts.partialEvaluationConstant(argumentNodes.length);
        LamaFunction function;
        try {
            function = callableNode.executeFunction(frame);
        } catch (UnexpectedResultException e) {
            throw LamaException.typeError(callableNode, "function", e.getResult());
        }
        if (argumentNodes.length != function.getArgumentsCount()) {
            throw LamaException.argumentCountMismatch(this, function.getArgumentsCount(), argumentNodes.length);
        }
        Object[] arguments = new Object[argumentNodes.length + 1];
        arguments[0] = function;
        for (int i = 0; i < argumentNodes.length; i++) {
            arguments[i + 1] = argumentNodes[i].execute(frame);
        }
        return callNode.call(function.getCallTarget(), arguments);
    }
}
