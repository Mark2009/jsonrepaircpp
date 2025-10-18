#include "jsonrepair.hpp"
#include "string_utils.hpp"
#include <sstream>
#include <iomanip>

namespace jsonrepair {

class JSONRepairImpl {
public:
    JSONRepairImpl(const std::string& text) : text_(text), i_(0) {}
    
    std::string repair() {
        parseMarkdownCodeBlock({"```", "[```", "{```"});
        
        bool processed = parseValue();
        if (!processed) {
            throwUnexpectedEnd();
        }
        
        parseMarkdownCodeBlock({"```", "```]", "```}"});
        
        bool processedComma = parseCharacter(',');
        if (processedComma) {
            parseWhitespaceAndSkipComments();
        }
        
        // Check if there's another value following (newline-delimited JSON)
        // This handles cases like: {"a":1}\n{"b":2} or {"a":1} {"b":2}
        if (i_ < text_.length() && isStartOfValue(text_[i_])) {
            // If we have another value, we need to wrap everything in an array
            if (!processedComma) {
                output_ = insertBeforeLastWhitespace(output_, ",");
            }
            parseNewlineDelimitedJSON();
        } else if (processedComma) {
            // Remove trailing comma
            output_ = stripLastOccurrence(output_, ",");
        }
        
        // Remove redundant end quotes
        while (i_ < text_.length() && (text_[i_] == '}' || text_[i_] == ']')) {
            i_++;
            parseWhitespaceAndSkipComments();
        }
        
        if (i_ >= text_.length()) {
            return output_;
        }
        
        throwUnexpectedCharacter();
        return output_; // Never reached
    }

private:
    std::string text_;
    size_t i_;
    std::string output_;
    
    bool parseValue() {
        parseWhitespaceAndSkipComments();
        bool processed = parseObject() || parseArray() || parseString() || 
                        parseNumber() || parseKeywords() || 
                        parseUnquotedString(false) || parseRegex();
        parseWhitespaceAndSkipComments();
        return processed;
    }
    
    bool parseWhitespaceAndSkipComments(bool skipNewline = true) {
        size_t start = i_;
        bool changed = parseWhitespace(skipNewline);
        
        do {
            changed = parseComment();
            if (changed) {
                changed = parseWhitespace(skipNewline);
            }
        } while (changed);
        
        return i_ > start;
    }
    
    bool parseWhitespace(bool skipNewline) {
        std::string whitespace;
        
        while (i_ < text_.length()) {
            if (skipNewline ? isWhitespace(text_, i_) : isWhitespaceExceptNewline(text_, i_)) {
                // Only add the whitespace character if it's a space or tab (not newline/return)
                // Or if we're explicitly keeping newlines
                char c = text_[i_];
                if (c == ' ' || c == '\t' || !skipNewline) {
                    whitespace += c;
                }
                i_++;
            } else if (isSpecialWhitespace(text_, i_)) {
                whitespace += ' ';
                i_++;
            } else {
                break;
            }
        }
        
        if (!whitespace.empty()) {
            output_ += whitespace;
            return true;
        }
        return false;
    }
    
    bool parseComment() {
        // Block comment /* ... */
        if (i_ < text_.length() - 1 && text_[i_] == '/' && text_[i_ + 1] == '*') {
            while (i_ < text_.length() && !atEndOfBlockComment()) {
                i_++;
            }
            i_ += 2;
            return true;
        }
        
        // Line comment // ...
        if (i_ < text_.length() - 1 && text_[i_] == '/' && text_[i_ + 1] == '/') {
            while (i_ < text_.length() && text_[i_] != '\n') {
                i_++;
            }
            return true;
        }
        
        return false;
    }
    
    bool atEndOfBlockComment() {
        return i_ < text_.length() - 1 && text_[i_] == '*' && text_[i_ + 1] == '/';
    }
    
    bool parseMarkdownCodeBlock(const std::vector<std::string>& blocks) {
        size_t startOutput = output_.length();
        
        if (skipMarkdownCodeBlock(blocks)) {
            if (i_ < text_.length() && isFunctionNameCharStart(text_[i_])) {
                while (i_ < text_.length() && isFunctionNameChar(text_[i_])) {
                    i_++;
                }
            }
            
            // Remove any whitespace that was added from the markdown block
            output_ = output_.substr(0, startOutput);
            
            parseWhitespaceAndSkipComments();
            return true;
        }
        return false;
    }
    
