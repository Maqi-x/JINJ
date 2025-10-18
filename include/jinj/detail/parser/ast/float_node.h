#ifndef JINJ_DETAIL_PARSER_AST_FLOAT_NODE_H
#define JINJ_DETAIL_PARSER_AST_FLOAT_NODE_H

#include <stdint.h>

typedef struct JinjParser JinjParser;
typedef struct _JinjParserASTNode _JinjParserASTNode;

typedef struct _JinjParserASTFloatNode {
    double value;
} _JinjParserASTFloatNode;

_JinjParserASTNode* _jinj_parser_ast_float_node_alloc(JinjParser* parser);

#endif // JINJ_DETAIL_PARSER_AST_FLOAT_NODE_H
