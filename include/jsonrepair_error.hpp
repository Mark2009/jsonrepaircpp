#pragma once

#include <stdexcept>
#include <string>

namespace jsonrepair {

/**
 * Exception class for JSON repair errors
 */
class JSONRepairError : public std::runtime_error {
public:
    /**
     * Construct a JSONRepairError
     * @param message Error message
     * @param position Position in the input string where the error occurred
     */
    JSONRepairError(const std::string& message, size_t position)
        : std::runtime_error(message + " at position " + std::to_string(position))
        , position_(position) 
        , message_(message)
    {}

    /**
     * Get the position where the error occurred
     * @return Position in the input string
     */
    size_t position() const noexcept { return position_; }

    /**
     * Get the error message without position information
     * @return Error message
     */
    const std::string& message() const noexcept { return message_; }

private:
    size_t position_;
    std::string message_;
};

} // namespace jsonrepair
