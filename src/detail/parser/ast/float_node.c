#include <jinj/detail/parser/ast/float_node.h>
#include <jinj/detail/parser/parser.h>

#include <stdint.h>
#include <stdlib.h>

_JinjParserASTNode* _jinj_parser_ast_float_node_alloc(JinjParser* parser) {
    _JinjParserASTNode* node = _jinj_parser_ast_node_pool_alloc_uninit(parser->node_pool);
    node->type = _JinjParserASTNodeTypeFloat;
    return node;
}
