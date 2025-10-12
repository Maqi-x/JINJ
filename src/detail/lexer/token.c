#include <jinj/detail/lexer/token.h>
#include <stdlib.h>
#include <string.h>

static const char* jinj_token_type_to_string_map[] = {
    [JinjTokenTypeIdent] =  "IDENT",
    [JinjTokenTypeString] = "STRING",
    [JinjTokenTypeChar] =   "CHAR",
    [JinjTokenTypeInt] =    "INT",
    [JinjTokenTypeFloat] =  "FLOAT",
    [JinjTokenTypeBool] =   "BOOL",
    [JinjTokenTypeNil] =    "NIL",

    [JinjTokenTypeLParen] =   "LPAREN",
    [JinjTokenTypeRParen] =   "RPAREN",
    [JinjTokenTypeLBracket] = "BRACKET",
    [JinjTokenTypeRBracket] = "BRACKET",
    [JinjTokenTypeLBrace] =   "LBRACE",
    [JinjTokenTypeRBrace] =   "RBRACE",

    [JinjTokenTypeHash] =  "HASH",
    [JinjTokenTypeComma] = "COMMA",
    [JinjTokenTypeColon] = "COLON",
    [JinjTokenTypeEqual] = "EQUAL",

    [JinjTokenTypeLineComment] =   "LINE_COMMENT",
    [JinjTokenTypeBlockComment] = "BLOCK_COMMENT",

    [JinjTokenTypeWhitespace] = "WHITESPACE",

    [JinjTokenTypeEOF] = "EOF",
};

const char* jinj_token_type_to_string(JinjTokenType tt) {
    if (tt < 0 || tt >= sizeof(jinj_token_type_to_string_map)/sizeof(char*))
        return "UNKNOWN";
    const char* s = jinj_token_type_to_string_map[tt];
    return s ? s : "UNKNOWN";
}

usize jinj_format_token(JinjToken tok, usize n, char buf[static n]) {
    const char* type_string = jinj_token_type_to_string(tok.type);
    const usize type_string_len = strlen(type_string);

    if (type_string_len <= n) {
        memcpy(buf, type_string, n-1);
        buf[n-1] = '\0';
        return n;
    }

    usize count = 0;
    memcpy(buf, type_string, type_string_len);
    count += n;

    if (tok.value.str != NULL) {
        buf[count++] = '('; // there must always be at least one free character here
                            // - guaranteed by the condition from line 36

        if (tok.value.len <= n) {
            memcpy(buf + count, tok.value.str, n - count - 1);
            count += n - count;
            buf[n-1] = '\0';
            return count;
        }

        buf[count++] = ')'; // there also must always be at least one free character here
                            // - guaranteed by the condition from line 49
    }

    return count;
}

usize jinj_token_to_string(JinjToken tok, char** out) {
    const char* type_string = jinj_token_type_to_string(tok.type);
    const usize type_string_len = strlen(type_string);

    if (tok.value.str == NULL) {
        *out = malloc(type_string_len + 1); // +1 for \0
        *out = memcpy(*out, type_string, type_string_len + 1);
        return type_string_len;
    }

    const char* value_string = tok.value.str;
    const usize value_string_len = tok.value.len;

    const usize full_len =
        type_string_len
        + 1 // '('
        + value_string_len
        + 1; // ')'

    *out = malloc(full_len + 1); // +1 for \0
    memcpy(*out, type_string, type_string_len);
    (*out)[type_string_len] = '(';
    memcpy(*out + type_string_len + 1, value_string, value_string_len);
    (*out)[type_string_len + 1 + value_string_len] = ')';
    (*out)[full_len] = '\0';
    return type_string_len;
}

JinjToken jinj_make_token(JinjTokenType type, int line, int column) {
    return (JinjToken) {
        .type = type,
        .location = (JinjTokenLocation) { .line = line, .column = column },
    };
}

JinjToken jinj_make_token_with_value(JinjTokenType type, int line, int column,
                                     const char* value, usize value_len) {
    return (JinjToken) {
        .type = type,
        .location = (JinjTokenLocation) { .line = line, .column = column },
        .value = (JinjTokenValue) { .str = value, .len = value_len },
    };
}
