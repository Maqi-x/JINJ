/// @file result.h
/// @brief Contains JINJ lexer result type and error codes.
/// @addtogroup lexer
/// @{

#ifndef JINJ_DETAIL_LEXER_RESULT_H
#define JINJ_DETAIL_LEXER_RESULT_H

#include "token.h"

/// @brief Represents the result of a lexer operation.
/// @details
/// Used by lexer functions to indicate success or specific error conditions.
typedef struct JinjLexerResult {
    /// @brief Lexer error codes.
    enum JinjLexerErrorCode {
        JinjLexerSuccess = 0,      ///< Operation completed successfully.

        JinjLexerErrorAllocFailed, ///< Memory allocation failed during lexing.

        /// Unknown or invalid character encountered in input.
        /// The unexpected character is stored in @ref error_details.ch
        JinjLexerErrorUnknownChar,

        /// @brief Unterminated string literal.
        /// @warning This error is not reported when the lexer is configured
        ///          with the @ref JinjLexerAllowUnterminated flag.
        JinjLexerErrorUnterminatedStringLiteral,

        /// @brief Unterminated character literal.
        /// @warning This error is not reported when the lexer is configured
        ///         with the @ref JinjLexerAllowUnterminated flag.
        JinjLexerErrorUnterminatedCharLiteral,

        /// @brief Unterminated block comment.
        /// @warning This error is not reported when the lexer is configured
        ///          with the @ref JinjLexerAllowUnterminated flag.
        JinjLexerErrorUnterminatedBlockComment,
    } code;

    /// Source location where the error occurred.
    /// Maybe uninitialized if @ref code is JinjLexerSuccess.
    JinjTokenLocation location;
    union {
        char ch;
        void* other;
    } error_details;
} JinjLexerResult;

/// @def JINJ_LEXER_RESULT_SUCCESS
/// @brief Predefined successful lexer result.
/// @hideinitializer
#define JINJ_LEXER_RESULT_SUCCESS ((JinjLexerResult) { .code = JinjLexerSuccess, .error_details.other = NULL })

#endif // JINJ_DETAIL_LEXER_RESULT_H

/// @}
