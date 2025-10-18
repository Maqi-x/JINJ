#ifndef JINJ_DETAIL_PARSER_PARSER_H
#define JINJ_DETAIL_PARSER_PARSER_H

#include "ast/node_pool.h"

typedef struct JinjParser {
    _JinjParserASTNodePool* node_pool;
} JinjParser;

#endif // JINJ_DETAIL_PARSER_PARSER_H
