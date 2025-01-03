# IRC Server Test Summary
## Test Execution Details

- **Date**: 2025-01-03 10:32:37 UTC
- **Duration**: 0h 0m 20s
- **User**: mlamkadm
- **Host**: morpheus-arch

## Configuration
- Server Port: 22200
- Debug Mode: Enabled
- Verbose Mode: Enabled
- Test File: test/test-cases/join-tests-2

## Test Results
- **Total Tests**: 20
- **Passed**: 10 🟢
- **Failed**: 10 🔴
- **Skipped**: 0 ⚪
- **Success Rate**: 50%

## Failed Tests
```
❌ Join public channel - Expected: ':test_user!~user@host JOIN #public_channel\r\n:server 353 test_user = #public_channel :@test_user\r\n:server 366 test_user #public_channel :End of /NAMES list.'
❌ Join private channel without key - Expected: ':server 475 test_user #private_channel :Cannot join channel (+k) - bad key'
❌ Join private channel with correct key - Expected: ':test_user!~user@host JOIN #private_channel\r\n:server 353 test_user = #private_channel :@test_user\r\n:server 366 test_user #private_channel :End of /NAMES list.'
❌ Join invite-only channel without invite - Expected: ':server 473 test_user #invite_only_channel :Cannot join channel (+i) - you must be invited'
❌ Join invite-only channel with invite - Expected: ':test_user!~user@host JOIN #invite_only_channel\r\n:server 353 test_user = #invite_only_channel :@test_user\r\n:server 366 test_user #invite_only_channel :End of /NAMES list.'
❌ Join multiple channels - Expected: ':test_user!~user@host JOIN #channel1,#channel2\r\n:server 353 test_user = #channel1 :@test_user\r\n:server 353 test_user = #channel2 :@test_user\r\n:server 366 test_user #channel1 :End of /NAMES list.\r\n:server 366 test_user #channel2 :End of /NAMES list.'
❌ Join channel with space in name - Expected: ':test_user!~user@host JOIN #channel\ with\ space\r\n:server 353 test_user = #channel\ with\ space :@test_user\r\n:server 366 test_user #channel\ with\ space :End of /NAMES list.'
❌ Join channel with special chars - Expected: ':test_user!~user@host JOIN #channel!@#$%\r\n:server 353 test_user = #channel!@#$% :@test_user\r\n:server 366 test_user #channel!@#$% :End of /NAMES list.'
❌ Join non-existent channel - Expected: ':server 403 test_user #nonexistent :No such channel'
❌ Join too many channels - Expected: ':server 405 test_user :You have joined too many channels'
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

