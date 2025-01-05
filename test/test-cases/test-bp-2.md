
---

**System-Prompt:**


---
Generate JOIN tests only.
---

**Prompt:**

---

**Generate a test configuration file in YAML format for IRC commands. The configuration should include the following sections:**

1. **Global test configuration:** 
    - Define server connection details (host, port, password).
    - Set default values for timeout, nick_prefix, and channels.

2. **Reusable test templates:**
    - Define a template for commands that require authentication.
    - Define a template for commands related to channel operations.

3. **Test suites organized by command groups:**
    - **Connection Registration:** Tests for connection and registration commands.
    - **Channel Operations:** Tests for channel-related commands.
    - **User Queries:** Tests for user information commands.
    - **Server Queries:** Tests for server information commands.
    - **Message Commands:** Tests for messaging functionality.
    - **Complex Scenarios:** Multi-step test scenarios.
    - **Error Handling:** Tests for error conditions.
    - **Mode Commands:** Tests for mode settings.

4. **Special test scenarios:**
    - Include scenarios for rate limiting and long-running operations.

5. **Error code reference:**
    - Provide a mapping of error codes to their descriptions.

**Example Test Configuration Layout:**

```yaml
# Global test configuration
config:
  server:
    host: localhost
    port: 22200
    password: "Alilepro135!"
  defaults:
    timeout: 5
    nick_prefix: "test_user"
    channels: ["#test"]

# Reusable test templates
templates:
  # Template for command requiring authentication
  auth_command: &auth_command
    type: AUTH
    setup:
      - authenticate: true
    cleanup:
      - disconnect: true

  # Template for channel operations
  channel_op: &channel_op
    <<: *auth_command 
    setup:
      - authenticate: true
      - join_channel: "#test"
    cleanup:
      - part_channel: "#test"
      - disconnect: true

# Test suites organized by command groups
test_suites:
  - name: "Connection Registration"
    description: "Tests for connection and registration commands"
    tests:
      - name: "valid_registration"
        type: AUTH
        commands:
          - "NICK test_user"
          - "USER test 0 * :Test User"
        expect: "001"  # Welcome message
      
      - name: "invalid_password"
        type: NO_AUTH
        commands:
          - "PASS wrongpass"
          - "NICK test_user"
        expect: "464"  # Password incorrect

  - name: "Channel Operations"
    description: "Tests for channel-related commands"
    tests:
      - name: "join_channel"
        <<: *auth_command
        commands:
          - "JOIN #test"
        expect: 
          - "353"  # Names reply
          - "366"  # End of names
      
      - name: "topic_change"
        <<: *channel_op
        commands:
          - "TOPIC #test :New Topic"
        expect: "332"  # Topic message

  - name: "User Queries"
    description: "Tests for user information commands"
    tests:
      - name: "whois_query"
        <<: *auth_command
        commands:
          - "WHOIS test_user"
        expect:
          - "311"  # Whois user
          - "318"  # End of whois

  - name: "Server Queries"
    description: "Tests for server information commands"
    tests:
      - name: "version_query"
        <<: *auth_command
        commands:
          - "VERSION"
        expect: "351"  # Version reply

  - name: "Message Commands"
    description: "Tests for messaging functionality"
    tests:
      - name: "private_message"
        <<: *channel_op
        vars:
          target_user: "other_user"
        setup:
          - authenticate: true
          - create_user: "{target_user}"
        commands:
          - "PRIVMSG {target_user} :Test message"
        expect: "PRIVMSG"
        cleanup:
          - remove_user: "{target_user}"

  - name: "Complex Scenarios"
    description: "Multi-step test scenarios"
    tests:
      - name: "channel_interaction"
        <<: *channel_op
        vars:
          other_user: "other_test_user"
        setup:
          - authenticate: true
          - join_channel: "#test"
          - create_user: "{other_user}"
          - user_join_channel: "{other_user} #test"
        commands:
          - "PRIVMSG #test :Hello"
          - "MODE #test +o {other_user}"
          - "WHO #test"
        expect:
          - "PRIVMSG #test"
          - "MODE #test +o"
          - "352"  # WHO reply
        cleanup:
          - remove_user: "{other_user}"
          - part_channel: "#test"

  - name: "Error Handling"
    description: "Tests for error conditions"
    tests:
      - name: "invalid_channel_join"
        <<: *auth_command
        commands:
          - "JOIN #invalid*channel"
        expect: "403"  # No such channel

      - name: "missing_params"
        <<: *auth_command
        commands:
          - "JOIN"
        expect: "461"  # Not enough parameters

  - name: "Mode Commands"
    description: "Tests for mode settings"
    tests:
      - name: "channel_modes"
        <<: *channel_op
        commands:
          - "MODE #test +n"
          - "MODE #test +t"
          - "MODE #test +i"
        expect: "MODE #test"

      - name: "user_modes"
        <<: *auth_command
        commands:
          - "MODE test_user +i"
        expect: "MODE test_user"

# Special test scenarios
special_cases:
  - name: "Rate Limiting"
    description: "Tests for server rate limiting"
    type: AUTH
    setup:
      - authenticate: true
    commands:
      - repeat: 10
        command: "PRIVMSG #test :Test message"
        interval: 0.1
    expect: "263"  # Rate limit exceeded

  - name: "Long Operations"
    description: "Tests for long-running operations"
    type: AUTH
    setup:
      - authenticate: true
      - create_channels: 50  # Create 50 test channels
    commands:
      - "LIST"
    expect: "323"  # End of list
    timeout: 30

# Error code reference
error_codes:
  "001": "Welcome message"
  "311": "WHOIS user reply"
  "318": "End of WHOIS"
  "332": "Topic message"
  "353": "Names reply"
  "366": "End of names"
  "403": "No such channel"
  "461": "Not enough parameters"
  "464": "Password incorrect"
```

---

**Instructions:**

- Use the above structure to generate a complete `tests.yaml` file.
- Ensure that each test case is well-defined with clear commands and expected responses.
- Include setup and cleanup phases where necessary to ensure a clean testing environment.
- Utilize reusable templates to avoid redundancy and maintain consistency.
- Provide detailed descriptions for each test suite and test case to clarify the purpose and expected outcomes.
- Add special test scenarios to cover edge cases and extended operations.
- Ensure the error code reference section is comprehensive and accurate.
