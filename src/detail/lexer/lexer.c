#include "jinj/detail/lexer/lexer.h"
#include "jinj/detail/lexer/result.h"
#include "jinj/detail/lexer/token.h"
#include "jinj/detail/lexer/tokenlist.h"
#include <assert.h>
#include <ctype.h>
#include <jinj/detail/lexer.h>

static inline char peek(JinjLexer* lexer) {
    if (lexer->pos >= lexer->input_len) return '\0';
    return lexer->input[lexer->pos];
}

static inline char peek_next(JinjLexer* lexer) {
    if (lexer->pos + 1 >= lexer->input_len) return '\0';
    return lexer->input[lexer->pos + 1];
}

static inline char next(JinjLexer* lexer) {
    if (lexer->pos >= lexer->input_len) return '\0';
    char c = lexer->input[lexer->pos++];
    lexer->location.column++;
    if (c == '\n') {
        lexer->location.line++;
        lexer->location.column = 0;
    }
    return c;
}

JinjLexerResult _jinj_lexer_add_token(JinjLexer* lexer, JinjTokenType type) {
    return jinj_token_list_append_new(&lexer->tokens, type,
            lexer->token_start_location.line, lexer->token_start_location.column);
}

JinjLexerResult _jinj_lexer_add_token_with_value(JinjLexer* lexer, JinjTokenType type,
                                                 const char* value, usize value_len) {
    return jinj_token_list_append_new_with_value(&lexer->tokens, type,
            lexer->location.line, lexer->location.column,
            value, value_len);
}

void jinj_lexer_init(JinjLexer *lexer, const char *input, usize input_len) {
    jinj_token_list_init(&lexer->tokens);
    lexer->input = input;
    lexer->input_len = input_len;

    lexer->location = (JinjTokenLocation) { 0, 0 };
    lexer->pos = 0;

    lexer->token_start_location = (JinjTokenLocation) { 0, 0 };
    lexer->token_start_pos = 0;
}

void jinj_lexer_deinit(JinjLexer *lexer) {
    jinj_token_list_deinit(&lexer->tokens);
}

JinjLexerResult jinj_lexer(JinjLexer* lexer) {
    lexer->state = JinjLexerStateDefault;

    while (lexer->pos < lexer->input_len) {
        char c = peek(lexer);

        switch (lexer->state) {
        case JinjLexerStateDefault:
            if (isspace(c)) { next(lexer); break; }

            lexer->token_start_pos = lexer->pos;
            lexer->token_start_location = lexer->location;
            c = next(lexer);

            if (isalpha(c) || c == '_') {
                lexer->state = JinjLexerStateParsingIdent;
                break;
            } else if (c == '"') {
                lexer->state = JinjLexerStateParsingString;
                break;
            } else if (c == '\'') {
                lexer->state = JinjLexerStateParsingChar;
                break;
            } else if (isdigit(c)) {
                lexer->state = JinjLexerStateParsingNumber;
                break;
            }

#           define HANDLE_SINGLE_CHAR_TOKEN(ch, tp) case ch: _jinj_lexer_add_token(lexer, JinjTokenType##tp); break

            switch (c) {
            HANDLE_SINGLE_CHAR_TOKEN('#', Hash);
            HANDLE_SINGLE_CHAR_TOKEN(',', Comma);
            HANDLE_SINGLE_CHAR_TOKEN(':', Colon);
            HANDLE_SINGLE_CHAR_TOKEN('=', Equal);

            HANDLE_SINGLE_CHAR_TOKEN('(', LParen);   HANDLE_SINGLE_CHAR_TOKEN(')', RParen);
            HANDLE_SINGLE_CHAR_TOKEN('[', LBracket); HANDLE_SINGLE_CHAR_TOKEN(']', RBracket);
            HANDLE_SINGLE_CHAR_TOKEN('{', LBrace);   HANDLE_SINGLE_CHAR_TOKEN('}', RBrace);
            }

#           undef HANDLE_SINGLE_CHAR_TOKEN
            break;

        case JinjLexerStateParsingIdent:
            if (isalnum(c) || c == '_') {
                next(lexer);
                continue;
            }

            _jinj_lexer_add_token_with_value(lexer, JinjTokenTypeIdent,
                                             lexer->input + lexer->token_start_pos,
                                             lexer->pos - lexer->token_start_pos);
            lexer->state = JinjLexerStateDefault;
            break;


        case JinjLexerStateParsingString:
            c = next(lexer);
            if (c == '\\') { next(lexer); break; }
            if (c != '"') break;

            _jinj_lexer_add_token_with_value(
                lexer, JinjTokenTypeString,
                lexer->input + lexer->token_start_pos + 1,
                lexer->pos - lexer->token_start_pos - 2
            );
            lexer->state = JinjLexerStateDefault;
            break;

        case JinjLexerStateParsingChar:
            c = next(lexer);
            if (c == '\\') { next(lexer); break; }
            if (c != '\'') break;

            _jinj_lexer_add_token_with_value(
                lexer, JinjTokenTypeChar,
                lexer->input + lexer->token_start_pos + 1,
                lexer->pos - lexer->token_start_pos - 2
            );
            lexer->state = JinjLexerStateDefault;
            break;

        case JinjLexerStateParsingNumber:
            if (isdigit(c)) {
                next(lexer);
                break;
            }

            if (c == '.' && isdigit(peek_next(lexer))) {
                next(lexer);
                lexer->state = JinjLexerStateParsingFloat;
                break;
            }

            _jinj_lexer_add_token_with_value(
                lexer, JinjTokenTypeInt,
                lexer->input + lexer->token_start_pos,
                lexer->pos - lexer->token_start_pos
            );
            lexer->state = JinjLexerStateDefault;
            break;

        case JinjLexerStateParsingFloat:
            if (isdigit(c)) {
                next(lexer);
                break;
            }

            _jinj_lexer_add_token_with_value(
                lexer, JinjTokenTypeFloat,
                lexer->input + lexer->token_start_pos,
                lexer->pos - lexer->token_start_pos
            );
            lexer->state = JinjLexerStateDefault;
            break;

        case JinjLexerStateDone: assert("unreachable" && 0);
        }
    }

    // finalize last token if input ended in the middle of one
    switch (lexer->state) {
    case JinjLexerStateParsingIdent:
        _jinj_lexer_add_token_with_value(
            lexer, JinjTokenTypeIdent,
            lexer->input + lexer->token_start_pos,
            lexer->pos - lexer->token_start_pos
        );
        break;
    
    case JinjLexerStateParsingNumber:
        _jinj_lexer_add_token_with_value(
            lexer, JinjTokenTypeInt,
            lexer->input + lexer->token_start_pos,
            lexer->pos - lexer->token_start_pos
        );
        break;
    
    case JinjLexerStateParsingString:
        return (JinjLexerResult) {
            .code = JinjLexerErrorUnterminatedStringLiteral,
            .location = lexer->token_start_location
        };

    case JinjLexerStateParsingChar:
        return (JinjLexerResult) {
            .code = JinjLexerErrorUnterminatedCharLiteral,
            .location = lexer->token_start_location
        };

    default:
        break;
    }


    lexer->state = JinjLexerStateDone;
    return JINJ_LEXER_RESULT_SUCCESS;
}
