# IRC Server Test Summary
## Test Execution Details

- **Date**: 2025-01-04 19:12:19 UTC
- **Duration**: 0h 0m 10s
- **User**: mlamkadm
- **Host**: morpheus-arch

## Configuration
- Server Port: 22200
- Debug Mode: Enabled
- Verbose Mode: Enabled
- Test File: test/test-cases/part.test

## Test Results
- **Total Tests**: 20
- **Passed**: 0 🟢
- **Failed**: 40 🔴
- **Skipped**: 0 ⚪
- **Success Rate**: 0%

## Failed Tests
```
❌ PartSingleChannel - Expected: ':server 001 testNick :Welcome to IRC;:testNick!~testNick@host PART #channel'
❌ PartMultipleChannels - Expected: ':server 001 testNick :Welcome to IRC;:testNick!~testNick@host PART #channel1,#channel2'
❌ PartWithMessage - Expected: ':server 001 testNick :Welcome to IRC;:testNick!~testNick@host PART #channel :Goodbye'
❌ PartWithoutChannel - Expected: ':server 461 testNick PART :Not enough parameters'
❌ PartInvalidChannel - Expected: ':server 403 testNick #invalid :No such channel'
❌ PartNotInChannel - Expected: ':server 442 testNick #channel :You're not on that channel'
❌ PartMultipleChannelsWithMessage - Expected: ':server 001 testNick :Welcome to IRC;:testNick!~testNick@host PART #channel1,#channel2 :Goodbye'
❌ PartWithSpecialCharacters - Expected: ':server 001 testNick :Welcome to IRC;:testNick!~testNick@host PART #chan!@# :Leaving now'
❌ PartWithColonInChannelName - Expected: ':server 001 testNick :Welcome to IRC;:testNick!~testNick@host PART #channel:subchannel :Goodbye'
❌ PartWithNumericChannelName - Expected: ':server 001 testNick :Welcome to IRC;:testNick!~testNick@host PART #12345 :Leaving now'
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

