#include <jinj/detail/lexer/token.h>
#include <stdlib.h>
#include <string.h>

const char* jinj_token_type_to_string(JinjTokenType tt) {
    switch (tt) {
    case JinjTokenTypeIdent:  return "IDENT";
    case JinjTokenTypeString: return "STRING";
    case JinjTokenTypeChar:   return "CHAR";
    case JinjTokenTypeInt:    return "INT";
    case JinjTokenTypeFloat:  return "FLOAT";
    case JinjTokenTypeBool:   return "BOOL";
    case JinjTokenTypeNil:    return "NIL";

    case JinjTokenTypeLParen:   return "LPAREN";
    case JinjTokenTypeRParen:   return "RPAREN";
    case JinjTokenTypeLBracket: return "LBRACKET";
    case JinjTokenTypeRBracket: return "RBRACKET";
    case JinjTokenTypeLBrace:   return "LBRACE";
    case JinjTokenTypeRBrace:   return "RBRACE";

    case JinjTokenTypeHash:  return "HASH";
    case JinjTokenTypeComma: return "COMMA";
    case JinjTokenTypeColon: return "COLON";
    case JinjTokenTypeEqual: return "EQUAL";

    case JinjTokenTypeComment:          return "COMMENT";
    case JinjTokenTypeMultilineComment: return "MULTILINE_COMMENT";

    case JinjTokenTypeEOF: return "EOF";
    }
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


