// novela-macos/src/main.cpp
#include <lgfx/v1/platforms/sdl/Panel_sdl.hpp>
#if defined(SDL_h_)

#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <ctime>

#define LGFX_USE_V1
#include <LGFX_AUTODETECT.hpp>

#include <novela.h>
#include <ReliableConnectionMacOS.h>
#include "system_clock.h"                 // System clock instead of Arduino clock
#include "console_logger.h"               // Console logger instead of serial logger
#include "sdl_cursor.h"
#include <lgfx_sdl_canvas.h>

// Create display instances - adjust size as needed
LGFX screen(800, 480);  // Terminal window size
LGFXSDLCanvas canvas = LGFXSDLCanvas();

// Global variables - will be initialized in main() with command line args
ReliableConnectionMacOS* serial = nullptr;
SystemClock ticker = SystemClock();
ConsoleLogger logger = ConsoleLogger();
SDLCursor* cursor = nullptr;
Novela* novela = nullptr;

// State variables
int lastType = 0;
int waitTime = 0;
int counter = 0;
bool tap = false;
bool mouse_was_down = false;

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

    auto* panel = screen.getPanel();
    if (panel) {
        auto* sdl_panel = static_cast<lgfx::Panel_sdl*>(panel);
        sdl_panel->setWindowTitle("Novela");
        sdl_panel->setScaling(2, 2);  // 2x scaling
    }

    screen.init();
    std::cout << "Screen initialized" << std::endl;

    canvas.begin(&screen);
    std::cout << "Canvas initialized" << std::endl;

    // Call this last to ensure that everything is initialized before we start up the terminal.
    novela->begin();

    std::cout << "Novela initialized" << std::endl;
    std::cout << "setup() complete." << std::endl;

    // Clear screen equivalent
    std::cout << "\033[2J\033[H";
}

void loop()
{
    // Read from serial connection
    std::vector<char> input_data = serial->read();
    if(!input_data.empty())
    {
        novela->process(input_data);
        // Echo to console for debugging
        std::cout.write(input_data.data(), input_data.size());
        std::cout.flush();
    }

    novela->update();

    // For now, we'll comment out bidirectional communication
    // You might want to add keyboard input handling here later
    /*
    std::vector<char> keyboard_input = getKeyboardInput();
    if(!keyboard_input.empty())
    {
        serial->write(keyboard_input);
    }
    */

    // Debug code - uncomment to test random character generation
    /*
    if(lastType == 0)
    {
        lastType = millis();
    }
    else
    {
        if(millis() - lastType > waitTime)
        {
            lastType = millis();
            waitTime = random(1, 100) * 3;
            novela->process({static_cast<char>(random(32, 126))});
            counter = (counter + 1) % 10;
            novela->update();
        }
    }
    */

    // Handle mouse clicks as touch events
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
    } while (*running);
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