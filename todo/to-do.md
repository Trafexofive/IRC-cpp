
### [TASKS]

- Implement core event handling. [DONE]
  - Ensure proper event loop.
  - Handle all core events like connect, disconnect, and message receive.

- Start implementing basic CMDs. [DONE] [UNSTABLE]
  - Add initial set of commands like JOIN, PART, and PRIVMSG.
  - Test for basic command functionality.

- RFC-compliant authentication. [URGENT] [DONE]
  - Follow RFC standards for user authentication.
  - Ensure secure handling of user credentials.

- Add 3 more commands. [PENDING]
  - Implement additional commands like WHO, LIST, and TOPIC.
  - Ensure proper parsing and handling as per RFC.

- Fix the Python tester. [PENDING]
  - Debug and resolve issues in the Python testing script.
  - Ensure it covers all test cases.

- Over-engineered tester test/main.sh [DONE] [TESTED]
  - Create a comprehensive test script.
  - Ensure all edge cases are covered.

- Simple interactive client script for some user testing. [DONE] [UNSTABLE] [TESTED]
  - Develop a basic client script for user interaction.
  - Test for stability and usability.

- Refine and clean-up code. [PENDING]
  - Perform code refactoring.
  - Remove any redundant or unused code.

- Complete unfinished bot implementations. [PENDING] [UNMAKEABLE]
  - Finish coding the bot features.
  - Ensure they are functioning as expected.

- Implement debug.hpp for proper error handling. [PENDING] [FLEX]
  - Create a debug header for error logging and handling.
  - Ensure it integrates well with the existing codebase.

- Industry standards missing (proper -h and flag functionalities; check subject). [PENDING] [FLEX]
  - Add command-line flags and help options.
  - Ensure compliance with industry standards.

- Add test-cases.txt and add its implementation to main.sh [PENDING] [FLEX]
  - Document all test cases.
  - Integrate them into the main testing script.

- Finish Dockerfile, add docker-compose. [PENDING] [FLEX]
  - Complete the Dockerfile for containerization.
  - Create a docker-compose file for easier setup.

- Fix all ANSI issues (.log files, tester stdout...), no way to get around if this device doesn't support; best we can do is check and disable. [PENDING]
  - Resolve ANSI compatibility issues.
  - Implement checks to disable ANSI where not supported.

- Channels. [DONE] [MANDATORY] [UNSTABLE]
- [COMMANDS] :
    - CAP LS behavior. (modern irc auth compliance)
    - full channel operator support ()
    - 
    - 
- [URGENT] need to overhall client and channel specific methodes (helpers), they are slowing command developement.

### [BUGS] use location-line : {test} when reporting bugs

- Example: inc/test.hpp-43:
  ```bash
  #bin/bash
  test-case
  ```

### [FEATURES]

- Add more advanced IRC commands.
- Introduce plugin support for custom commands.
- Develop a web-based interface for the server. [LOL]

### [TLDR;]

- Core event handling and basic commands implemented.
- Authentication completed as per RFC standards.
- Pending tasks include adding more commands, fixing the tester, and refining the code.
- Bugs should be reported with the format {location-line}.
- Future features include advanced commands, GUI client, and plugin support.

