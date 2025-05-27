//
// Created by Dr. Brandon Wiley on 5/26/25.
//

#ifndef NOVELA_VTERM_H
#define NOVELA_VTERM_H

#include <bell.h>
#include <cstdint>
#include <cursor.h>
#include <optional>
#include <vector>

#include <vterm.h>
#include "canvas.h"
#include "Connection.h"
#include "clock.h"

class NovelaVterm
{
  public:
    static NovelaVterm *instance;

    Canvas& canvas;
    Connection& connection;
    Clock& clock;
    std::optional<Cursor> cursor = std::nullopt;
    std::optional<Bell> bell = std::nullopt;

    VTerm *vt;
    VTermScreen *screen;
    VTermScreenCallbacks screen_callbacks;

    explicit NovelaVterm(Canvas& canvas, Connection& connection, Clock& clock);

    void process(uint8_t c);

    void setTitle(std::string newTitle);
    void update();

  private:
    std::string title;;
};

int output_callback(const char *bytes, size_t len, void *user);
int damage_callback(VTermRect rect, void *user);
int movecursor_callback(VTermPos pos, VTermPos oldpos, int visible, void *user);
int bell_callback(void *user);
int screen_settermprop(VTermProp prop, VTermValue *val, void *user);
void on_output(const char *s, size_t len, void *user);

#endif //NOVELA_VTERM_H
