# IRC Server Test Summary
## Test Execution Details

- **Date**: 2025-01-04 10:28:16 UTC
- **Duration**: 0h 0m 13s
- **User**: mlamkadm
- **Host**: morpheus-arch

## Configuration
- Server Port: 22200
- Debug Mode: Enabled
- Verbose Mode: Enabled
- Test File: test/test-cases/join-tests

## Test Results
- **Total Tests**: 13
- **Passed**: 2 🟢
- **Failed**: 24 🔴
- **Skipped**: 0 ⚪
- **Success Rate**: 15%

## Failed Tests
```
❌ Version Query - Expected: 'VERSION'
❌ Time Query - Expected: 'TIME'
❌ Join single channel - Expected: ':server 001 join_test1 :Welcome to the IRC network\r\n:join_test1!user@host JOIN #test\r\n:server 353 join_test1 = #test :join_test1\r\n:server 366 join_test1 #test :End of /NAMES list.'
❌ Join multiple channels - Expected: ':server 001 join_test2 :Welcome to the IRC network\r\n:join_test2!user@host JOIN #test1\r\n:server 353 join_test2 = #test1 :join_test2\r\n:server 366 join_test2 #test1 :End of /NAMES list.\r\n:join_test2!user@host JOIN #test2\r\n:server 353 join_test2 = #test2 :join_test2\r\n:server 366 join_test2 #test2 :End of /NAMES list.'
❌ Join channel with key - Expected: ':server 001 join_test3 :Welcome to the IRC network\r\n:join_test3!user@host JOIN #keyed\r\n:server 353 join_test3 = #keyed :join_test3\r\n:server 366 join_test3 #keyed :End of /NAMES list.'
❌ Join invite-only channel without invite - Expected: ':server 001 join_test4 :Welcome to the IRC network\r\n:server 473 join_test4 #invite :Cannot join channel (+i) - you must be invited.'
❌ Join invite-only channel with invite - Expected: ':server 001 join_test5 :Welcome to the IRC network\r\n:server 341 join_test5 join_test5 #invite\r\n:server 342 join_test5 #invite :Invitation sent.\r\n:join_test5!user@host JOIN #invite\r\n:server 353 join_test5 = #invite :join_test5\r\n:server 366 join_test5 #invite :End of /NAMES list.'
❌ Join channel with incorrect key - Expected: ':server 001 join_test6 :Welcome to the IRC network\r\n:server 475 join_test6 #keyed :Cannot join channel (+k) - incorrect key.'
❌ Join channel with correct key - Expected: ':server 001 join_test7 :Welcome to the IRC network\r\n:join_test7!user@host JOIN #keyed\r\n:server 353 join_test7 = #keyed :join_test7\r\n:server 366 join_test7 #keyed :End of /NAMES list.'
❌ Join banned channel - Expected: ':server 001 join_test8 :Welcome to the IRC network\r\n:server 474 join_test8 #banned :Cannot join channel (+b) - you are banned.'
❌ Join channel with limit exceeded - Expected: ':server 001 join_test9 :Welcome to the IRC network\r\n:server 471 join_test9 #limited :Cannot join channel (+l) - channel is full.'
❌ Join channel removing all others - Expected: ':server 001 join_test10 :Welcome to the IRC network\r\n:join_test10!user@host JOIN #test\r\n:server 353 join_test10 = #test :join_test10\r\n:server 366 join_test10 #test :End of /NAMES list.\r\n:join_test10!user@host JOIN 0\r\n:server 396 join_test10 :You have left all channels.'
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

