#include <jinj/detail/lexer.h>

#include <ctype.h>
#include <assert.h>
#include <stdbool.h>

#define _jinj_lexer_unexpected_char(lexer, c) \
    do { \
        if (!(lexer->flags & JinjLexerSkipUnknown)) { \
            return (JinjLexerResult) { \
                .code = JinjLexerErrorUnknownChar, \
                .location = lexer->location, \
                .error_details.ch = c, \
            }; \
        } \
    } while (0)

#define JINJ_LEXER_LINE_COMMENT_PREFIX_LEN 2 // '//'
#define JINJ_LEXER_LINE_COMMENT_SUFFIX_LEN 1  // '\n'

#define JINJ_LEXER_BLOCK_COMMENT_PREFIX_LEN 2 // '/*'
#define JINJ_LEXER_BLOCK_COMMENT_SUFFIX_LEN 2  // '*/'

#define JINJ_LEXER_STRING_LITERAL_PREFIX_LEN 1 // "
#define JINJ_LEXER_STRING_LITERAL_SUFFIX_LEN 1 // "

#define JINJ_LEXER_MULTILINE_STRING_LITERAL_PREFIX_LEN 3 // """
#define JINJ_LEXER_MULTILINE_STRING_LITERAL_SUFFIX_LEN 3 // """

#define JINJ_LEXER_CHAR_LITERAL_PREFIX_LEN 1 // "
#define JINJ_LEXER_CHAR_LITERAL_SUFFIX_LEN 1 // "

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

    // for windows
    if (c == '\r') {
        if (peek(lexer) == '\n') lexer->pos++; // skip '\n' in CRLF
        lexer->location.line++;
        lexer->location.column = 0;
    } else if (c == '\n') {
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
            lexer->token_start_location.line, lexer->token_start_location.column,
            value, value_len);
}

void jinj_lexer_init(JinjLexer *lexer, const char *input, usize input_len, JinjLexerFlags flags) {
    jinj_token_list_init(&lexer->tokens);
    lexer->input = input;
    lexer->input_len = input_len;

    lexer->location = (JinjTokenLocation) { 0, 0 };
    lexer->pos = 0;

    lexer->token_start_location = (JinjTokenLocation) { 0, 0 };
    lexer->token_start_pos = 0;
    
    lexer->flags = flags;
}

void jinj_lexer_deinit(JinjLexer *lexer) {
    jinj_token_list_deinit(&lexer->tokens);
}

