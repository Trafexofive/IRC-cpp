# IRC Server Test Summary
## Test Execution Details

- **Date**: 2025-01-18 02:13:39 UTC
- **Duration**: 0h 0m 4s
- **User**: Trafexofive
- **Host**: morpheus-arch

## Configuration
- Server Port: 22200
- Debug Mode: Enabled
- Verbose Mode: Enabled
- Test File: test/test-cases/rfc-join.test

## Test Results
- **Total Tests**: 4
- **Passed**: 4 🟢
- **Failed**: 0 🔴
- **Skipped**: 0 ⚪
- **Success Rate**: 100%

## Failed Tests
```
No failed tests
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
Generated: 2025-01-18 02:11:48 UTC
Test Suite User: Trafexofive

Failed Tests:
❌ SINGLE NORMAL CHANNEL JOIN - Expected: ':test-user!testUser@localhost JOIN #test-channel'
❌ SINGLE NORMAL CHANNEL JOIN WITH KEY - Expected: ':test-user!testUser@localhost JOIN #test-channel'
❌ JOIN 2 CHANNELS - Expected: ':test-user!testUser@localhost JOIN #test-channel1\r\n:test-user!testUser@localhost JOIN #test-channel2'
❌ JOIN MULTIPLE CHANNELS - Expected: ':test-user!testUser@localhost JOIN #test-channel1\r\n:test-user!testUser@localhost JOIN #test-channel2\r\n:test-user!testUser@localhost JOIN #test-channel3'

Test Statistics:
- Total Tests: 4
- Passed: 0
- Failed: 4
- Skipped: 0
- Success Rate: 0%
```
