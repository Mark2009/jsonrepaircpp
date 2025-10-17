#include "string_utils.hpp"
#include <algorithm>
#include <cctype>
#include <regex>

namespace jsonrepair {

bool isHex(char c) {
    return (c >= '0' && c <= '9') || 
           (c >= 'A' && c <= 'F') || 
           (c >= 'a' && c <= 'f');
}

bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool isValidStringCharacter(char c) {
    // Valid range is between \u{0020} and higher
    return static_cast<unsigned char>(c) >= 0x20;
}

bool isDelimiter(char c) {
    return c == ',' || c == ':' || c == '[' || c == ']' || 
           c == '/' || c == '{' || c == '}' || c == '(' || 
           c == ')' || c == '\n' || c == '+';
}

bool isFunctionNameCharStart(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || 
           c == '_' || c == '$';
}

bool isFunctionNameChar(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || 
           c == '_' || c == '$' || (c >= '0' && c <= '9');
}

bool isUnquotedStringDelimiter(char c) {
    return c == ',' || c == '[' || c == ']' || c == '/' || 
           c == '{' || c == '}' || c == '\n' || c == '+';
}

bool isStartOfValue(char c) {
    if (isQuote(c)) return true;
    // alpha, number, minus, or opening bracket or brace
    return c == '[' || c == '{' || c == '-' || 
           (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || 
           (c >= '0' && c <= '9') || c == '_';
}

bool isControlCharacter(char c) {
    return c == '\n' || c == '\r' || c == '\t' || c == '\b' || c == '\f';
}

bool isWhitespace(const std::string& text, size_t index) {
    if (index >= text.length()) return false;
    int code = static_cast<unsigned char>(text[index]);
    return code == CODE_SPACE || code == CODE_NEWLINE || 
           code == CODE_TAB || code == CODE_RETURN;
}

bool isWhitespaceExceptNewline(const std::string& text, size_t index) {
    if (index >= text.length()) return false;
    int code = static_cast<unsigned char>(text[index]);
    return code == CODE_SPACE || code == CODE_TAB || code == CODE_RETURN;
}

bool isSpecialWhitespace(const std::string& text, size_t index) {
    if (index >= text.length()) return false;
    
    // Handle multi-byte UTF-8 characters
    unsigned char first = static_cast<unsigned char>(text[index]);
    
    // Non-breaking space (0xA0) in UTF-8 is 0xC2 0xA0
    if (first == 0xC2 && index + 1 < text.length()) {
        unsigned char second = static_cast<unsigned char>(text[index + 1]);
        if (second == 0xA0) return true;
    }
    
    // Other special spaces (0x2000-0x200A, 0x202F, 0x205F, 0x3000)
    // These are encoded in UTF-8 as multi-byte sequences
    if (first == 0xE2 && index + 2 < text.length()) {
        unsigned char second = static_cast<unsigned char>(text[index + 1]);
        unsigned char third = static_cast<unsigned char>(text[index + 2]);
        
        // 0x2000-0x200A: E2 80 80 to E2 80 8A
        if (second == 0x80 && third >= 0x80 && third <= 0x8A) return true;
        // 0x202F: E2 80 AF
        if (second == 0x80 && third == 0xAF) return true;
        // 0x205F: E2 81 9F
        if (second == 0x81 && third == 0x9F) return true;
    }
    
    // 0x3000: E3 80 80
    if (first == 0xE3 && index + 2 < text.length()) {
        unsigned char second = static_cast<unsigned char>(text[index + 1]);
        unsigned char third = static_cast<unsigned char>(text[index + 2]);
        if (second == 0x80 && third == 0x80) return true;
    }
    
    return false;
}

bool isDoubleQuoteLike(char c) {
    // Regular double quote and special variants
    return c == '"' || c == '\u201c' || c == '\u201d';
}

bool isDoubleQuote(char c) {
    return c == '"';
}

bool isSingleQuoteLike(char c) {
    return c == '\'' || c == '\u2018' || c == '\u2019' || 
           c == '\u0060' || c == '\u00b4';
}

bool isSingleQuote(char c) {
    return c == '\'';
}

bool isQuote(char c) {
    return isDoubleQuoteLike(c) || isSingleQuoteLike(c);
}

std::string stripLastOccurrence(const std::string& text, const std::string& textToStrip, bool stripRemainingText) {
    size_t index = text.rfind(textToStrip);
    if (index != std::string::npos) {
        if (stripRemainingText) {
            return text.substr(0, index);
        } else {
            return text.substr(0, index) + text.substr(index + textToStrip.length());
        }
    }
    return text;
}

std::string insertBeforeLastWhitespace(const std::string& text, const std::string& textToInsert) {
    size_t index = text.length();
    
    if (index == 0 || !isWhitespace(text, index - 1)) {
        return text + textToInsert;
    }
    
    while (index > 0 && isWhitespace(text, index - 1)) {
        index--;
    }
    
    return text.substr(0, index) + textToInsert + text.substr(index);
}

std::string removeAtIndex(const std::string& text, size_t start, size_t count) {
    return text.substr(0, start) + text.substr(start + count);
}

bool endsWithCommaOrNewline(const std::string& text) {
    static std::regex pattern(R"([,\n][ \t\r]*$)");
    return std::regex_search(text, pattern);
}

std::string getControlCharacter(char c) {
    switch (c) {
        case '\b': return "\\b";
        case '\f': return "\\f";
        case '\n': return "\\n";
        case '\r': return "\\r";
        case '\t': return "\\t";
        default: return std::string(1, c);
    }
}

char getEscapeCharacter(char c) {
    switch (c) {
        case '"': return '"';
        case '\\': return '\\';
        case '/': return '/';
        case 'b': return '\b';
        case 'f': return '\f';
        case 'n': return '\n';
        case 'r': return '\r';
        case 't': return '\t';
        default: return '\0';
    }
}

bool startsWithUrlSchema(const std::string& text) {
    static std::regex urlStartPattern(R"(^(http|https|ftp|mailto|file|data|irc)://)");
    return std::regex_search(text, urlStartPattern);
}

bool isUrlChar(char c) {
    static std::string validUrlChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#@!$&'()*+;=";
    return validUrlChars.find(c) != std::string::npos;
}

} // namespace jsonrepair
