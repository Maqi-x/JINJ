#include <stdint.h>

typedef struct JinjParser JinjParser;
typedef struct _JinjParserASTNode _JinjParserASTNode;

typedef struct _JinjParserASTIntNode {
    uint64_t value;
} _JinjParserASTIntNode;

_JinjParserASTIntNode* _jinj_parser_ast_int_node_alloc(JinjParser* parser);
