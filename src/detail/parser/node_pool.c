#include <jinj/detail/parser/ast/node_pool.h>

#include <stdlib.h>
#include <string.h>

#include <jinj/defs.h>
#include <jinj/detail/lib/next_power_of_two.h>

_JinjParserASTNodePoolBlock* _jinj_parser_ast_node_pool_add_block(_JinjParserASTNodePool* pool) {
    _JinjParserASTNodePoolBlock* new_block = malloc(sizeof(*new_block));
    if (new_block != NULL) {
        if (pool->tail != NULL) {
            pool->tail->next = new_block;
        } else {
            pool->head = new_block; 
        }
        pool->tail = new_block;
        new_block->next = NULL;
        pool->num_blocks++;

        new_block->num_used = 0;
        new_block->is_owned = JINJ_TRUE;
    }
    
    return new_block;
}

_JinjParserASTNodePoolBlock* _jinj_parser_ast_node_pool_add_blocks(_JinjParserASTNodePool* pool,
                                                                   jinj_usize_t num_new_blocks) {
    if (num_new_blocks == 0) return NULL;
    if (num_new_blocks == 1) return _jinj_parser_ast_node_pool_add_block(pool);

    // calloc resets the memory, so each block has .is_malloced = false, .num_used = 0
    _JinjParserASTNodePoolBlock* blocks = calloc(num_new_blocks,
                                                 sizeof(_JinjParserASTNodePoolBlock));
    if (!blocks) return NULL;

    // only the first block in a contiguous calloc'ed region owns the memory
    blocks[0].is_owned = JINJ_TRUE;

    for (jinj_usize_t i = 0; i < num_new_blocks - 1; i++) {
        blocks[i].next = &blocks[i + 1];
    }

    if (pool->tail) {
        pool->tail->next = &blocks[0];
    } else {
        pool->head = &blocks[0];
    }

    pool->tail = &blocks[num_new_blocks - 1];
    pool->num_blocks += num_new_blocks;
    return &blocks[0];
}

void _jinj_parser_ast_node_pool_init(_JinjParserASTNodePool* pool) {
    pool->num_blocks = 0;
    pool->tail = NULL;
    pool->head = NULL;
}

void _jinj_parser_ast_node_pool_free(_JinjParserASTNodePool* pool) {
    _JinjParserASTNodePoolBlock* block = pool->head;
    while (block != NULL) {
        _JinjParserASTNodePoolBlock* next = block->next;
        if (block->is_owned) {
            free(block);
        }
        block = next;
    }

    pool->num_blocks = 0;
}

_JinjParserASTNode* _jinj_parser_ast_node_pool_alloc_uninit(_JinjParserASTNodePool* pool) {
    if (pool->tail == NULL || pool->tail->num_used >= _JINJ_PARSER_AST_NODE_POOL_BLOCK_SIZE) {
        if (!_jinj_parser_ast_node_pool_add_block(pool)) return NULL;
    }

    return &pool->tail->data[pool->tail->num_used++];
}

_JinjParserASTNode* _jinj_parser_ast_node_pool_alloc_many_uninit(_JinjParserASTNodePool* pool, jinj_usize_t count) {
    if (count == 0) return NULL;

    count = _jinj_next_power_of_two(count);

    if (pool->tail && pool->tail->num_used + count <= _JINJ_PARSER_AST_NODE_POOL_BLOCK_SIZE) {
        _JinjParserASTNode* res = &pool->tail->data[pool->tail->num_used];
        pool->tail->num_used += count;
        return res;
    }

    // calculate how many full blocks we need to allocate to fit 'count' nodes
    // this is equivalent to: ceil(count / _JINJ_PARSER_AST_NODE_POOL_BLOCK_SIZE)
    // the bitshift with __builtin_ctz assumes the block size is a power of two
    jinj_usize_t needed_blocks =
        (count + _JINJ_PARSER_AST_NODE_POOL_BLOCK_SIZE - 1) >> __builtin_ctz(_JINJ_PARSER_AST_NODE_POOL_BLOCK_SIZE);

    _JinjParserASTNodePoolBlock* first_new = _jinj_parser_ast_node_pool_add_blocks(pool, needed_blocks);
    if (!first_new) return NULL;

    // compute how many nodes are used in the last block after allocation
    // equivalent to count % block_size, but faster since block_size is a power of two
    jinj_usize_t used_in_last = count & (_JINJ_PARSER_AST_NODE_POOL_BLOCK_SIZE - 1);
    if (used_in_last == 0) used_in_last = _JINJ_PARSER_AST_NODE_POOL_BLOCK_SIZE;
    pool->tail->num_used = used_in_last;

    return &first_new->data[0];
}

_JinjParserASTNode* _jinj_parser_ast_node_pool_alloc_zeroed(_JinjParserASTNodePool* pool) {
    _JinjParserASTNode* node = _jinj_parser_ast_node_pool_alloc_uninit(pool);
    if (node != NULL) {
        memset(node, 0, sizeof(*node));
    }
    return node;
}

_JinjParserASTNode* _jinj_parser_ast_node_pool_alloc_many_zeroed(_JinjParserASTNodePool* pool, jinj_usize_t count) {
    _JinjParserASTNode* nodes = _jinj_parser_ast_node_pool_alloc_many_uninit(pool, count);
    if (nodes != NULL) {
        memset(nodes, 0, sizeof(nodes[0]) * count);
    }
    return nodes;

}
