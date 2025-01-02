# IRC Server Test Summary
## Test Execution Details

- **Date**: 2025-01-02 14:43:22 UTC
- **Duration**: 0h 0m 37s
- **User**: mlamkadm
- **Host**: morpheus-arch

## Configuration
- Server Port: 22200
- Debug Mode: Enabled
- Verbose Mode: Enabled
- Test File: ./test/test.txt

## Test Results
- **Total Tests**: 37
- **Passed**: 13 🟢
- **Failed**: 24 🔴
- **Skipped**: 0 ⚪
- **Success Rate**: 35%

## Failed Tests
```
❌ Basic Registration - Expected: '001'
❌ SASL Authentication - Expected: 'AUTHENTICATE'
❌ CAP Negotiation - Expected: 'CAP'
❌ Nick Collision - Expected: '433'
❌ Channel Part - Expected: 'PART'
❌ User Mode Change - Expected: 'MODE'
❌ Channel Mode Change - Expected: 'MODE'
❌ Channel Ban - Expected: 'MODE'
❌ Private Message - Expected: 'PRIVMSG'
❌ Topic Change - Expected: 'TOPIC'
❌ Topic Query - Expected: 'TOPIC'
❌ Channel List - Expected: 'LIST'
❌ Who Query - Expected: 'WHO'
❌ Whois Query - Expected: 'WHOIS'
❌ Invalid Channel Name - Expected: '403'
❌ No Such Channel - Expected: '403'
❌ Version Query - Expected: 'VERSION'
❌ Time Query - Expected: 'TIME'
❌ Quit Command - Expected: 'ERROR'
❌ Away Status - Expected: 'AWAY'
❌ ISON Query - Expected: 'ISON'
❌ USERHOST Query - Expected: 'USERHOST'
❌ Multiple Mode Changes - Expected: 'MODE'
❌ UTF8 Nick Test - Expected: 'NICK'
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

