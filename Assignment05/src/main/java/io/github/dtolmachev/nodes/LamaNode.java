package io.github.dtolmachev.nodes;

import com.oracle.truffle.api.dsl.TypeSystemReference;
import com.oracle.truffle.api.nodes.Node;

@TypeSystemReference(LamaTypes.class)
public abstract class LamaNode extends Node {
}