    bool skipMarkdownCodeBlock(const std::vector<std::string>& blocks) {
        size_t start = i_;
        parseWhitespace(true);
        
        for (const auto& block : blocks) {
            if (i_ + block.length() <= text_.length() && 
                text_.substr(i_, block.length()) == block) {
                i_ += block.length();
                return true;
            }
        }
        
        // Restore position if no block was found
        i_ = start;
        return false;
    }
    
    bool parseCharacter(char c) {
        if (i_ < text_.length() && text_[i_] == c) {
            output_ += text_[i_];
            i_++;
            return true;
        }
        return false;
    }
    
    bool skipCharacter(char c) {
        if (i_ < text_.length() && text_[i_] == c) {
            i_++;
            return true;
        }
        return false;
    }
    
    bool skipEllipsis() {
        parseWhitespaceAndSkipComments();
        
        if (i_ + 2 < text_.length() && text_[i_] == '.' && 
            text_[i_ + 1] == '.' && text_[i_ + 2] == '.') {
            i_ += 3;
            parseWhitespaceAndSkipComments();
            skipCharacter(',');
            return true;
        }
        return false;
    }
    
    bool parseObject() {
        if (i_ >= text_.length() || text_[i_] != '{') {
            return false;
        }
        
        output_ += '{';
        i_++;
        parseWhitespaceAndSkipComments();
        
        // Skip leading comma
        if (skipCharacter(',')) {
            parseWhitespaceAndSkipComments();
        }
        
        bool initial = true;
        while (i_ < text_.length()) {
            // Check for premature } that's actually a typo
            // If we see }, lookahead to see if there's another property after it
            if (text_[i_] == '}') {
                size_t lookahead = i_ + 1;
                while (lookahead < text_.length() && isWhitespace(text_, lookahead)) {
                    lookahead++;
                }
                // If next char is a quote (another property), this } is wrong - skip it
                if (lookahead < text_.length() && isQuote(text_[lookahead])) {
                    i_++; // Skip the erroneous }
                    parseWhitespaceAndSkipComments();
                    // Continue to process the next property
                } else {
                    // This is the real closing brace
                    break;
                }
            }
            
            bool processedComma;
            if (!initial) {
                processedComma = parseCharacter(',');
                if (!processedComma) {
                    output_ = insertBeforeLastWhitespace(output_, ",");
                }
                parseWhitespaceAndSkipComments();
            } else {
                processedComma = true;
                initial = false;
            }
            
            skipEllipsis();
            
            bool processedKey = parseString() || parseUnquotedString(true);
            if (!processedKey) {
                if (i_ >= text_.length() || text_[i_] == '}' || text_[i_] == '{' || 
                    text_[i_] == ']' || text_[i_] == '[') {
                    output_ = stripLastOccurrence(output_, ",");
                } else {
                    throwObjectKeyExpected();
                }
                break;
            }
            
            parseWhitespaceAndSkipComments();
            bool processedColon = parseCharacter(':');
            bool truncatedText = i_ >= text_.length();
            
            if (!processedColon) {
                if (i_ < text_.length() && (isStartOfValue(text_[i_]) || truncatedText)) {
                    output_ = insertBeforeLastWhitespace(output_, ":");
                } else {
                    throwColonExpected();
                }
            }
            
            bool processedValue = parseValue();
            if (!processedValue) {
                if (processedColon || truncatedText) {
                    output_ += "null";
                } else {
                    throwColonExpected();
                }
            }
        }
        
        if (i_ < text_.length() && text_[i_] == '}') {
            output_ += '}';
            i_++;
        } else {
            output_ = insertBeforeLastWhitespace(output_, "}");
        }
        
        return true;
    }
    
