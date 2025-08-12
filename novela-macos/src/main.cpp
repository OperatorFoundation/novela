// novela-macos/src/main.cpp
#include <lgfx/v1/platforms/sdl/Panel_sdl.hpp>
#if defined(SDL_h_)

#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <cstring>

#define LGFX_USE_V1
#include <LGFX_AUTODETECT.hpp>

#include <novela.h>
#include <ReliableConnectionMacOS.h>
#include "system_clock.h"                 // System clock instead of Arduino clock
#include "console_logger.h"               // Console logger instead of serial logger
#include "sdl_cursor.h"
#include <lgfx_sdl_canvas.h>

// Include caneta for keyboard handling
#include "caneta_sdl.h"
extern "C" {
#include "caneta.h"
}

// Create display instances - adjust size as needed
LGFX screen(800, 480);  // Terminal window size
LGFXSDLCanvas canvas = LGFXSDLCanvas();

// Global variables - will be initialized in main() with command line args
ReliableConnectionMacOS* serial = nullptr;
SystemClock ticker = SystemClock();
ConsoleLogger logger = ConsoleLogger();
SDLCursor* cursor = nullptr;
Novela* novela = nullptr;

// Caneta keyboard handling
caneta::SDLToHID* keyboard = nullptr;
extern "C" {
    extern kbd_state_t kbd_state;  // Declared in caneta.c
}

// State variables
int lastType = 0;
int waitTime = 0;
int counter = 0;
bool tap = false;
bool mouse_was_down = false;
bool should_exit = false;  // Add flag to signal exit
bool local_echo = false;    // Local echo mode (off by default for microcontrollers)
uint32_t last_keypress_time = 0;  // Track when last key was pressed

// Function to get current time in milliseconds (Arduino millis() equivalent)
uint64_t millis() {
    static auto start = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
}

// Function to generate random numbers (Arduino random() equivalent)
int random(int min, int max) {
    return min + (std::rand() % (max - min));
}

void tapped()
{
    canvas.nextTheme();
}

// Send VT100 escape sequence to serial port
void send_vt100_escape(const char* sequence)
{
    // Send ESC character
    serial->write({'\x1B'});
    // Send the sequence
    std::vector<char> seq_vec(sequence, sequence + strlen(sequence));
    serial->write(seq_vec);

    // Debug output
    std::string msg = "TX: ESC" + std::string(sequence);
    logger.debug(msg.c_str());
}

// Send single byte to serial port
void send_byte_to_serial(uint8_t byte)
{
    serial->write({static_cast<char>(byte)});

    // Debug output
    if (byte >= 32 && byte < 127) {
        logger.debugf("TX: '%c'", byte);
    } else {
        logger.debugf("TX: 0x%02X", byte);
    }
}

