# Zep - Vim-like Text Editor

## Overview
Zep is a terminal-based text editor designed with Vim-like controls, but built entirely from scratch without any standard libraries. It provides a modal editing experience, rich with unique features and customization options.

## Controls

### Modes
- **Command Line Mode**: Type `/` to open the command line.
- **Insert Mode**: Type `i` to enter Insert Mode.
- **Normal Mode**: Press `ESC` to exit Insert Mode.

### Cursor Movement
- `hjkl` or arrow keys: Move the cursor.
- `HJKL`: Jump 24 lines and center on the screen.
- `w`: Jump forward until whitespace.
- `b`: Jump backward until whitespace.
- `o`: Move cursor down and start a new line.
- `O`: Move cursor up and start a new line.
- 'f': Find forwards in line.
- 'F': Find backwards in line.

### Editing Commands
- `x`: Delete the current character.
- `a`: Move cursor right, then enter Insert Mode.
- `s`: Delete the character and enter Insert Mode.
- `f[letter]`: Find `[letter]` in the current line.
- `F[letter]`: Find `[letter]` in the current line, searching backward.
- `c`: Center the current line on the screen.
- `u`: Undo (persistent across the session).
- `r`: Redo (to the newest change).

### Quick Commands
- `CTRL+q`: Quick exit (without saving).
- `CTRL+s`: Quick save.

### Command Line Commands
- `/theme`: Change theme. Available themes: `light`, `dark`, `default1` to `default5`, `gptcolors`, `retro_orange`, `retro_green`.
- `/w`: Save the file.
- `/q`: Quit.
- `/e`: Edit a file.

## Features
- **Syntax Highlighting**: Supports single-line comments and basic C syntax. Plans to dynamically adjust syntax based on file extension.
- **Themes**: Multiple themes available with support for dynamic switching based on time of day.
- **Undo/Redo**: Fully functional and memory-safe.
- **Multiple Buffers**: Support for managing multiple files/windows (work in progress).
- **Scrolling**: Smooth horizontal and vertical scrolling.
- **Status Bar**: Displays helpful information about the current session.

## Planned Features
- Vim-style `<leader>` commands for faster workflows.
- Motion commands like `10j`, `dw`, `db`, etc.
- Visual Line Mode and Zep-only buffer for copy-pasting.
- Enhanced status bar with spillover support.
- Keystroke tracker.

## Known Issues and Notes
- No copy-paste functionality yet.
- Some command sequences may need rework to support motion commands.
- Flickering issues during fast typing may occur.

## Development Log

### Commit History
#### Commit #30
- Started work on an undo/redo tree and added support for multiple buffers.
- Enhanced status bar with more accurate write size notifications.
- Fixed an issue where `s` caused crashes in normal mode.
- Added `CTRL+Q` for quick exit.

#### Commit #29
- Fixed scrolling bugs related to `w` and `L`.
- Resolved a bug when typing at the end of a line.
- Added `CTRL+S` for quick save and introduced a dark theme.
- Now tracks bytes added and time of the last write.

#### Commit #28
- Updated status bar for better readability.
- Added `c` and `JK` to center the screen on the current line.
- Cleaned up normal mode checks, allowing for future modularization.

#### Commit #27
- Fixed memory leaks and resolved a `yoffset` bug.
- Corrected `oO` behavior and added `H` and `L` for jumping to start/end of lines.
- Adjusted `JK` to jump up/down 20 lines while centering the screen.

#### Commit #26
- Fixed `fF` behavior and scrolling issues.

#### Commit #25
- Changed command leader from `:` to `/` for easier use.
- Added `/b` to jump to the bottom and `/t` to jump to the top of the file.
- Introduced `/theme` for theme switching and `/themes?` for listing themes in a separate window.

#### Commit #24
- Added support for `/t` and `/theme` commands.
- Reorganized theme management for scalability.
- Introduced several new themes, including `default1` to `default5` and `gptcolors`.

#### Commit #23
- Rewrote `drawbuffer` for faster performance and horizontal scrolling.
- Improved the logic for highlighting substrings.
- Added `/syntax` for changing syntax highlighting dynamically.

#### Commit #22
- Reduced tearing during scrolling.

#### Commit #21
- Updated color schemes and enhanced status bar.
- Improved support for multiple windows.

#### Commit #20
- Introduced syntax highlighting for C syntax and single-line comments.
- Began work on dynamically setting syntax based on file extension.
- Improved color scheme support.

#### Commit #19
- Added left padding for scrolling.
- Reduced flickering during fast typing.
- Enhanced status bar functionality.

#### Commit #18
- Overhauled code structure for better maintainability and scalability.
- Adjusted scrolling behavior with added padding.

#### Commit #17
- Began implementing syntax highlighting.
- Fixed cursor behavior at line ends.
- Improved typing behavior.

#### Commit #16
- Made minor bug fixes.
- Planned future support for motion commands and tab handling.
- Initiated a structural overhaul for easier maintenance.

#### Commit #15
- Fixed `fF` scrolling issues.
- Enhanced handling of basic commands and introduced `:e` for file editing.

#### Commit #14
- Added `fF` and `oO` commands.
- Improved readability of normal mode controls.

#### Commit #13
- Changed color schemes and reorganized project files.
- Added support for `x` and `s` in normal mode.
- Removed cursor wrapping for smoother operation.
- Improved status bar functionality.

#### Commit #12
- Introduced `w`, `q`, `q!`, and `wq` commands.
- Added the ability to create new files with `./editor [new filename]`.

#### Commit #11
- Introduced a basic status bar and improved color support.
- Added a `saveas` command.

#### Commit #10
- Completed general cursor movement functionality.
- Added basic modal editing with normal and insert modes.
- Enabled `hjkl`, `w`, and `b` for cursor movement.

#### Commit #9
- Fixed scrolling bugs and improved line numbering.
- Displayed `~` for empty lines.

#### Commit #8
- Fixed issues with creating new lines at the bottom of the screen.

#### Commit #7
- Added functionality for deleting and creating new lines.
- Improved color support for better readability.

#### Commit #6
- Implemented horizontal scrolling and fixed line snapping.
- Introduced easy logging functionality.

#### Commit #5
- Made the editor "spam-proof" by addressing input synchronization issues.
- Fixed cursor behavior during fast input.

#### Commit #4
- Introduced basic horizontal scrolling.

#### Commit #3
- Improved line snapping for better navigation.
- Fixed flickering issues during scrolling.

#### Commit #2
- Added vertical scroll support.
- Planned future implementations for newline and tab insertion.

#### Commit #1
- Initial commit with basic buffer handling and cursor movement.
- Fixed issues with handling simultaneous key presses.

## Installation
Currently, Zep is built from source. Ensure you have a C compiler and terminal emulator to use it.

## Contribution
Contributions are welcome! Feel free to submit issues or pull requests to improve Zep.

thanks chatgpt for formatting this nicely
