import pygame
import sys
import os
import subprocess
import shlex
import threading
import queue


class PygameTerminal:
    def __init__(self, width=800, height=600):
        # Initialize Pygame
        pygame.init()

        # Screen setup
        self.width = width
        self.height = height
        self.screen = pygame.display.set_mode((width, height))
        pygame.display.set_caption("Pygame Terminal Emulator")

        # Colors
        self.BLACK = (0, 0, 0)
        self.WHITE = (255, 255, 255)
        self.GREEN = (0, 255, 0)
        self.YELLOW = (255, 255, 0)
        self.GRAY = (50, 50, 50)

        # Fonts
        self.font = pygame.font.Font(pygame.font.match_font('couriernew'), 16)

        # Terminal state
        self.current_path = r"C:\Users\owner"  # Original initial path

        # Predefined paths array (from original code)
        self.paths_array = ["Users", "Documents", "Videos", "Music", "Desktop"]

        # Directory-specific lists
        self.directories = {
            "Desktop": [],
            "Documents": [],
            "Music": [],
            "Videos": []
        }

        # Command history and input
        self.command_history = []
        self.max_history = 100
        self.input_text = ""

        # Input handling
        self.cursor_visible = True
        self.cursor_timer = 0

        # Command processing
        self.command_queue = queue.Queue()
        self.command_thread = threading.Thread(target=self.process_commands, daemon=True)
        self.command_thread.start()

        # Scrolling
        self.scroll_offset = 0
        self.line_height = 20

        # Clock for controlling frame rate
        self.clock = pygame.time.Clock()

    def render_text(self, text, color=None):
        """Render text to the screen"""
        color = color or self.WHITE
        return self.font.render(text, True, color)

    def draw_terminal(self):
        """Draw the terminal interface"""
        # Clear screen
        self.screen.fill(self.BLACK)

        # Draw command history
        start_index = max(0, len(self.command_history) - self.height // self.line_height)
        for i, line in enumerate(self.command_history[start_index:]):
            # Color code different types of output
            if line.startswith(">>"):
                text_surface = self.render_text(line, self.YELLOW)
            elif line.startswith("Error:"):
                text_surface = self.render_text(line, (255, 0, 0))
            else:
                text_surface = self.render_text(line)

            self.screen.blit(text_surface, (10, i * self.line_height))

        # Draw current input line
        prompt = f"{self.current_path}> {self.input_text}"
        input_surface = self.render_text(prompt, self.GREEN)
        self.screen.blit(input_surface, (10, self.height - 30))

        # Draw blinking cursor
        if self.cursor_visible:
            cursor_x = 10 + self.font.size(f"{self.current_path}> {self.input_text}")[0]
            cursor_surface = self.render_text("|", self.WHITE)
            self.screen.blit(cursor_surface, (cursor_x, self.height - 30))

        pygame.display.flip()

    def handle_events(self):
        """Handle pygame events"""
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                return False

            # Keyboard input
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_RETURN:
                    # Submit command
                    if self.input_text.strip():
                        self.command_queue.put(self.input_text)
                        self.command_history.append(f"{self.current_path}> {self.input_text}")

                        # Manage history length
                        if len(self.command_history) > self.max_history:
                            self.command_history.pop(0)

                        self.input_text = ""

                elif event.key == pygame.K_BACKSPACE:
                    # Remove last character
                    self.input_text = self.input_text[:-1]

                elif event.unicode and event.unicode.isprintable():
                    # Add printable characters
                    self.input_text += event.unicode

        return True

    def process_commands(self):
        """Process commands in a separate thread"""
        while True:
            try:
                # Get command from queue
                command = self.command_queue.get()

                # Process the command
                output = self.execute_command(command)

                # Add output to history if not empty
                if output:
                    for line in output.split('\n'):
                        if line.strip():
                            self.command_history.append(line)

                # Mark queue task as done
                self.command_queue.task_done()
            except Exception as e:
                self.command_history.append(f"Error: {str(e)}")

    def list_directory(self, path=None):
        """List contents of a directory"""
        try:
            # Use current path if no path specified
            target_path = path or self.current_path

            # Verify path exists and is a directory
            if not os.path.exists(target_path):
                return f"Path not found: {target_path}"

            if not os.path.isdir(target_path):
                return f"Not a directory: {target_path}"

            # Get directory contents
            contents = os.listdir(target_path)

            # Prepare output
            output_lines = [f">> Directory contents of {target_path}:"]
            for item in contents:
                # Get full path of item
                full_path = os.path.join(target_path, item)

                # Determine item type
                if os.path.isdir(full_path):
                    output_lines.append(f"[DIR]  {item}")
                elif os.path.isfile(full_path):
                    output_lines.append(f"[FILE] {item}")
                else:
                    output_lines.append(f"[LINK] {item}")

            return "\n".join(output_lines)

        except Exception as e:
            return f"Error listing directory: {str(e)}"

    def execute_command(self, msg):
        """Execute terminal commands with original logic and new features"""
        try:
            # echo command
            if msg.startswith("echo "):
                return msg[5:]

            # manual command
            elif msg == "man ls":
                return (
                    "This is the manual to access it you type 'man ls'\n"
                    "echo - echo can be used to print any word, phrase, or character the user inputs\n"
                    "cd - cd can be used to change the path\n"
                    "'..'' - '..' can be used along side the 'cd' command to go back a directory\n"
                    "ls - list contents of current or specified directory\n"
                    "exit - the exit command can be used to terminate the terminal"
                )

            # list directory
            elif msg == "ls":
                return self.list_directory()

            # list directory with path
            elif msg.startswith("ls "):
                path = msg[3:].strip()
                return self.list_directory(path)

            # exit command
            elif msg == "exit":
                pygame.quit()
                sys.exit()

            # change directory
            elif msg.startswith("cd "):
                desired_path = msg[3:]

                # Check for invalid characters
                if "/" in desired_path:
                    return "'/' is not a valid character when changing the path"

                # Handle going back a directory
                if desired_path == "..":
                    path_parts = self.current_path.split("\\")
                    if len(path_parts) > 1:
                        self.current_path = "\\".join(path_parts[:-1])

                    # Ensure we don't go beyond root
                    if not self.current_path:
                        self.current_path = "C:\\"

                    return f"Changed path to: {self.current_path}"

                # Change to specified directory
                else:
                    # Construct full path
                    try:
                        full_path = os.path.abspath(os.path.join(self.current_path, desired_path))

                        # Verify path exists and is a directory
                        if os.path.exists(full_path) and os.path.isdir(full_path):
                            self.current_path = full_path
                            return f"Changed path to: {self.current_path}"
                        else:
                            return f"Directory not found: {desired_path}"
                    except Exception as e:
                        return f"Error changing directory: {str(e)}"

            # Unknown command
            else:
                return f"Unknown command >>: {msg}"

        except Exception as e:
            return f"Error executing command: {str(e)}"

    def run(self):
        """Main game loop"""
        running = True

        # Welcome message
        self.command_history.append("Welcome to Pygame Terminal Emulator!")
        self.command_history.append("Type 'man ls' for help, 'exit' to quit")

        while running:
            # Handle events
            running = self.handle_events()

            # Cursor blinking
            self.cursor_timer += 1
            if self.cursor_timer > 30:
                self.cursor_visible = not self.cursor_visible
                self.cursor_timer = 0

            # Draw terminal
            self.draw_terminal()

            # Control frame rate
            self.clock.tick(30)

        # Cleanup
        pygame.quit()
        sys.exit()


def main():
    terminal = PygameTerminal()
    terminal.run()


if __name__ == '__main__':
    main()