    bool parseArray() {
        if (i_ >= text_.length() || text_[i_] != '[') {
            return false;
        }
        
        output_ += '[';
        i_++;
        parseWhitespaceAndSkipComments();
        
        // Skip leading comma
        if (skipCharacter(',')) {
            parseWhitespaceAndSkipComments();
        }
        
        bool initial = true;
        while (i_ < text_.length() && text_[i_] != ']') {
            if (!initial) {
                bool processedComma = parseCharacter(',');
                if (!processedComma) {
                    output_ = insertBeforeLastWhitespace(output_, ",");
                }
            } else {
                initial = false;
            }
            
            skipEllipsis();
            
            bool processedValue = parseValue();
            if (!processedValue) {
                output_ = stripLastOccurrence(output_, ",");
                break;
            }
        }
        
        if (i_ < text_.length() && text_[i_] == ']') {
            output_ += ']';
            i_++;
        } else {
            output_ = insertBeforeLastWhitespace(output_, "]");
        }
        
        return true;
    }
    
    void parseNewlineDelimitedJSON() {
        bool initial = true;
        bool processedValue = true;
        
        while (processedValue) {
            if (!initial) {
                bool processedComma = parseCharacter(',');
                if (!processedComma) {
                    output_ = insertBeforeLastWhitespace(output_, ",");
                }
            } else {
                initial = false;
            }
            
            processedValue = parseValue();
        }
        
        if (!processedValue) {
            output_ = stripLastOccurrence(output_, ",");
        }
        
        output_ = "[\n" + output_ + "\n]";
    }
    
    bool parseString(bool stopAtDelimiter = false, size_t stopAtIndex = std::string::npos) {
        if (i_ >= text_.length()) return false;
        
        bool skipEscapeChars = text_[i_] == '\\';
        if (skipEscapeChars) {
            i_++;
            skipEscapeChars = true;
        }
        
        if (i_ >= text_.length() || !isQuote(text_[i_])) {
            return false;
        }
        
        char quoteChar = text_[i_];
        size_t iBefore = i_;
        size_t oBefore = output_.length();
        
        std::string str = "\"";
        i_++;
        
        while (true) {
            if (i_ >= text_.length()) {
                // Missing end quote
                size_t iPrev = prevNonWhitespaceIndex(i_ - 1);
                if (!stopAtDelimiter && iPrev < text_.length() && isDelimiter(text_[iPrev])) {
                    i_ = iBefore;
                    output_ = output_.substr(0, oBefore);
                    return parseString(true);
                }
                
                str = insertBeforeLastWhitespace(str, "\"");
                output_ += str;
                return true;
            }
            
            if (stopAtIndex != std::string::npos && i_ == stopAtIndex) {
                str = insertBeforeLastWhitespace(str, "\"");
                output_ += str;
                return true;
            }
            
            // Check for end quote
            bool isEndQuote = (isDoubleQuote(quoteChar) && isDoubleQuote(text_[i_])) ||
                             (isSingleQuote(quoteChar) && isSingleQuote(text_[i_])) ||
                             (isSingleQuoteLike(quoteChar) && isSingleQuoteLike(text_[i_])) ||
                             (isDoubleQuoteLike(quoteChar) && isDoubleQuoteLike(text_[i_]));
            
            if (isEndQuote) {
                size_t iQuote = i_;
                size_t oQuote = str.length();
                str += "\"";
                i_++;
                output_ += str;
                
                parseWhitespaceAndSkipComments(false);
                
                if (stopAtDelimiter || i_ >= text_.length() || isDelimiter(text_[i_]) ||
                    isQuote(text_[i_]) || isDigit(text_[i_])) {
                    parseConcatenatedString();
                    return true;
                }
                
                size_t iPrevChar = prevNonWhitespaceIndex(iQuote - 1);
                if (iPrevChar < text_.length()) {
                    char prevChar = text_[iPrevChar];
                    
                    if (prevChar == ',') {
                        i_ = iBefore;
                        output_ = output_.substr(0, oBefore);
                        return parseString(false, iPrevChar);
                    }
                    
                    if (isDelimiter(prevChar)) {
                        i_ = iBefore;
                        output_ = output_.substr(0, oBefore);
                        return parseString(true);
                    }
                }
                
                output_ = output_.substr(0, oBefore);
                i_ = iQuote + 1;
                str = str.substr(0, oQuote) + "\\" + str.substr(oQuote);
                
            } else if (stopAtDelimiter && isUnquotedStringDelimiter(text_[i_])) {
                // Check for URL
                if (i_ > 0 && text_[i_ - 1] == ':' && 
                    iBefore + 1 < text_.length() &&
                    startsWithUrlSchema(text_.substr(iBefore + 1, i_ - iBefore + 1))) {
                    while (i_ < text_.length() && isUrlChar(text_[i_])) {
                        str += text_[i_];
                        i_++;
                    }
                }
                
                str = insertBeforeLastWhitespace(str, "\"");
                output_ += str;
                parseConcatenatedString();
                return true;
                
            } else if (text_[i_] == '\\') {
                // Handle escape sequences
                if (i_ + 1 < text_.length()) {
                    char nextChar = text_[i_ + 1];
                    char escapeChar = getEscapeCharacter(nextChar);
                    
                    if (escapeChar != '\0') {
                        str += text_.substr(i_, 2);
                        i_ += 2;
                    } else if (nextChar == 'u') {
                        size_t j = 2;
                        while (j < 6 && i_ + j < text_.length() && isHex(text_[i_ + j])) {
                            j++;
                        }
                        
                        if (j == 6) {
                            str += text_.substr(i_, 6);
                            i_ += 6;
                        } else if (i_ + j >= text_.length()) {
                            i_ = text_.length();
                        } else {
                            throwInvalidUnicodeCharacter();
                        }
                    } else {
                        str += nextChar;
                        i_ += 2;
                    }
                } else {
                    i_++;
                }
            } else {
                char c = text_[i_];
                
                if (c == '"' && (i_ == 0 || text_[i_ - 1] != '\\')) {
                    str += "\\\"";
                    i_++;
                } else if (isControlCharacter(c)) {
                    str += getControlCharacter(c);
                    i_++;
                } else {
                    if (!isValidStringCharacter(c)) {
                        throwInvalidCharacter(c);
                    }
                    str += c;
                    i_++;
                }
            }
            
            if (skipEscapeChars && i_ < text_.length() && text_[i_] == '\\') {
                i_++;
            }
        }
        
        return false;
    }
    
