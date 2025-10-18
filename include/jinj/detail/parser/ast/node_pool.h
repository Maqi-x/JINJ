#ifndef JINJ_PARSER_AST_NODE_POOL_H
#define JINJ_PARSER_AST_NODE_POOL_H

#include <jinj/defs.h>

#include "node.h"

#define _JINJ_PARSER_AST_NODE_POOL_BLOCK_SIZE 64

typedef struct _JinjParserASTNodePoolBlock {
    jinj_usize_t num_used;
    _JinjParserASTNode data[_JINJ_PARSER_AST_NODE_POOL_BLOCK_SIZE];

    struct _JinjParserASTNodePoolBlock* next;
    jinj_bool_t is_owned;
} _JinjParserASTNodePoolBlock;

typedef struct _JinjParserASTNodePool {
    jinj_usize_t num_blocks;
    _JinjParserASTNodePoolBlock* head;
    _JinjParserASTNodePoolBlock* tail;
} _JinjParserASTNodePool;

_JinjParserASTNodePoolBlock* _jinj_parser_ast_node_pool_add_block(_JinjParserASTNodePool* pool);
_JinjParserASTNodePoolBlock* _jinj_parser_ast_node_pool_add_blocks(_JinjParserASTNodePool* pool,
                                                                   jinj_usize_t num_new_blocks);

void _jinj_parser_ast_node_pool_init(_JinjParserASTNodePool* pool);
void _jinj_parser_ast_node_pool_free(_JinjParserASTNodePool* pool);

_JinjParserASTNode* _jinj_parser_ast_node_pool_alloc_uninit(_JinjParserASTNodePool* pool);
_JinjParserASTNode* _jinj_parser_ast_node_pool_alloc_many_uninit(_JinjParserASTNodePool* pool, jinj_usize_t count);

_JinjParserASTNode* _jinj_parser_ast_node_pool_alloc_zeroed(_JinjParserASTNodePool* pool);
_JinjParserASTNode* _jinj_parser_ast_node_pool_alloc_many_zeroed(_JinjParserASTNodePool* pool, jinj_usize_t count);

#endif // JINJ_PARSER_AST_NODE_POOL_H

