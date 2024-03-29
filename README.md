

# FILE EXPLORER APPLICATION


This File Explorer is compatible with any [UNIX](https://unix.org) based Operating System.

![alt text](https://github.com/Mayhem-29/Terminal_Based_File-Explorer/blob/main/explorer.jpg)

## Installation and Execution

Use the BASH Terminal to install C++.

```bash
sudo apt-get install g++
```
Compile `main.cpp` using 
```bash
c++ main.cpp
```
Then Execute using
```bash
./a.out
```

## Modes
This application will work in two modes :

 1 .`Normal Mode` : used to explore the current directory and navigate the filesystem.

2 .`Command Mode` : used to enter and execute shell commands.

The **Home** of the application is the directory where the application will start.
**Root** will be System Root i.e. `/home/username/`

## Switching Between Modes
Press `:` to switch from **NORMAL MODE** to **COMMAND MODE**.
Press `esc` to switch from **COMMAND MODE** to **NORMAL MODE**.

## Command Mode
Unless a Directory is Specified in the end of command. All commands will work on Current Working Directory.

1. `Copy` - copy <source_file(s)> <destination_directory> or <source_directory(s)> <destination_directory> 

2. `Move` - move <source_file(s)> <destination_directory>

3. `Rename` - rename <old_filename> <new_filename>

4. `Create File` - create_file <file_name> <destination_path>

5. `Create Directory` - create_dir <dir_name> <destination_path>

6. `Delete File or Directory ` - delete <file_path> or <dir_path> (<directory_to_delete_in>)

7. `Goto` - goto <location>

8. `Search` - search <file_name> or <directory_name>  (<directory_to_search_in>)

## Normal Mode
Number of Directory Contents visible at a time can be changed by changing `MAX` macro in the code.
1. `↑` `↓`  to Navigate  the contents of the Directory 
2. `←` `→`  to Navigate between previously visited Directories.
3. `w` `a` `s` `d` to freely move the cursor and move Display Window to unseen contents.
4. `BACKSPACE` to go one Level Up the Current Working Directory.
5. `h` to goto **HOME** Directory.
6. `ENTER` to Open a Directory or a File currently pointed by the Cursor.
7. `q` to Quit the application.
8. `k` `l` to scroll contents up and down respectively (Not a recommended way of navigation).



# Assumptions

- I have considered "/home/user" as root and home directory.
- Upon pressing "h",program return to "/home/user".



# Command Mode

- Only last 2 rows of the screen have been used for command mode.
- Upon pressing ESC, it returns back to to normal mode.
- If "q" is pressed as first character in command line,program will get terminated.
- On "GOTO" command, program will open required directory and come back to command mode.
- Upon pressing UP/DOWN in command mode, cursor will help to navigate the CWD.
- Command will works only if paths/commands/directory/file names are VALID.









