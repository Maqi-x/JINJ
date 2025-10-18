#include <stdint.h>

typedef struct JinjParser JinjParser;
typedef struct _JinjParserASTNode _JinjParserASTNode;

typedef struct _JinjParserASTFloatNode {
    double value;
} _JinjParserASTFloatNode;

_JinjParserASTNode* _jinj_parser_ast_float_node_alloc(JinjParser* parser);
