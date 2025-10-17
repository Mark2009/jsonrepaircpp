#pragma once

#include "jsonrepair_error.hpp"
#include <string>
#include <vector>

namespace jsonrepair {

/**
 * Detailed information about repairs made to JSON
 */
struct RepairInfo {
    bool needed_repair = false;              // True if any repairs were made
    std::vector<std::string> corrections;    // List of corrections applied
    size_t original_size = 0;                // Original JSON size
    size_t repaired_size = 0;                // Repaired JSON size
};

/**
 * Repair a string containing invalid JSON document.
 * For example, changes JavaScript notation into JSON notation.
 *
 * Example:
 *     try {
 *         std::string json = "{name: 'John'}";
 *         std::string repaired = jsonrepair::repair(json);
 *         std::cout << repaired << std::endl;
 *         // Output: {"name": "John"}
 *     } catch (const JSONRepairError& e) {
 *         std::cerr << e.what() << std::endl;
 *     }
 *
 * @param text The invalid JSON string to repair
 * @return The repaired JSON string
 * @throws JSONRepairError if the JSON cannot be repaired
 */
std::string repair(const std::string& text);

/**
 * Repair JSON in-place with detailed diagnostics.
 * This version modifies the input string directly (more efficient for large JSON)
 * and provides detailed information about what repairs were performed.
 *
 * Example:
 *     try {
 *         std::string json = "{name: 'John', age: 30}";
 *         RepairInfo info;
 *         jsonrepair::repair_with_diagnostics(json, info);
 *         
 *         if (info.needed_repair) {
 *             std::cout << "Repairs made:" << std::endl;
 *             for (const auto& correction : info.corrections) {
 *                 std::cout << "  - " << correction << std::endl;
 *             }
 *         }
 *     } catch (const JSONRepairError& e) {
 *         std::cerr << e.what() << std::endl;
 *     }
 *
 * @param json_inout Input JSON string that will be modified in-place
 * @param info_out Output parameter containing repair diagnostics
 * @throws JSONRepairError if the JSON cannot be repaired
 */
void repair_with_diagnostics(std::string& json_inout, RepairInfo& info_out);

/**
 * Check if JSON needs repair without actually repairing it.
 * Useful for validation and metrics collection.
 *
 * @param text The JSON string to check
 * @param info_out Output parameter containing diagnostic information
 * @return true if JSON is valid, false if it needs repair
 */
bool check_json_validity(const std::string& text, RepairInfo& info_out);

} // namespace jsonrepair
