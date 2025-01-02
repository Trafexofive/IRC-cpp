# IRC Server Test Summary
## Test Execution Details

- **Date**: 2025-01-02 17:08:33 UTC
- **Duration**: 0h 0m 10s
- **User**: mlamkadm
- **Host**: morpheus-arch

## Configuration
- Server Port: 22200
- Debug Mode: Enabled
- Verbose Mode: Enabled
- Test File: test/test-cases/privmsg-tests

## Test Results
- **Total Tests**: 10
- **Passed**: 0 🟢
- **Failed**: 10 🔴
- **Skipped**: 0 ⚪
- **Success Rate**: 0%

## Failed Tests
```
❌ Private Message to User - Expected: 'PRIVMSG Wiz :Are you receiving this message ?'
❌ Private Message to Host - Expected: 'PRIVMSG jto@tolsun.oulu.fi :Hello !'
❌ Private Message to Full Address - Expected: 'PRIVMSG Wiz!jto@tolsun.oulu.fi :Hello !'
❌ Private Message to Channel - Expected: 'PRIVMSG #channel :Hello everyone!'
❌ Private Message to Multiple Targets - Expected: 'PRIVMSG Wiz,#channel :Hello!'
❌ Private Message with Special Characters - Expected: 'PRIVMSG Wiz :Hello @world!'
❌ Private Message to Operator - Expected: 'PRIVMSG $*.fi :Server rebooting.'
❌ Private Message to Masked Users - Expected: 'PRIVMSG #*.edu :NSFNet maintenance.'
❌ Private Message with Long Text - Expected: 'PRIVMSG Wiz :This is a very long message that should be split into multiple lines.'
❌ Private Message Error Handling - Expected: 'ERR_NORECIPIENT'
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
- Summary File: test_summary.md

