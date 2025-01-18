# IRC Server Test Summary
## Test Execution Details

- **Date**: 2025-01-18 00:57:15 UTC
- **Duration**: 0h 0m 10s
- **User**: Trafexofive
- **Host**: morpheus-arch

## Configuration
- Server Port: 22200
- Debug Mode: Enabled
- Verbose Mode: Enabled
- Test File: test/test-cases/rfc-join.test

## Test Results
- **Total Tests**: 10
- **Passed**: 9 🟢
- **Failed**: 1 🔴
- **Skipped**: 0 ⚪
- **Success Rate**: 90%

## Failed Tests
```
❌ JOIN CHANNEL WITH SPACES (INVALID) - Expected: ':test-user!testUser@localhost JOIN #test channel'
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
Generated: 2025-01-18 00:54:04 UTC
Test Suite User: Trafexofive

Failed Tests:
❌ JOIN CHANNEL WITH SPACES (INVALID) - Expected: ':test-user!testUser@localhost JOIN #test channel'
❌ JOIN CHANNEL WITH INVALID CHARACTERS (INVALID) - Expected: ':test-user!testUser@localhost JOIN #test-channel,'

Test Statistics:
- Total Tests: 20
- Passed: 18
- Failed: 2
- Skipped: 0
- Success Rate: 90%
```