// Process HID report from caneta-sdl (same logic as RP2040)
void process_hid_report(const uint8_t* report, uint16_t len)
{
    if (len < 8) return;  // Standard keyboard report is 8 bytes

    // Byte 0: Modifier keys
    uint8_t modifiers = report[0];
    bool shift = (modifiers & 0x22) != 0;  // Left or right shift
    bool ctrl = (modifiers & 0x11) != 0;   // Left or right ctrl
    bool alt = (modifiers & 0x44) != 0;    // Left or right alt
    bool cmd = (modifiers & 0x88) != 0;    // Left or right GUI/Command

    kbd_state.shift_pressed = shift;
    kbd_state.ctrl_pressed = ctrl;
    kbd_state.alt_pressed = alt;

    // Check for newly pressed keys
    for (int i = 2; i < 8; i++) {
        uint8_t keycode = report[i];
        if (keycode == 0) continue;  // No key

        // Check if this is a new key press (not in last report)
        bool was_pressed = false;
        for (int j = 0; j < 6; j++) {
            if (kbd_state.last_keys[j] == keycode) {
                was_pressed = true;
                break;
            }
        }

        if (!was_pressed) {
            // New key press - process it

            // Check for hotkey combinations first
            if (cmd) {
                // ⌘+E toggles local echo
                if (keycode == 0x08) {  // 'E' key
                    local_echo = !local_echo;
                    logger.infof("Local echo %s", local_echo ? "ON" : "OFF");
                    std::cout << "\r\n[Local echo " << (local_echo ? "ON" : "OFF") << "]\r\n";
                    continue;  // Don't send this key to serial
                }
            }

            // Handle special keys
            const char* special_seq = process_special_keys(keycode);
            if (*special_seq) {  // If not empty string
                send_vt100_escape(special_seq);

                // Local echo for escape sequences (show cursor movement, etc.)
                if (local_echo) {
                    std::vector<char> echo_data;
                    echo_data.push_back('\x1B');
                    for (const char* p = special_seq; *p; p++) {
                        echo_data.push_back(*p);
                    }
                    novela->process(echo_data);

                    // Need to update to show cursor movement
                    novela->update();
                }
                continue;
            }

            // Handle regular keys using caneta function
            char ascii_char = hid_to_ascii(keycode, shift);

            if (ascii_char != 0) {
                // Handle Ctrl combinations
                if (ctrl && ascii_char >= 'a' && ascii_char <= 'z') {
                    ascii_char = ascii_char - 'a' + 1;  // Ctrl+A = 0x01, etc.
                } else if (ctrl && ascii_char >= 'A' && ascii_char <= 'Z') {
                    ascii_char = ascii_char - 'A' + 1;
                }

                // Send the character to serial
                send_byte_to_serial(ascii_char);

                // Local echo if enabled
                if (local_echo) {
                    std::vector<char> echo_data;
                    echo_data.push_back(ascii_char);

                    // Process the character through vterm
                    novela->process(echo_data);

                    // For printable characters, ensure the display is updated
                    // Call update twice - once to draw the character, once to position cursor
                    if (ascii_char >= 32 && ascii_char < 127) {
                        novela->update();  // Draw character
                        novela->update();  // Update cursor position
                    } else {
                        novela->update();  // Non-printable, just update once
                    }
                }
            }
        }
    }

    // Save current keys for next comparison
    memset(kbd_state.last_keys, 0, 6);
    for (int i = 2; i < 8; i++) {
        if (report[i] != 0) {
            kbd_state.last_keys[i-2] = report[i];
        }
    }
}

void setup()
{
    std::cout << "Hello, Operator." << std::endl;

    // Initialize random seed
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    std::cout << "Input/Output connections initialized" << std::endl;

    // Enable flow control and begin serial connection
    serial->enableXonXoff();
    serial->begin();

    std::cout << "Connections initialized" << std::endl;

    logger.setLevel(Logger::Level::INFO);
    std::cout << "Logger initialized" << std::endl;

    // Disable LovyanGFX keyboard shortcuts by requiring a modifier key
    // Use Command key (⌘) on macOS as the modifier for window controls
    lgfx::Panel_sdl::setShortcutKeymod(KMOD_LGUI);  // Left Command key on macOS
    // Now: ⌘+R/L to rotate, ⌘+1-6 to scale

    auto* panel = screen.getPanel();
    if (panel) {
        auto* sdl_panel = static_cast<lgfx::Panel_sdl*>(panel);
        sdl_panel->setWindowTitle("Novela");
        sdl_panel->setScaling(2, 2);  // 2x scaling

        // Optional: Add key mappings if you want to simulate GPIO buttons
        // For example: Map 'Q' key to GPIO 0, 'W' to GPIO 1, etc.
        // lgfx::Panel_sdl::addKeyCodeMapping(SDLK_q, 0);
    }

    screen.init();
    std::cout << "Screen initialized" << std::endl;

    canvas.begin(&screen);
    std::cout << "Canvas initialized" << std::endl;

    // Initialize caneta keyboard handler
    keyboard = new caneta::SDLToHID();
    keyboard->setReportCallback(process_hid_report);
    std::cout << "Keyboard handler initialized" << std::endl;

    // Call this last to ensure that everything is initialized before we start up the terminal.
    novela->begin();

    std::cout << "Novela initialized" << std::endl;
    std::cout << "setup() complete." << std::endl;
    std::cout << "Hotkeys:" << std::endl;
    std::cout << "  ⌘+E: Toggle local echo (currently OFF)" << std::endl;
    std::cout << "  ⌘+R/L: Rotate window" << std::endl;
    std::cout << "  ⌘+1-6: Scale window" << std::endl;

    // Clear screen equivalent
    std::cout << "\033[2J\033[H";
}

