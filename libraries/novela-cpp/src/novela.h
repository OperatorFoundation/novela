//
// Created by Dr. Brandon Wiley on 5/26/25.
//

#ifndef NOVELA_H
#define NOVELA_H

#include <bell.h>
#include <cstdint>
#include <cursor.h>
#include <optional>
#include <vector>

#include <vterm.h>
#include "canvas.h"
#include "Connection.h"
#include "clock.h"
#include "logger.h"

struct ScrollbackLine
{
  VTermScreenCell* cells;
  int cols;
};

class Novela
{
  public:
    static Novela *instance;

    static const int MAX_SCROLLBACK_LINES = 100;

    Canvas& canvas;
    Connection* connection;
    Clock& clock;
    Logger& logger;

    Cursor *cursor = nullptr;
    std::optional<Bell> bell = std::nullopt;

    VTerm *vt;
    VTermScreen *screen;
    VTermScreenCallbacks screen_callbacks;
    std::vector<ScrollbackLine> scrollback_buffer;

    explicit Novela(Canvas& canvas, Connection* connection, Clock& clock, Logger& logger, Cursor *cursor);

    void begin();
    void process(std::vector<char> bs);
    void processCharacter(char c);

    void setTitle(std::string newTitle);
    void update();

  private:
    enum class Mode
    {
      NORMAL = 0,
      ESC = 1,
      CSI = 2,
      OSC = 3,
      SPACE = 4,
      CHARSET = 5,
      DEC_SPECIAL = 6,
      ENCODING = 7
    };

    Mode mode = Mode::NORMAL;
    std::vector<char> buffer;

    std::string title;;
};

int redraw(VTermRect rect, void *user);
int move(VTermRect dest, VTermRect src, void *user);
int move_cursor(VTermPos pos, VTermPos oldpos, int visible, void *user);
int bell_rung(void *user);
int set_prop(VTermProp prop, VTermValue *val, void *user);
void on_output(const char *s, size_t len, void *user);
int pushline(int cols, const VTermScreenCell *cells, void *user);
int popline(int cols, VTermScreenCell *cells, void *user);

#endif //NOVELA_H
