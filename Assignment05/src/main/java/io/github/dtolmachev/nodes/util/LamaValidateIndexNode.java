package io.github.dtolmachev.nodes.util;

import io.github.dtolmachev.LamaException;
import io.github.dtolmachev.nodes.LamaNode;

public final class LamaValidateIndexNode extends LamaNode {
    public void execute(LamaNode node, int size, int index) {
        if (index < 0 || index >= size) {
            throw LamaException.outOfRange(node, size, index);
        }
    }
}
