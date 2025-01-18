# Test Case 1: Send PRIVMSG to a Channel
PRIVMSG TO CHANNEL|:test-user!testUser@localhost PRIVMSG #test-channel :Hello, world!|PASS Alilepro135!\r\nNICK test-user\r\nUSER testUser 0 * :Test User\r\nPRIVMSG #test-channel :Hello, world!\r\n

# Test Case 2: Send PRIVMSG to a User
PRIVMSG TO USER|:test-user!testUser@localhost PRIVMSG other-user :Hi there!|PASS Alilepro135!\r\nNICK test-user\r\nUSER testUser 0 * :Test User\r\nPRIVMSG other-user :Hi there!\r\n

# Test Case 3: Send PRIVMSG with Special Characters
PRIVMSG WITH SPECIAL CHARACTERS|:test-user!testUser@localhost PRIVMSG #test-channel :Hello! @#$%^&*()_+{}[]|PASS Alilepro135!\r\nNICK test-user\r\nUSER testUser 0 * :Test User\r\nPRIVMSG #test-channel :Hello! @#$%^&*()_+{}[]\r\n

# Test Case 4: Send PRIVMSG with Emojis
PRIVMSG WITH EMOJIS|:test-user!testUser@localhost PRIVMSG #test-channel :Hello! 😊👍|PASS Alilepro135!\r\nNICK test-user\r\nUSER testUser 0 * :Test User\r\nPRIVMSG #test-channel :Hello! 😊👍\r\n

# Test Case 5: Send PRIVMSG with Long Message
PRIVMSG WITH LONG MESSAGE|:test-user!testUser@localhost PRIVMSG #test-channel :This is a very long message that spans multiple words and characters to test the handling of long messages in the IRC protocol.|PASS Alilepro135!\r\nNICK test-user\r\nUSER testUser 0 * :Test User\r\nPRIVMSG #test-channel :This is a very long message that spans multiple words and characters to test the handling of long messages in the IRC protocol.\r\n

# Test Case 6: Send PRIVMSG with Empty Message (Invalid)
PRIVMSG WITH EMPTY MESSAGE (INVALID)|:test-user!testUser@localhost PRIVMSG #test-channel :|PASS Alilepro135!\r\nNICK test-user\r\nUSER testUser 0 * :Test User\r\nPRIVMSG #test-channel :\r\n

# Test Case 7: Send PRIVMSG to Invalid Channel (Invalid)
PRIVMSG TO INVALID CHANNEL (INVALID)|:test-user!testUser@localhost PRIVMSG #invalid-channel :Hello|PASS Alilepro135!\r\nNICK test-user\r\nUSER testUser 0 * :Test User\r\nPRIVMSG #invalid-channel :Hello\r\n

# Test Case 8: Send PRIVMSG to Invalid User (Invalid)
PRIVMSG TO INVALID USER (INVALID)|:test-user!testUser@localhost PRIVMSG invalid-user :Hello|PASS Alilepro135!\r\nNICK test-user\r\nUSER testUser 0 * :Test User\r\nPRIVMSG invalid-user :Hello\r\n

# Test Case 9: Send PRIVMSG with Trailing Space
PRIVMSG WITH TRAILING SPACE|:test-user!testUser@localhost PRIVMSG #test-channel :Hello |PASS Alilepro135!\r\nNICK test-user\r\nUSER testUser 0 * :Test User\r\nPRIVMSG #test-channel :Hello \r\n

# Test Case 10: Send PRIVMSG with Leading Space
PRIVMSG WITH LEADING SPACE|:test-user!testUser@localhost PRIVMSG #test-channel : Hello|PASS Alilepro135!\r\nNICK test-user\r\nUSER testUser 0 * :Test User\r\nPRIVMSG #test-channel : Hello\r\n

# Test Case 11: Send PRIVMSG with Multiple Spaces
PRIVMSG WITH MULTIPLE SPACES|:test-user!testUser@localhost PRIVMSG #test-channel :Hello   world!|PASS Alilepro135!\r\nNICK test-user\r\nUSER testUser 0 * :Test User\r\nPRIVMSG #test-channel :Hello   world!\r\n

# Test Case 12: Send PRIVMSG with Newline (Invalid)
PRIVMSG WITH NEWLINE (INVALID)|:test-user!testUser@localhost PRIVMSG #test-channel :Hello\nworld|PASS Alilepro135!\r\nNICK test-user\r\nUSER testUser 0 * :Test User\r\nPRIVMSG #test-channel :Hello\nworld\r\n

# Test Case 13: Send PRIVMSG with Carriage Return (Invalid)
PRIVMSG WITH CARRIAGE RETURN (INVALID)|:test-user!testUser@localhost PRIVMSG #test-channel :Hello\rworld|PASS Alilepro135!\r\nNICK test-user\r\nUSER testUser 0 * :Test User\r\nPRIVMSG #test-channel :Hello\rworld\r\n

# Test Case 14: Send PRIVMSG to Multiple Channels (Invalid)
PRIVMSG TO MULTIPLE CHANNELS (INVALID)|:test-user!testUser@localhost PRIVMSG #channel1,#channel2 :Hello|PASS Alilepro135!\r\nNICK test-user\r\nUSER testUser 0 * :Test User\r\nPRIVMSG #channel1,#channel2 :Hello\r\n

# Test Case 15: Send PRIVMSG to Channel and User (Invalid)
PRIVMSG TO CHANNEL AND USER (INVALID)|:test-user!testUser@localhost PRIVMSG #channel1,user2 :Hello|PASS Alilepro135!\r\nNICK test-user\r\nUSER testUser 0 * :Test User\r\nPRIVMSG #channel1,user2 :Hello\r\n

# Test Case 16: Send PRIVMSG with No Target (Invalid)
PRIVMSG WITH NO TARGET (INVALID)|:test-user!testUser@localhost PRIVMSG :Hello|PASS Alilepro135!\r\nNICK test-user\r\nUSER testUser 0 * :Test User\r\nPRIVMSG :Hello\r\n

# Test Case 17: Send PRIVMSG with No Message (Invalid)
PRIVMSG WITH NO MESSAGE (INVALID)|:test-user!testUser@localhost PRIVMSG #test-channel|PASS Alilepro135!\r\nNICK test-user\r\nUSER testUser 0 * :Test User\r\nPRIVMSG #test-channel\r\n

# Test Case 18: Send PRIVMSG with Unicode Characters
PRIVMSG WITH UNICODE CHARACTERS|:test-user!testUser@localhost PRIVMSG #test-channel :こんにちは|PASS Alilepro135!\r\nNICK test-user\r\nUSER testUser 0 * :Test User\r\nPRIVMSG #test-channel :こんにちは\r\n

# Test Case 19: Send PRIVMSG with Mixed Unicode and Special Characters
PRIVMSG WITH MIXED UNICODE AND SPECIAL CHARACTERS|:test-user!testUser@localhost PRIVMSG #test-channel :こんにちは! @#$%^&*()_+{}[]|PASS Alilepro135!\r\nNICK test-user\r\nUSER testUser 0 * :Test User\r\nPRIVMSG #test-channel :こんにちは! @#$%^&*()_+{}[]\r\n

# Test Case 20: Send PRIVMSG with Maximum Length Message
PRIVMSG WITH MAXIMUM LENGTH MESSAGE|:test-user!testUser@localhost PRIVMSG #test-channel :This is a very long message that is exactly 512 characters long. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.|PASS Alilepro135!\r\nNICK test-user\r\nUSER testUser 0 * :Test User\r\nPRIVMSG #test-channel :This is a very long message that is exactly 512 characters long. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\r\n
