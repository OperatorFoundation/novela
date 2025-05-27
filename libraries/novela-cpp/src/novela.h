//
// Created by Dr. Brandon Wiley on 5/13/25.
//

#ifndef NOVELA_H
#define NOVELA_H

#include <cstdint>
#include <optional>
#include <vector>

#include "canvas.h"

class Novela
{
  public:
    static constexpr int display = 1;
    static constexpr int esc = 2;
    static constexpr int command = 3;
    static constexpr int modeset = 4;

    explicit Novela(Canvas& canvas) : canvas(canvas) {}

    void process(uint8_t c);

  private:
    Canvas& canvas;
    int mode = display;
    std::vector<std::optional<unsigned int>> parameters;

    void process_display(uint8_t c);
    void process_command(uint8_t c);
    void process_modeset(uint8_t c);

    void process_control(uint8_t c);
    void process_space();
    void process_printable(uint8_t c);
    void process_backspace();
    void process_high(uint8_t c);
    void process_escape();
    void process_parameter_digit(uint8_t c);

    void setMode(int parameter, int onOff);

    bool checkWrap();
    bool checkScroll();
    void scroll();
};

#endif //NOVELA_H
