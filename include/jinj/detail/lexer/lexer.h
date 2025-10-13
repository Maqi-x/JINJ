/// @file lexer.h
/// @brief Core JINJ lexer definition and API.

/// @defgroup lexer Lexer
/// @brief JINJ lexical analyzer module.
/// @details This module provides tokenization of input text into structured tokens.
/// @{

#ifndef JINJ_DETAIL_LEXER_LEXER_H
#define JINJ_DETAIL_LEXER_LEXER_H

#include "result.h"
#include "token.h"
#include "tokenlist.h"

/// @enum JinjLexerFlags
/// @brief Represents configuration flags controlling lexer behavior.
/// These flags modify how the lexer processes the input stream. They can be combined using bitwise OR.
/// @note Unused bits are reserved for future extensions.
typedef enum JinjLexerFlags {
    JinjLexerFlagsNone             = 0,

    JinjLexerSaveComments          = 1 << 0,
    JinjLexerAllowUnterminated     = 1 << 1,
    JinjLexerTrimWhitespace        = 1 << 2,
    JinjLexerAllowUtf8Idents       = 1 << 3,
    JinjLexerSkipUnknown           = 1 << 4,
} JinjLexerFlags;

/// @def JINJ_LEXER_FLAGS_DEFAULT
/// @brief Default lexer configuration flags.
/// @details By default, the lexer saves comments, trims whitespace,
///          and allows UTF-8 identifiers.
#define JINJ_LEXER_FLAGS_DEFAULT \
    (JinjLexerSaveComments | JinjLexerTrimWhitespace | JinjLexerAllowUtf8Idents)

/// @enum JinjLexerState
/// @brief Represents the internal state of the lexer.
/// @warning These states are used internally and should not be modified directly.
typedef enum JinjLexerState {
    JinjLexerStateDefault = 0,         ///< Idle/default lexer state.

    JinjLexerStateParsingIdent,        ///< Currently parsing an identifier.
    JinjLexerStateParsingNumber,       ///< Currently parsing an integer literal.
    JinjLexerStateParsingFloat,        ///< Currently parsing a floating-point literal.
    JinjLexerStateParsingChar,         ///< Currently parsing a character literal.
    JinjLexerStateParsingString,       ///< Currently parsing a string literal.

    JinjLexerStateParsingLineComment,  ///< Inside a line comment.
    JinjLexerStateParsingBlockComment, ///< Inside a block comment.

    JinjLexerStateParsingWhitespace,   ///< Processing whitespace between tokens.

    JinjLexerStateDone                 ///< Lexer finished processing input.
} JinjLexerState;

/// @struct JinjLexer
/// @brief Holds all lexer runtime data and state.
/// @details This structure encapsulates all lexer-related state,
///          including input buffer, current position, token list, and flags.
/// @see JinjToken
typedef struct JinjLexer {
    JinjLexerFlags flags;                   ///< Active lexer configuration flags.

    const char* input;                      ///< Input source text buffer.
    jinj_usize_t input_len;                        ///< Length of the input buffer in bytes.

    JinjTokenLocation location;             ///< Current location in the input stream.
    jinj_usize_t pos;                              ///< @internal Current byte offset in the input buffer.
    JinjTokenLocation token_start_location; ///< Start location of the current token.
    jinj_usize_t token_start_pos;                  ///< @internal Start position index of the current token.

    JinjLexerState state;                   ///< @internal Current lexer state.

    JinjTokenList tokens;                   ///< Collected tokens.
} JinjLexer;

/// @brief Initialize a lexer instance.
/// @param lexer Pointer to the lexer.
/// @param input Pointer to input string buffer.
/// @param input_len Input length in bytes.
/// @param flags Lexer configuration flags.
void jinj_lexer_init(JinjLexer* lexer, const char* input, jinj_usize_t input_len, JinjLexerFlags flags);

/// @brief Free resources associated with a lexer.
/// @param lexer Pointer to the lexer.
void jinj_lexer_deinit(JinjLexer* lexer);

/// @brief Execute the lexer and tokenize the input.
/// @param lexer Pointer to the lexer.
/// @return Lexer result with success or error code.
/// @details The tokenization result is stored in lexer->tokens.
JinjLexerResult jinj_lexer_tokenize(JinjLexer* lexer);

/// @internal
/// @brief Add a token to the lexer's token list.
JinjLexerResult _jinj_lexer_add_token(JinjLexer* lexer, JinjTokenType type);

/// @internal
/// @brief Add a token with a string value to the lexer's token list.
JinjLexerResult _jinj_lexer_add_token_with_value(JinjLexer* lexer, JinjTokenType type,
                                                 const char* value, jinj_usize_t value_len);

/// @internal
/// @brief Determine whether an identifier is a keyword or a regular identifier.
/// @param lexer Pointer to the active lexer instance.
/// @param value Pointer to the identifier text.
/// @param value_len Length of the identifier text in bytes.
/// @return The corresponding token type: `JinjTokenTypeTrueLit`, `JinjTokenTypeFalseLit`,
///         `JinjTokenTypeNilLit`, or `JinjTokenTypeIdent`.
/// @details This function checks whether the given identifier matches any
///          reserved keywords (`true`, `false`, or `nil`). If not, it defaults
///          to returning `JinjTokenTypeIdent`.
JinjTokenType _jinj_lexer_get_keyword_or_ident_type(JinjLexer* lexer, const char* value, jinj_usize_t value_len);

#endif // JINJ_DETAIL_LEXER_LEXER_H

/// @}
