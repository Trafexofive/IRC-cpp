# IRC Server Test Summary
## Test Execution Details

- **Date**: 2025-01-03 17:45:06 UTC
- **Duration**: 0h 0m 22s
- **User**: mlamkadm
- **Host**: morpheus-arch

## Configuration
- Server Port: 22200
- Debug Mode: Enabled
- Verbose Mode: Enabled
- Test File: test/test-cases/join-tests-2

## Test Results
- **Total Tests**: 44
- **Passed**: 24 🟢
- **Failed**: 20 🔴
- **Skipped**: 0 ⚪
- **Success Rate**: 54%

## Failed Tests
```
❌ AUTH - Expected: 'Authenticate and join public channel'
❌ AUTH - Expected: 'Alilepro135!'
❌ AUTH - Expected: 'Authentication fails with wrong password'
❌ AUTH - Expected: 'WrongPassword!'
❌ AUTH - Expected: 'Authenticate and join private channel with key'
❌ AUTH - Expected: 'Alilepro135!'
❌ SeparateAuth - Expected: 'Separate PASS, NICK, USER commands to authenticate'
❌ NoAuth - Expected: 'Attempt to join a channel without authentication'
❌ AuthMessage - Expected: 'Authenticate and send a message to a channel'
❌ AUTH - Expected: 'Alilepro135!'
```

## System Information
```
OS: Linux
Kernel: 6.6.68-1-lts
Architecture: x86_64
Memory: 15Gi
```

## Log Files
- Test Log: irc_test.log
- Server Log: server.log
- Summary File: testSummary.md