void loop()
{
    // LovyanGFX processes SDL events on the main thread
    // We can safely read the keyboard state that it maintains

    // Make sure SDL is initialized before trying to read keyboard state
    if (SDL_WasInit(SDL_INIT_VIDEO) && keyboard)
    {
        const Uint8* keystate = SDL_GetKeyboardState(nullptr);
        static Uint8 prev_keystate[SDL_NUM_SCANCODES] = {0};
        static bool first_run = true;

        // Skip first run to initialize prev_keystate
        if (first_run) {
            memcpy(prev_keystate, keystate, SDL_NUM_SCANCODES);
            first_run = false;
        } else {
            // Check for key press/release events by comparing states
            bool any_key_pressed = false;
            for (int scancode = 0; scancode < SDL_NUM_SCANCODES; scancode++) {
                if (keystate[scancode] != prev_keystate[scancode]) {
                    any_key_pressed = true;
                    SDL_Keycode keycode = SDL_GetKeyFromScancode((SDL_Scancode)scancode);

                    // Create a synthetic SDL event for caneta
                    SDL_Event fake_event;
                    memset(&fake_event, 0, sizeof(fake_event));

                    if (keystate[scancode]) {
                        // Key was just pressed
                        fake_event.type = SDL_KEYDOWN;
                        fake_event.key.state = SDL_PRESSED;

                        // Check for ESC to exit
                        if (keycode == SDLK_ESCAPE) {
                            should_exit = true;
                            return;
                        }
                    } else {
                        // Key was just released
                        fake_event.type = SDL_KEYUP;
                        fake_event.key.state = SDL_RELEASED;
                    }

                    fake_event.key.keysym.sym = keycode;
                    fake_event.key.keysym.scancode = (SDL_Scancode)scancode;
                    fake_event.key.keysym.mod = SDL_GetModState();
                    fake_event.key.repeat = 0;

                    // Process through caneta
                    keyboard->processEvent(fake_event);
                }
            }

            // Track typing for cursor management
            if (any_key_pressed && local_echo) {
                last_keypress_time = millis();
            }

            // Save current state for next frame
            memcpy(prev_keystate, keystate, SDL_NUM_SCANCODES);
        }
    }

    // Temporarily disable cursor blinking while typing with local echo
    if (local_echo && cursor) {
        uint32_t time_since_keypress = millis() - last_keypress_time;
        if (time_since_keypress < 500) {  // Within 500ms of last keypress
            cursor->setBlinking(false);
            cursor->setVisible(true);  // Keep cursor visible but not blinking
        } else {
            cursor->setBlinking(true);  // Resume blinking after 500ms of no typing
        }
    }

    // Read from serial connection (data coming from the device)
    std::vector<char> input_data = serial->read();
    if(!input_data.empty())
    {
        novela->process(input_data);
        // Echo to console for debugging
        std::cout.write(input_data.data(), input_data.size());
        std::cout.flush();
    }

    novela->update();

    // Handle mouse clicks as touch events (existing code)
    lgfx::touch_point_t touch_point;
    bool is_touched = screen.getTouch(&touch_point);

    if (is_touched && !tap)
    {
        std::cout << "Touch/Click started at (" << touch_point.x << ", " << touch_point.y << ")" << std::endl;
        tapped();
    }
    else if (!is_touched && tap)
    {
        std::cout << "Touch/Click ended" << std::endl;
    }

    tap = is_touched;

    // Small delay to prevent excessive CPU usage
    lgfx::delay(16); // ~60 FPS
}

// Required by LovyanGFX SDL
int user_func(bool* running)
{
    setup();
    do {
        loop();
        // Check if we should exit
        if (should_exit) {
            *running = false;
        }
    } while (*running);
    
    // Cleanup
    if (keyboard) {
        delete keyboard;
        keyboard = nullptr;
    }
    
    return 0;
}

int main(int argc, char* argv[])
{
    std::string device_path = "/dev/tty.usbserial-0001";  // Default device

    // Parse command line arguments
    if (argc > 1) {
        device_path = argv[1];
        std::cout << "Using serial device: " << device_path << std::endl;
    } else {
        std::cout << "Usage: " << argv[0] << " [serial_device]" << std::endl;
        std::cout << "Using default device: " << device_path << std::endl;
        std::cout << "Common macOS devices:" << std::endl;
        std::cout << "  /dev/tty.usbserial-*" << std::endl;
        std::cout << "  /dev/tty.usbmodem*" << std::endl;
        std::cout << "  /dev/cu.usbserial-*" << std::endl;
        std::cout << std::endl;
    }

    // Initialize global objects with the device path
    serial = new ReliableConnectionMacOS(device_path);
    cursor = new SDLCursor(ticker, canvas, logger, &screen);
    novela = new Novela(canvas, serial, ticker, logger, cursor);

    return lgfx::Panel_sdl::main(user_func);
}

#endif