#pragma once

#include <string>

/**
 * @brief Protocol commands between server and client
 */
namespace Protocol {

    constexpr const char* CMD_CORES = "CORES";
    constexpr const char* CMD_TASK = "TASK";
    constexpr const char* CMD_RESULT = "RESULT";

}