    bool parseConcatenatedString() {
        bool processed = false;
        
        parseWhitespaceAndSkipComments();
        while (i_ < text_.length() && text_[i_] == '+') {
            processed = true;
            i_++;
            parseWhitespaceAndSkipComments();
            
            output_ = stripLastOccurrence(output_, "\"", true);
            size_t start = output_.length();
            bool parsedStr = parseString();
            
            if (parsedStr) {
                output_ = removeAtIndex(output_, start, 1);
            } else {
                output_ = insertBeforeLastWhitespace(output_, "\"");
            }
        }
        
        return processed;
    }
    
    bool parseNumber() {
        size_t start = i_;
        
        if (i_ < text_.length() && text_[i_] == '-') {
            i_++;
            if (atEndOfNumber()) {
                repairNumberEndingWithNumericSymbol(start);
                return true;
            }
            if (i_ >= text_.length() || !isDigit(text_[i_])) {
                i_ = start;
                return false;
            }
        }
        
        while (i_ < text_.length() && isDigit(text_[i_])) {
            i_++;
        }
        
        if (i_ < text_.length() && text_[i_] == '.') {
            i_++;
            if (atEndOfNumber()) {
                repairNumberEndingWithNumericSymbol(start);
                return true;
            }
            if (i_ >= text_.length() || !isDigit(text_[i_])) {
                i_ = start;
                return false;
            }
            while (i_ < text_.length() && isDigit(text_[i_])) {
                i_++;
            }
        }
        
        if (i_ < text_.length() && (text_[i_] == 'e' || text_[i_] == 'E')) {
            i_++;
            if (i_ < text_.length() && (text_[i_] == '-' || text_[i_] == '+')) {
                i_++;
            }
            if (atEndOfNumber()) {
                repairNumberEndingWithNumericSymbol(start);
                return true;
            }
            if (i_ >= text_.length() || !isDigit(text_[i_])) {
                i_ = start;
                return false;
            }
            while (i_ < text_.length() && isDigit(text_[i_])) {
                i_++;
            }
        }
        
        if (!atEndOfNumber()) {
            i_ = start;
            return false;
        }
        
        if (i_ > start) {
            std::string num = text_.substr(start, i_ - start);
            // Check for invalid leading zero
            static std::regex leadingZeroPattern("^0\\d");
            bool hasInvalidLeadingZero = std::regex_search(num, leadingZeroPattern);
            
            if (hasInvalidLeadingZero) {
                output_ += "\"" + num + "\"";
            } else {
                output_ += num;
            }
            return true;
        }
        
        return false;
    }
    
