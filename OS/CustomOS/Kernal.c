#include <stdbool.h>    // Include boolean type support
#include <stddef.h>     // Include standard definition types
#include <stdint.h>     // Include standard integer types
#include <string.h>     // Include string manipulation functions
#include <time.h>       // Include time functions
#include <stdlib.h>      

#define VGA_ADDRESS 0xB8000  // Base address for VGA text mode memory
#define VGA_WIDTH 80         // Number of characters per row in VGA text mode
#define VGA_HEIGHT 25        // Number of rows in VGA text mode

// Terminal interface state
static size_t terminal_row;    // Current row position for text output
static size_t terminal_column; // Current column position for text output
static uint8_t terminal_color; // Current text color
static uint16_t* terminal_buffer; // Pointer to the VGA text mode buffer

// Function to initialize the terminal
void terminal_initialize() {
    terminal_row = 0;          // Start at the first row
    terminal_column = 0;       // Start at the first column
    terminal_color = 0x07;     // Default text color: light grey on black
    terminal_buffer = (uint16_t*) VGA_ADDRESS;  // Point to the VGA buffer

    // Clear the screen by setting every cell to a space with the default color
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;  // Calculate the buffer index
            terminal_buffer[index] = (uint16_t) ' ' | (uint16_t) terminal_color << 8;  // Set character and color
        }
    }
}

// Function to write a string to the terminal
void terminal_writestring(const char* data) {
    for (size_t i = 0; data[i] != '\0'; i++) {  // Iterate over each character in the string
        const size_t index = terminal_row * VGA_WIDTH + terminal_column;  // Calculate the buffer index
        terminal_buffer[index] = (uint16_t) data[i] | (uint16_t) terminal_color << 8;  // Set character and color
        if (++terminal_column == VGA_WIDTH) {  // Move to the next column
            terminal_column = 0;  // If at the end of the row, move to the next row
            if (++terminal_row == VGA_HEIGHT) {  // If at the end of the screen, start from the top
                terminal_row = 0;
            }
        }
    }
}

__declspec(dllexport) void handle_command(const char* command) {
    // Bridge between C# and C kernel
    extern void kernel_handle_command(const char* cmd);
    kernel_handle_command(command);
}

// Function to print the help message
void print_help() {
    terminal_writestring("Available commands:\n");
    terminal_writestring("echo [text] - Echo the input text\n");
    terminal_writestring("cls/clear - Clear the screen\n");
    terminal_writestring("dir/ls - List directory contents (not implemented)\n");
    terminal_writestring("help - Display this help message\n");
    terminal_writestring("date - Display the current date\n");
    terminal_writestring("time - Display the current time\n");
    terminal_writestring("shutdown - Shutdown the system (not implemented)\n");
    terminal_writestring("reboot - Reboot the system (not implemented)\n");
    terminal_writestring("cd [dir] - Change directory (not implemented)\n");
    terminal_writestring("mkdir [dir] - Create a directory (not implemented)\n");
}

// Function to print the current date (simulated)
void print_date() {
    struct tm current_time;  // Struct to hold the current time
    current_time.tm_year = 2024 - 1900;  // Set the year (years since 1900)
    current_time.tm_mon = 11;  // Set the month (0-based, so 11 = December)
    current_time.tm_mday = 11;  // Set the day of the month
    char buffer[30];
    strftime(buffer, 30, "Current date: %Y-%m-%d\n", &current_time);  // Format the date into the buffer
    terminal_writestring(buffer);  // Print the formatted date
}

// Function to print the current time (simulated)
void print_time() {
    struct tm current_time;  // Struct to hold the current time
    current_time.tm_hour = 13;  // Set the hour (24-hour format)
    current_time.tm_min = 25;   // Set the minutes
    current_time.tm_sec = 0;    // Set the seconds
    char buffer[30];
    strftime(buffer, 30, "Current time: %H:%M:%S\n", &current_time);  // Format the time into the buffer
    terminal_writestring(buffer);  // Print the formatted time
}

// Function to handle different commands
void handle_command(const char* command) {
    if (strncmp(command, "echo ", 5) == 0) {  // Check if the command starts with "echo "
        terminal_writestring(command + 5);  // Echo the text following "echo "
        terminal_writestring("\n");
    } else if (strcmp(command, "cls") == 0 || strcmp(command, "clear") == 0) {  // Check if the command is "cls" or "clear"
        terminal_initialize();  // Clear the screen
    } else if (strcmp(command, "dir") == 0 || strcmp(command, "ls") == 0) {  // Check if the command is "dir" or "ls"
        terminal_writestring("Directory listing not implemented\n");
    } else if (strcmp(command, "help") == 0) {  // Check if the command is "help"
        print_help();  // Show help
    } else if (strcmp(command, "date") == 0) {  // Check if the command is "date"
        print_date();  // Show date
    } else if (strcmp(command, "time") == 0) {  // Check if the command is "time"
        print_time();  // Show time
    } else if (strcmp(command, "shutdown") == 0) {  // Check if the command is "shutdown"
        terminal_writestring("Shutdown command not implemented\n");
    } else if (strcmp(command, "reboot") == 0) {  // Check if the command is "reboot"
        terminal_writestring("Reboot command not implemented\n");
    } else if (strncmp(command, "cd ", 3) == 0) {  // Check if the command starts with "cd "
        terminal_writestring("Change directory command not implemented\n");
    } else if (strncmp(command, "mkdir ", 6) == 0) {  // Check if the command starts with "mkdir "
        terminal_writestring("Create directory command not implemented\n");
    } else {
        terminal_writestring("Unknown command\n");  // Handle unknown commands
    }
}

// Main function to initialize the terminal and simulate command handling
void kernel_main() {
    terminal_initialize();  // Initialize the terminal (clear the screen)
    terminal_writestring("Welcome to My Simple OS!\n");  // Print a welcome message

    // Simulate handling of some commands (you would handle keyboard input in a real OS)
    handle_command("echo Hello, World!");  // Echo command
    handle_command("cls");  // Clear screen command
    handle_command("help");  // Help command
    handle_command("date");  // Date command
    handle_command("time");  // Time command
}