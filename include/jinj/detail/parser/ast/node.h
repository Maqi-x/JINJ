#include "int_node.h"
#include "float_node.h"

typedef struct _JinjParserASTNode {
    enum _JinjParserASTNodeType {
        _JinjParserASTNodeTypeInt,
        _JinjParserASTNodeTypeFloat,
    } type;
    union _JinjParserASTNodeData {
        _JinjParserASTIntNode int_node;
        _JinjParserASTFloatNode float_node;
    } value;
} _JinjParserASTNode;