    bool parseKeywords() {
        return parseKeyword("true", "true") ||
               parseKeyword("false", "false") ||
               parseKeyword("null", "null") ||
               parseKeyword("True", "true") ||
               parseKeyword("False", "false") ||
               parseKeyword("None", "null");
    }
    
    bool parseKeyword(const std::string& name, const std::string& value) {
        if (i_ + name.length() <= text_.length() && 
            text_.substr(i_, name.length()) == name) {
            output_ += value;
            i_ += name.length();
            return true;
        }
        return false;
    }
    
    bool parseUnquotedString(bool isKey) {
        size_t start = i_;
        
        if (i_ >= text_.length() || !isFunctionNameCharStart(text_[i_])) {
            return false;
        }
        
        while (i_ < text_.length() && isFunctionNameChar(text_[i_])) {
            i_++;
        }
        
        size_t j = i_;
        while (j < text_.length() && isWhitespace(text_, j)) {
            j++;
        }
        
        if (j < text_.length() && text_[j] == '(') {
            // Function call like NumberLong(2) or callback({...})
            i_ = j + 1;
            parseValue();
            
            if (i_ < text_.length() && text_[i_] == ')') {
                i_++;
                if (i_ < text_.length() && text_[i_] == ';') {
                    i_++;
                }
            }
            return true;
        }
        
        while (i_ < text_.length() && !isUnquotedStringDelimiter(text_[i_]) && 
               !isQuote(text_[i_]) && (!isKey || text_[i_] != ':')) {
            i_++;
        }
        
        // Check for URL
        if (i_ > 0 && text_[i_ - 1] == ':' && 
            start < text_.length() && 
            startsWithUrlSchema(text_.substr(start, i_ - start + 2))) {
            while (i_ < text_.length() && isUrlChar(text_[i_])) {
                i_++;
            }
        }
        
        if (i_ > start) {
            // Go back to remove trailing whitespace
            while (i_ > start && isWhitespace(text_, i_ - 1)) {
                i_--;
            }
            
            std::string symbol = text_.substr(start, i_ - start);
            
            if (symbol == "undefined") {
                output_ += "null";
            } else {
                // Quote the unquoted string
                output_ += "\"";
                for (char c : symbol) {
                    if (c == '"') {
                        output_ += "\\\"";
                    } else if (c == '\\') {
                        output_ += "\\\\";
                    } else {
                        output_ += c;
                    }
                }
                output_ += "\"";
            }
            
            if (i_ < text_.length() && text_[i_] == '"') {
                i_++;
            }
            
            return true;
        }
        
        return false;
    }
    
    bool parseRegex() {
        if (i_ >= text_.length() || text_[i_] != '/') {
            return false;
        }
        
        size_t start = i_;
        i_++;
        
        while (i_ < text_.length() && (text_[i_] != '/' || 
               (i_ > 0 && text_[i_ - 1] == '\\'))) {
            i_++;
        }
        i_++;
        
        output_ += "\"" + text_.substr(start, i_ - start) + "\"";
        return true;
    }
    
    size_t prevNonWhitespaceIndex(size_t start) {
        size_t prev = start;
        while (prev > 0 && isWhitespace(text_, prev)) {
            prev--;
        }
        return prev;
    }
    
    bool atEndOfNumber() {
        return i_ >= text_.length() || isDelimiter(text_[i_]) || isWhitespace(text_, i_);
    }
    
    void repairNumberEndingWithNumericSymbol(size_t start) {
        output_ += text_.substr(start, i_ - start) + "0";
    }
    
    void throwInvalidCharacter(char c) {
        std::ostringstream oss;
        oss << "Invalid character '" << c << "'";
        throw JSONRepairError(oss.str(), i_);
    }
    
    void throwUnexpectedCharacter() {
        std::ostringstream oss;
        oss << "Unexpected character '" << text_[i_] << "'";
        throw JSONRepairError(oss.str(), i_);
    }
    
    void throwUnexpectedEnd() {
        throw JSONRepairError("Unexpected end of json string", text_.length());
    }
    
    void throwObjectKeyExpected() {
        throw JSONRepairError("Object key expected", i_);
    }
    
    void throwColonExpected() {
        throw JSONRepairError("Colon expected", i_);
    }
    
