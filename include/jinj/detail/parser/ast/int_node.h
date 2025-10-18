#ifndef JINJ_DETAIL_PARSER_AST_INT_NODE_H
#define JINJ_DETAIL_PARSER_AST_INT_NODE_H

#include <stdint.h>

typedef struct JinjParser JinjParser;
typedef struct _JinjParserASTNode _JinjParserASTNode;

typedef struct _JinjParserASTIntNode {
    uint64_t value;
} _JinjParserASTIntNode;

_JinjParserASTNode* _jinj_parser_ast_int_node_alloc(JinjParser* parser);

#endif // JINJ_DETAIL_PARSER_AST_INT_NODE_H
