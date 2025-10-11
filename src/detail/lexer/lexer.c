#include "jinj/detail/lexer/lexer.h"
#include "jinj/detail/lexer/result.h"
#include "jinj/detail/lexer/token.h"
#include "jinj/detail/lexer/tokenlist.h"
#include <assert.h>
#include <ctype.h>
#include <jinj/detail/lexer.h>

#define peek(lexer) (lexer->input[lexer->pos])
#define next(lexer) (lexer->input[lexer->location.column++, lexer->pos++])
#define peek_next(lexer) (lexer->input[lexer->pos + 1])

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

    for (usize i = 0; i < lexer->input_len; ++i) {
        char c = next(lexer);

        switch (lexer->state) {
        case JinjLexerStateDefault:
            lexer->token_start_pos = lexer->pos;
            lexer->token_start_location = lexer->location;

            if (isspace(c)) {
                if (c == '\n') {
                    lexer->location.line++;
                    lexer->location.column = 0;
                }
                break;
            }

            if (isalpha(c) || c == '_') {
                lexer->state = JinjLexerStateParsingIdent;
                continue;
            } else if (c == '"') {
                lexer->state = JinjLexerStateParsingString;
                continue;
            } else if (c == '\'') {
                lexer->state = JinjLexerStateParsingChar;
                continue;
            } else if (isdigit(c)) {
                lexer->state = JinjLexerStateParsingNumber;
                continue;
            }

#           define HANDLE(ch, tp) case ch: _jinj_lexer_add_token(lexer, JinjTokenType##tp); break

            switch (c) {
            HANDLE('#', Hash);
            HANDLE(',', Comma);
            HANDLE(':', Colon);
            HANDLE('=', Equal);

            HANDLE('(', LParen);   HANDLE(')', RParen);
            HANDLE('[', LBracket); HANDLE(']', RBracket);
            HANDLE('{', LBrace);   HANDLE('}', RBrace);
            }

#           undef HANDLE
            break;

        case JinjLexerStateParsingIdent:
            if (isalnum(c) || c == '_') {
                continue;
            }

            _jinj_lexer_add_token_with_value(lexer, JinjTokenTypeIdent,
                                             lexer->input + lexer->token_start_pos,
                                             lexer->pos - lexer->token_start_pos);
            lexer->state = JinjLexerStateDefault;
            break;

        case JinjLexerStateParsingString:
            if (c == '\\') c = next(lexer);
            if (c != '"') continue;

            _jinj_lexer_add_token_with_value(lexer, JinjTokenTypeString,
                                             lexer->input + lexer->token_start_pos + 1,
                                             lexer->pos - lexer->token_start_pos - 1);
            lexer->state = JinjLexerStateDefault;
            break;

        case JinjLexerStateParsingChar:
            if (c == '\\') c = next(lexer);
            if (c != '\'') continue;

            _jinj_lexer_add_token_with_value(lexer, JinjTokenTypeChar,
                                             lexer->input + lexer->token_start_pos + 1,
                                             lexer->pos - lexer->token_start_pos - 1);
            lexer->state = JinjLexerStateDefault;
            break;

        case JinjLexerStateParsingNumber:
            if (isdigit(c)) {
                continue;
            }

            if (c == '.') {
                if (isdigit(peek_next(lexer))) {
                    lexer->state = JinjLexerStateParsingFloat;
                    continue;
                }
            }

            // finalize integer
            _jinj_lexer_add_token_with_value(
                lexer, JinjTokenTypeInt,
                lexer->input + lexer->token_start_pos,
                lexer->pos - lexer->token_start_pos
            );

            lexer->state = JinjLexerStateDefault;
            // reprocess current char
            lexer->pos--;
            lexer->location.column--;
            break;

        case JinjLexerStateParsingFloat:
            if (isdigit(c)) {
                continue;
            }

            _jinj_lexer_add_token_with_value(
                lexer, JinjTokenTypeFloat,
                lexer->input + lexer->token_start_pos,
                lexer->pos - lexer->token_start_pos
            );

            lexer->state = JinjLexerStateDefault;
            lexer->pos--;
            lexer->location.column--;
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
