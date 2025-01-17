# IRC Protocol Test Suite
# Based on RFC [modern IRC RFC] and modern IRC protocol extensions
# Format: TestName|ExpectedResponse|Commands

# COMMAND-TO-TEST: {USER}

# FORMAT:
# TestName|ExpectedResponse|Commands
# [Additional lines in the same format if needed]



```test-examples
SINGLE NORMAL CHANNEL JOIN|:test-user!testUser@localhost JOIN #test-channel|PASS Alilepro135!\r\nNICK test-user\r\nUSER testUser 0 * :Test User\r\nJOIN #test-channel\r\n

SINGLE NORMAL CHANNEL JOIN WITH KEY|:test-user!testUser@localhost JOIN #test-channel|PASS Alilepro135!\r\nNICK test-user\r\nUSER testUser 0 * :Test User\r\nJOIN #test-channel Alilepro135!\r\n

JOIN MULTIPLE CHANNELS|:test-user!testUser@localhost JOIN #test-channel1\r\n:test-user!testUser@localhost JOIN #test-channel2|PASS Alilepro135!\r\nNICK test-user\r\nUSER testUser 0 * :Test User\r\nJOIN #test-channel1,#test-channel2\r\n

```