JinjLexerResult jinj_lexer(JinjLexer* lexer) {
    lexer->state = JinjLexerStateDefault;

    while (lexer->pos < lexer->input_len) {
        char c = peek(lexer);

        switch (lexer->state) {
        case JinjLexerStateDefault: {
            if (isspace(c)) { next(lexer); break; }

            lexer->token_start_pos = lexer->pos;
            lexer->token_start_location = lexer->location;
            c = next(lexer);

            if (c == '/') {
                if (peek(lexer) == '/') {
                    next(lexer);
                    lexer->state = JinjLexerStateParsingLineComment;
                    break;
                } else if (peek(lexer) == '*') {
                    next(lexer);
                    lexer->state = JinjLexerStateParsingBlockComment;
                    break;
                } else {
                    _jinj_lexer_unexpected_char(lexer, '/');
                    break;
                }
            }

            bool is_ident_start = isalpha(c) || c == '_';
            if (!is_ident_start && (lexer->flags & JinjLexerAllowUtf8Idents)) 
                is_ident_start = (unsigned char)c >= 0x80;

            if (is_ident_start) {
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

#           define HANDLE_SINGLE_CHAR_TOKEN(ch, tp) case ch: _jinj_lexer_add_token(lexer, JinjTokenType##tp); goto continue_lexer_loop

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

            _jinj_lexer_unexpected_char(lexer, c);

        continue_lexer_loop:
            break;
        }

        case JinjLexerStateParsingLineComment:
            c = next(lexer);
            if (c == '\n' || c == '\0') {
                if (lexer->flags & JinjLexerSaveComments) {
                    _jinj_lexer_add_token_with_value(
                        lexer, JinjTokenTypeLineComment,
                        lexer->input + lexer->token_start_pos + JINJ_LEXER_LINE_COMMENT_PREFIX_LEN,
                        lexer->pos - lexer->token_start_pos
                            - JINJ_LEXER_LINE_COMMENT_PREFIX_LEN
                            - JINJ_LEXER_LINE_COMMENT_SUFFIX_LEN
                    );
                }
                lexer->state = JinjLexerStateDefault;
            }
            break;
        
        case JinjLexerStateParsingBlockComment:
            c = next(lexer);
            if (c == '\0' && !(lexer->flags & JinjLexerAllowUnterminated)) {
                return (JinjLexerResult) {
                    .code = JinjLexerErrorUnterminatedBlockComment,
                    .location = lexer->token_start_location
                };
            }
        
            if (c == '*' && peek(lexer) == '/') {
                next(lexer);
                if (lexer->flags & JinjLexerSaveComments) {
                    _jinj_lexer_add_token_with_value(
                        lexer, JinjTokenTypeBlockComment,
                        lexer->input + lexer->token_start_pos + JINJ_LEXER_BLOCK_COMMENT_PREFIX_LEN,
                        lexer->pos - lexer->token_start_pos
                            - JINJ_LEXER_BLOCK_COMMENT_PREFIX_LEN
                            - JINJ_LEXER_BLOCK_COMMENT_SUFFIX_LEN
                    );
                }
                lexer->state = JinjLexerStateDefault;
            }
            break;

        case JinjLexerStateParsingIdent: {
            bool is_ident = isalnum(c) || c == '_';
            if (!is_ident && (lexer->flags & JinjLexerAllowUtf8Idents)) 
                is_ident = (unsigned char)c >= 0x80;

            if (is_ident) {
                next(lexer);
                continue;
            }

            _jinj_lexer_add_token_with_value(lexer, JinjTokenTypeIdent,
                                             lexer->input + lexer->token_start_pos,
                                             lexer->pos - lexer->token_start_pos);
            lexer->state = JinjLexerStateDefault;
            break;
        }

        case JinjLexerStateParsingString:
            c = next(lexer);
            if (c == '\\') { next(lexer); break; }
            if (c != '"') break;

            _jinj_lexer_add_token_with_value(
                lexer, JinjTokenTypeString,
                lexer->input + lexer->token_start_pos + JINJ_LEXER_STRING_LITERAL_PREFIX_LEN,
                lexer->pos - lexer->token_start_pos
                    - JINJ_LEXER_STRING_LITERAL_PREFIX_LEN
                    - JINJ_LEXER_STRING_LITERAL_SUFFIX_LEN
            );
            lexer->state = JinjLexerStateDefault;
            break;

        case JinjLexerStateParsingChar:
            c = next(lexer);
            if (c == '\\') { next(lexer); break; }
            if (c != '\'') break;

            _jinj_lexer_add_token_with_value(
                lexer, JinjTokenTypeChar,
                lexer->input + lexer->token_start_pos + JINJ_LEXER_CHAR_LITERAL_PREFIX_LEN,
                lexer->pos - lexer->token_start_pos
                    - JINJ_LEXER_CHAR_LITERAL_PREFIX_LEN
                    - JINJ_LEXER_CHAR_LITERAL_SUFFIX_LEN
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

    case JinjLexerStateParsingFloat:
        _jinj_lexer_add_token_with_value(
            lexer, JinjTokenTypeFloat,
            lexer->input + lexer->token_start_pos,
            lexer->pos - lexer->token_start_pos
        );
        break;
    
    case JinjLexerStateParsingString:
        if (lexer->flags & JinjLexerAllowUnterminated) break;
        return (JinjLexerResult) {
            .code = JinjLexerErrorUnterminatedStringLiteral,
            .location = lexer->token_start_location,
            .error_details.other = NULL,
        };

    case JinjLexerStateParsingChar:
        if (lexer->flags & JinjLexerAllowUnterminated) break;
        return (JinjLexerResult) {
            .code = JinjLexerErrorUnterminatedCharLiteral,
            .location = lexer->token_start_location,
            .error_details.other = NULL,
        };
        

    default:
        break;
    }


    lexer->state = JinjLexerStateDone;
    return JINJ_LEXER_RESULT_SUCCESS;
}
