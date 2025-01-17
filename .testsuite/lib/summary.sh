source $root_dir/config/colors.sh
source $root_dir/config/config.sh

function generate_summary() {
    local duration=$(($(date +%s) - $(date -d "$TEST_START_TIME" +%s)))
    local hours=$((duration / 3600))
    local minutes=$(( (duration % 3600) / 60 ))
    local seconds=$((duration % 60))
    
    cat > "$SUMMARY_FILE" << EOF
# IRC Server Test Summary
## Test Execution Details

- **Date**: $TEST_START_TIME
- **Duration**: ${hours}h ${minutes}m ${seconds}s
- **User**: Trafexofive
- **Host**: $(hostname)

## Configuration
- Server Port: $PORT
- Debug Mode: $([ "$DEBUG" -eq 1 ] && echo "Enabled" || echo "Disabled")
- Verbose Mode: $([ "$VERBOSE" -eq 1 ] && echo "Enabled" || echo "Disabled")
- Test File: ${CUSTOM_TESTS:-"None"}

## Test Results
- **Total Tests**: $TOTAL_TESTS
- **Passed**: $PASSED_TESTS 🟢
- **Failed**: $FAILED_TESTS 🔴
- **Skipped**: $SKIPPED_TESTS ⚪
- **Success Rate**: $(( (PASSED_TESTS * 100) / (TOTAL_TESTS - SKIPPED_TESTS) ))%

## Failed Tests
\`\`\`
EOF

    if [ ${#FAILED_TESTS_ARRAY[@]} -gt 0 ]; then
        printf '%s\n' "${FAILED_TESTS_ARRAY[@]}" >> "$SUMMARY_FILE"
    else
        echo "No failed tests" >> "$SUMMARY_FILE"
    fi

    echo -e "\`\`\`\n" >> "$SUMMARY_FILE"

    cat >> "$SUMMARY_FILE" << EOF
## System Information
\`\`\`
OS: $(uname -s)
Kernel: $(uname -r)
Architecture: $(uname -m)
Memory: $(free -h | awk '/^Mem:/ {print $2}' 2>/dev/null || echo "N/A")
\`\`\`

## Log Files
- Test Log: $LOG_FILE
- Server Log: server.log
- Summary File: $SUMMARY_FILE

EOF

    if [ -f "error_summary.txt" ]; then
        echo -e "\n## Error Summary" >> "$SUMMARY_FILE"
        echo -e "\`\`\`" >> "$SUMMARY_FILE"
        cat "error_summary.txt" >> "$SUMMARY_FILE"
        echo -e "\`\`\`" >> "$SUMMARY_FILE"
    fi
}
