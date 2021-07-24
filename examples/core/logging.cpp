#include <spine/core/logging.hpp>

int main() {
    using namespace spn::logging;

    // Set the logging level. All loglevels below the set level will be disabled.
    // For example, setting the logging level WARN will disable INFO and DEBUG messages
    set_log_level(LogLevel::OFF);
    set_log_level(LogLevel::ERR);
    set_log_level(LogLevel::WARN);
    set_log_level(LogLevel::INFO);
    set_log_level(LogLevel::DEBUG);

    [[maybe_unused]] auto warnings_are_logged = is_loggable(LogLevel::WARN); // check if a particular level is enabled
    [[maybe_unused]] auto logging_level = log_level(); // get the current logging level

    LOG("This is an information message. Used to signal events to a user that should be trackable in the logs")
    WARN("This is a warning message. Used to signal abnormal behaviour to a user that is to be expected");
    ERR("This is an error message. Used to signal erroneous behaviour to a user that should ideally not occur at all")

    // See examples/core/debugging.cpp for the DBG macro that prints when SPINE_DEBUG is defined

    return 0;
}