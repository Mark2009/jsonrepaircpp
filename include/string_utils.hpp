#pragma once

#include <string>
#include <regex>

namespace jsonrepair {

// Character code constants
constexpr int CODE_SPACE = 0x20;
constexpr int CODE_NEWLINE = 0x0A;
constexpr int CODE_TAB = 0x09;
constexpr int CODE_RETURN = 0x0D;
constexpr int CODE_NON_BREAKING_SPACE = 0xA0;
constexpr int CODE_EN_QUAD = 0x2000;
constexpr int CODE_HAIR_SPACE = 0x200A;
constexpr int CODE_NARROW_NO_BREAK_SPACE = 0x202F;
constexpr int CODE_MEDIUM_MATHEMATICAL_SPACE = 0x205F;
constexpr int CODE_IDEOGRAPHIC_SPACE = 0x3000;

/**
 * Check if a character is a hexadecimal digit
 */
bool isHex(char c);

/**
 * Check if a character is a digit (0-9)
 */
bool isDigit(char c);

/**
 * Check if a character is valid in a JSON string
 */
bool isValidStringCharacter(char c);

/**
 * Check if a character is a delimiter in JSON
 */
bool isDelimiter(char c);

/**
 * Check if a character can start a function name
 */
bool isFunctionNameCharStart(char c);

/**
 * Check if a character can be part of a function name
 */
bool isFunctionNameChar(char c);

/**
 * Check if a character is a delimiter for unquoted strings
 */
bool isUnquotedStringDelimiter(char c);

/**
 * Check if a character can start a JSON value
 */
bool isStartOfValue(char c);

/**
 * Check if a character is a control character
 */
bool isControlCharacter(char c);

/**
 * Check if a character is whitespace (space, tab, newline, or carriage return)
 */
bool isWhitespace(const std::string& text, size_t index);

/**
 * Check if a character is whitespace but not a newline
 */
bool isWhitespaceExceptNewline(const std::string& text, size_t index);

/**
 * Check if a character is a special whitespace character
 */
bool isSpecialWhitespace(const std::string& text, size_t index);

/**
 * Check if a character is any type of quote
 */
bool isQuote(char c);

/**
 * Check if a character is a double quote or double-quote-like character
 */
bool isDoubleQuoteLike(char c);

/**
 * Check if a character is a regular double quote
 */
bool isDoubleQuote(char c);

/**
 * Check if a character is a single quote or single-quote-like character
 */
bool isSingleQuoteLike(char c);

/**
 * Check if a character is a regular single quote
 */
bool isSingleQuote(char c);

/**
 * Strip the last occurrence of a substring from a string
 */
std::string stripLastOccurrence(const std::string& text, const std::string& textToStrip, bool stripRemainingText = false);

/**
 * Insert text before the last whitespace in a string
 */
std::string insertBeforeLastWhitespace(const std::string& text, const std::string& textToInsert);

/**
 * Remove characters at a specific index
 */
std::string removeAtIndex(const std::string& text, size_t start, size_t count);

/**
 * Test whether a string ends with a comma or newline and optional whitespace
 */
bool endsWithCommaOrNewline(const std::string& text);

/**
 * Get the control character escape sequence for a character
 */
std::string getControlCharacter(char c);

/**
 * Get the unescaped character for an escape sequence
 */
char getEscapeCharacter(char c);

/**
 * Check if string starts with URL schema
 */
bool startsWithUrlSchema(const std::string& text);

/**
 * Check if character is valid in a URL
 */
bool isUrlChar(char c);

} // namespace jsonrepair