    void throwInvalidUnicodeCharacter() {
        std::string chars = text_.substr(i_, std::min(size_t(6), text_.length() - i_));
        throw JSONRepairError("Invalid unicode character \"" + chars + "\"", i_);
    }
};

// Enhanced API: Repair with diagnostics (PRIMARY IMPLEMENTATION)
void repair_with_diagnostics(std::string& json_inout, RepairInfo& info_out) {
    info_out.original_size = json_inout.size();
    info_out.corrections.clear();
    info_out.needed_repair = false;
    
    // Pre-process: Fix common UTF-8 encoding issues (mojibake)
    std::string preprocessed = fixUtf8Encoding(json_inout);
    if (preprocessed != json_inout) {
        info_out.corrections.push_back("Fixed UTF-8 encoding issues");
        info_out.needed_repair = true;
    }
    
    try {
        // Perform repair
        JSONRepairImpl impl(preprocessed);
        std::string repaired = impl.repair();
        
        // Compare and collect diagnostics
        info_out.repaired_size = repaired.size();
        // Update needed_repair flag (may already be true from encoding fixes)
        info_out.needed_repair = info_out.needed_repair || (preprocessed != repaired);
        
        if (preprocessed != repaired) {
            // Analyze differences to provide meaningful corrections
            if (preprocessed.find('\'') != std::string::npos && repaired.find('\'') == std::string::npos) {
                info_out.corrections.push_back("Converted single quotes to double quotes");
            }
            if (preprocessed.find(",}") != std::string::npos || preprocessed.find(",]") != std::string::npos) {
                if (repaired.find(",}") == std::string::npos && repaired.find(",]") == std::string::npos) {
                    info_out.corrections.push_back("Removed trailing commas");
                }
            }
            // Check for unquoted keys by looking for patterns like: {name: or ,name:
            size_t pos = 0;
            bool found_unquoted = false;
            while ((pos = preprocessed.find(':', pos)) != std::string::npos) {
                // Look backward from colon to find key start
                size_t key_end = pos;
                while (key_end > 0 && (preprocessed[key_end - 1] == ' ' || preprocessed[key_end - 1] == '\t')) {
                    key_end--;
                }
                if (key_end > 0 && preprocessed[key_end - 1] != '"') {
                    // Check if there's a quote before this position
                    size_t check_pos = key_end - 1;
                    while (check_pos > 0 && (std::isalnum(preprocessed[check_pos]) || preprocessed[check_pos] == '_')) {
                        check_pos--;
                    }
                    if (check_pos < key_end - 1 && preprocessed[check_pos] != '"') {
                        found_unquoted = true;
                        break;
                    }
                }
                pos++;
            }
            if (found_unquoted) {
                info_out.corrections.push_back("Added quotes around unquoted object keys");
            }
            
            // Only add generic message if no specific corrections were identified
            if (info_out.corrections.size() == 1 && info_out.corrections[0] == "Fixed UTF-8 encoding issues") {
                // UTF-8 fix was done, but JSON structure also changed
                info_out.corrections.push_back("Applied JSON formatting corrections");
            } else if (info_out.corrections.empty()) {
                info_out.corrections.push_back("Applied JSON formatting corrections");
            }
        }
        
        // Update input string with repaired version
        json_inout = std::move(repaired);
        
    } catch (const JSONRepairError&) {
        // Let the exception propagate
        throw;
    }
}

// Simple API: Wrapper around enhanced version
std::string repair(const std::string& text) {
    std::string json_copy = text;
    RepairInfo info;  // Diagnostics discarded
    repair_with_diagnostics(json_copy, info);
    return json_copy;
}

// Check validity without repairing
bool check_json_validity(const std::string& text, RepairInfo& info_out) {
    info_out.original_size = text.size();
    info_out.corrections.clear();
    info_out.needed_repair = false;
    
    try {
        // Attempt to repair - if no changes needed, it's valid
        std::string text_copy = text;
        repair_with_diagnostics(text_copy, info_out);
        
        return !info_out.needed_repair;  // Valid if no repair needed
        
    } catch (const JSONRepairError&) {
        // If repair fails, JSON is invalid and cannot be repaired
        info_out.needed_repair = true;
        info_out.corrections.push_back("JSON is invalid and cannot be repaired");
        return false;
    }
}

} // namespace jsonrepair
