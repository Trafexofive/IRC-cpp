
#!/bin/bash
# test_suite.sh - Main entry point for the IRC server test suite

# Ensure the script is executed in the project's root directory
cd "$(dirname "$0")" || exit 1

# Include all libraries from the lib directory
LIB_DIR="./lib"
for lib_file in "$LIB_DIR"/*.sh; do
  source "$lib_file"
done

# Default test file and other configurations
default_test_file="tests/test_cases.txt"

# Parse command-line arguments
parse_args "$@"

# Initialize configuration
init_config

# Validate prerequisites
validate_prerequisites || exit 1

# Start the IRC server
start_server || {
  log_error "Failed to start the IRC server. Aborting."
  exit 1
}

# Execute tests
execute_tests "$TEST_FILE"

# Stop the IRC server
stop_server

# Generate and display the test summary
generate_summary

# Print completion message
log_info "Test suite completed successfully. Check the logs and summary for details."
