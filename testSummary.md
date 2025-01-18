# IRC Server Test Summary
## Test Execution Details

- **Date**: 2025-01-18 03:25:57 UTC
- **Duration**: 0h 0m 5s
- **User**: Trafexofive
- **Host**: morpheus-arch

## Configuration
- Server Port: 22200
- Debug Mode: Enabled
- Verbose Mode: Enabled
- Test File: test/test-cases/rfc-join.test

## Test Results
- **Total Tests**: 4
- **Passed**: 0 🟢
- **Failed**: 4 🔴
- **Skipped**: 0 ⚪
- **Success Rate**: 0%

## Failed Tests
```
❌ SINGLE NORMAL CHANNEL JOIN - Expected: ':test-user!testUser@localhost JOIN #test-channel'
❌ SINGLE NORMAL CHANNEL JOIN WITH KEY - Expected: ':test-user!testUser@localhost JOIN #test-channel'
❌ JOIN 2 CHANNELS - Expected: ':test-user!testUser@localhost JOIN #test-channel1\r\n:test-user!testUser@localhost JOIN #test-channel2'
❌ JOIN MULTIPLE CHANNELS - Expected: ':test-user!testUser@localhost JOIN #test-channel1\r\n:test-user!testUser@localhost JOIN #test-channel2\r\n:test-user!testUser@localhost JOIN #test-channel3'
```

## System Information
```
OS: Linux
Kernel: 6.6.68-1-lts
Architecture: x86_64
Memory: 15Gi
```

## Log Files
- Test Log: irc.log
- Server Log: server.log
- Summary File: testSummary.md


## Error Summary
```
Error Summary Report
===================
Generated: 2025-01-18 03:17:38 UTC
Test Suite User: Trafexofive

No errors detected during test execution.

Test Statistics:
- Total Tests: 4
- Passed: 4
- Failed: 0
- Skipped: 0
- Success Rate: 100%
```
