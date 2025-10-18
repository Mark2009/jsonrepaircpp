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
    // Allow all high-bit characters (0x80-0xFF) which are part of UTF-8 multi-byte sequences
    unsigned char uc = static_cast<unsigned char>(c);
    return uc >= 0x20 || uc >= 0x80;
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

std::string fixUtf8Encoding(const std::string& text) {
    std::string result;
    result.reserve(text.size());
    
    for (size_t i = 0; i < text.size(); ) {
        unsigned char c = static_cast<unsigned char>(text[i]);
        
        // Common Windows-1252 to UTF-8 mojibake patterns
        // â€™ (0xC3 0xA2 0xE2 0x82 0xAC 0xE2 0x84 0xA2) -> ' (0xE2 0x80 0x99)
        if (i + 8 < text.size() && 
            c == 0xC3 && static_cast<unsigned char>(text[i+1]) == 0xA2 &&
            static_cast<unsigned char>(text[i+2]) == 0xE2 && 
            static_cast<unsigned char>(text[i+3]) == 0x82 &&
            static_cast<unsigned char>(text[i+4]) == 0xAC &&
            static_cast<unsigned char>(text[i+5]) == 0xE2 &&
            static_cast<unsigned char>(text[i+6]) == 0x84 &&
            static_cast<unsigned char>(text[i+7]) == 0xA2) {
            // Right single quotation mark (U+2019)
            result += "\xE2\x80\x99";
            i += 8;
        }
        // â€˜ -> ' (left single quotation mark)
        else if (i + 8 < text.size() && 
                 c == 0xC3 && static_cast<unsigned char>(text[i+1]) == 0xA2 &&
                 static_cast<unsigned char>(text[i+2]) == 0xE2 && 
                 static_cast<unsigned char>(text[i+3]) == 0x82 &&
                 static_cast<unsigned char>(text[i+4]) == 0xAC &&
                 static_cast<unsigned char>(text[i+5]) == 0xCB &&
                 static_cast<unsigned char>(text[i+6]) == 0x9C) {
            // Left single quotation mark (U+2018)
            result += "\xE2\x80\x98";
            i += 7;
        }
        // â€œ -> " (left double quotation mark)
        else if (i + 8 < text.size() && 
                 c == 0xC3 && static_cast<unsigned char>(text[i+1]) == 0xA2 &&
                 static_cast<unsigned char>(text[i+2]) == 0xE2 && 
                 static_cast<unsigned char>(text[i+3]) == 0x82 &&
                 static_cast<unsigned char>(text[i+4]) == 0xAC &&
                 static_cast<unsigned char>(text[i+5]) == 0xC5 &&
                 static_cast<unsigned char>(text[i+6]) == 0x93) {
            // Left double quotation mark (U+201C)
            result += "\xE2\x80\x9C";
            i += 7;
        }
        // â€ -> " (right double quotation mark)
        else if (i + 6 < text.size() && 
                 c == 0xC3 && static_cast<unsigned char>(text[i+1]) == 0xA2 &&
                 static_cast<unsigned char>(text[i+2]) == 0xE2 && 
                 static_cast<unsigned char>(text[i+3]) == 0x82 &&
                 static_cast<unsigned char>(text[i+4]) == 0xAC &&
                 static_cast<unsigned char>(text[i+5]) == 0xC5) {
            // Right double quotation mark (U+201D)
            result += "\xE2\x80\x9D";
            i += 6;
        }
        // Simple fallback: just keep the character as-is
        else {
            result += text[i];
            i++;
        }
    }
    
    return result;
}

} // namespace jsonrepair
