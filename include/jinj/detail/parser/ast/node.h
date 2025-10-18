#ifndef JINJ_DETAIL_PARSER_AST_NODE_H
#define JINJ_DETAIL_PARSER_AST_NODE_H

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

#endif // JINJ_DETAIL_PARSER_AST_NODE_H

