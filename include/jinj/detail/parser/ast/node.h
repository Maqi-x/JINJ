#include "int_node.h"

typedef struct _JinjParserASTNode {
    enum _JinjParserASTNodeType {
        _JinjParserASTNodeTypeInt,
    } type;
    union _JinjParserASTNodeData {
        _JinjParserASTIntNode int_node;
    } value;
} _JinjParserASTNode;
