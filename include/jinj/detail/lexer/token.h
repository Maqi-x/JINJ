#ifndef JINJ_DETAIL_LEXER_TOKEN_H
#define JINJ_DETAIL_LEXER_TOKEN_H

#include <jinj/defs.h>
#include <stdio.h>

typedef enum JinjTokenType {
    JinjTokenTypeIdent,
    JinjTokenTypeString,
    JinjTokenTypeChar,
    JinjTokenTypeInt,
    JinjTokenTypeFloat,
    JinjTokenTypeBool,
    JinjTokenTypeNil,

    JinjTokenTypeLParen, JinjTokenTypeRParen,
    JinjTokenTypeLBracket, JinjTokenTypeRBracket,
    JinjTokenTypeLBrace, JinjTokenTypeRBrace,

    JinjTokenTypeHash,
    JinjTokenTypeComma,
    JinjTokenTypeColon,
    JinjTokenTypeEqual,

    JinjTokenTypeComment,
    JinjTokenTypeMultilineComment,

    JinjTokenTypeEOF,
} JinjTokenType;

typedef struct JinjTokenLocation {
    int line;
    int column;
} JinjTokenLocation;

typedef struct JinjTokenValue {
    const char* str;
    usize len;
} JinjTokenValue;

typedef struct JinjToken {
    JinjTokenType type;
    JinjTokenLocation location;
    JinjTokenValue value;
} JinjToken;

const char* jinj_token_type_to_string(JinjTokenType tt);
usize jinj_format_token(JinjToken tok, usize n, char buf[static n]);
usize jinj_token_to_string(JinjToken tok, char** out);
usize jinj_print_token(JinjToken tok, FILE* out);

JinjToken jinj_make_token(JinjTokenType type, int line, int column);
JinjToken jinj_make_token_with_value(JinjTokenType type, int line, int column,
                                     const char* value, usize value_len);

#endif // JINJ_DETAIL_LEXER_TOKEN_H
