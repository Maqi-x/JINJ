/// @file token.h
/// @brief Token definitions used by the JINJ lexer.
/// @addtogroup lexer
/// @{

#ifndef JINJ_DETAIL_LEXER_TOKEN_H
#define JINJ_DETAIL_LEXER_TOKEN_H

#include <jinj/defs.h>
#include <stdio.h>

/// @enum JinjTokenType
/// @brief Represents the type of a lexical token.
/// @details Token types correspond to the different lexical elements
///          recognized by the lexer, including literals, punctuation,
///          comments, and special markers.
typedef enum JinjTokenType {
    JinjTokenTypeIdent,       ///< Identifier token.
    JinjTokenTypeString,      ///< String literal.
    JinjTokenTypeChar,        ///< Character literal.
    JinjTokenTypeInt,         ///< Integer literal.
    JinjTokenTypeFloat,       ///< Floating-point literal.
    JinjTokenTypeBool,        ///< Boolean literal (`true` or `false`).
    JinjTokenTypeNil,         ///< `nil` literal.

    JinjTokenTypeLParen,      ///< `(` symbol
    JinjTokenTypeRParen,      ///< `)` symbol
    JinjTokenTypeLBracket,    ///< `[` symbol
    JinjTokenTypeRBracket,    ///< `]` symbol
    JinjTokenTypeLBrace,      ///< `{` symbol
    JinjTokenTypeRBrace,      ///< `}` symbol

    JinjTokenTypeHash,        ///< `@` symbol.
    JinjTokenTypeComma,       ///< `,` symbol.
    JinjTokenTypeColon,       ///< `:` symbol.
    JinjTokenTypeEqual,       ///< `=` symbol.

    JinjTokenTypeLineComment, ///< Line comment.
    JinjTokenTypeBlockComment,///< Block comment.

    JinjTokenTypeWhitespace,  ///< Whitespace or indentation.

    JinjTokenTypeEOF,         ///< End-of-file marker.
} JinjTokenType;

/// @struct JinjTokenLocation
/// @brief Represents a source location of a token.
/// @details Used to track the position of tokens in the input source.
typedef struct JinjTokenLocation {
    int line;   ///< Line number of the token.
    int column; ///< Column number of the token.
} JinjTokenLocation;

/// @struct JinjTokenValue
/// @brief Represents a string value associated with a token.
/// @details Used for tokens that carry textual content such as identifiers,
///          literals, or comments.
typedef struct JinjTokenValue {
    const char* str; ///< Pointer to the token's value string.
    usize len;       ///< Length of the value in bytes.
} JinjTokenValue;

/// @struct JinjToken
/// @brief Represents a single lexical token.
/// @details Combines token type, source location, and optional value.
typedef struct JinjToken {
    JinjTokenType type;           ///< Type of the token.
    JinjTokenLocation location;   ///< Source location in the input text.
    JinjTokenValue value;         ///< Optional associated string value.
} JinjToken;

/// @brief Convert a token type to its string representation.
/// @param tt Token type.
/// @return Pointer to a null-terminated string describing the token type.
const char* jinj_token_type_to_string(JinjTokenType tt);

/// @brief Format a token into a provided buffer.
/// @param tok Token to format.
/// @param n Size of the destination buffer.
/// @param buf Output buffer (must have at least `n` bytes).
/// @return Number of bytes written (excluding null terminator).
usize jinj_format_token(JinjToken tok, usize n, char buf[static n]);

/// @brief Convert a token into a newly allocated string representation.
/// @param tok Token to convert.
/// @param out Pointer to store the allocated string.
/// @return Length of the resulting string (excluding null terminator).
usize jinj_token_to_string(JinjToken tok, char** out);

/// @brief Print a token to a file stream.
/// @param tok Token to print.
/// @param out File stream to write to.
/// @return Number of bytes written.
usize jinj_print_token(JinjToken tok, FILE* out);

/// @brief Construct a new token from type and location data.
/// @param type Token type.
/// @param line Line number.
/// @param column Column number.
/// @return Newly created token.
JinjToken jinj_make_token(JinjTokenType type, int line, int column);

/// @brief Construct a new token with an associated string value.
/// @param type Token type.
/// @param line Line number.
/// @param column Column number.
/// @param value Pointer to string value.
/// @param value_len Length of the string value.
/// @return Newly created token with value.
JinjToken jinj_make_token_with_value(JinjTokenType type, int line, int column,
                                     const char* value, usize value_len);

#endif // JINJ_DETAIL_LEXER_TOKEN_H

/// @}

