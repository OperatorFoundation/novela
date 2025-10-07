//
// Created by Dr. Brandon Wiley on 5/26/25.
//

#include "tela.h"

#include <cstring>

#define CHAR_WIDTH 8
#define CHAR_HEIGHT 8

Tela* Tela::instance = nullptr;

Tela::Tela(Canvas& canvas, Connection* connection, Clock& clock, Logger& logger, Cursor *cursor) : canvas(canvas), connection(connection), clock(clock), logger(logger), cursor(cursor)
{
  instance = this;
}

// The begin function allows for nothing much to happen until Arduino setup() is concluded.
// Therefore, this should be called at the end of setup().
void Tela::begin()
{
  int cols = canvas.getHeight() - 3;
  int rows = canvas.getWidth() - 3;

  logger.infof("vterm_new(%d, %d)", cols, rows);

  vt = vterm_new(cols, rows);
  if (!vt)
  {
    logger.debugf("fatal error, vterm_new failed");
    return;
  }

  // Set output callback
  vterm_output_set_callback(vt, on_output, nullptr);

  // Get screen interface
  screen = vterm_obtain_screen(vt);

  if(cursor)
  {
    cursor->setScreen(screen);
  }

  bell.emplace(Bell(canvas, clock));

  // Enable features
  // FIXME - implement altscreen support
  // vterm_screen_enable_altscreen(screen, 1);

  VTermState *state = vterm_obtain_state(vt);
  vterm_state_set_unrecognised_fallbacks(state, nullptr, nullptr);

  // Set callbacks
  screen_callbacks = {
    .damage = redraw,
    .moverect = move,
    .movecursor = move_cursor,
    .settermprop = set_prop,
    .bell = bell_rung,
    .resize = NULL, // Not relevant since this is a fixed-size display
    .sb_pushline = pushline,
    .sb_popline = popline
  };
  vterm_screen_set_callbacks(screen, &screen_callbacks, NULL);

  // Initialize screen
  vterm_screen_reset(screen, 1);
}

void Tela::process(std::vector<char> bs)
{
  logger.debug("TelaVterm::process");

  vterm_input_write(instance->vt, bs.data(), bs.size());
}

void Tela::processCharacter(char c)
{
  switch(mode)
  {
    case Mode::NORMAL:
      if(c == 0x1B) // ESC
      {
        buffer.push_back(c);
        mode = Mode::ESC;
      }
      else
      {
        vterm_input_write(instance->vt, &c, 1);
      }

      break;

    case Mode::ESC:
      buffer.push_back(c);

      switch(c)
      {
        case 0x1B:  // ESC ESC
          mode = Mode::NORMAL;
          vterm_input_write(instance->vt, buffer.data(), buffer.size());
          buffer.clear();
          break;

        case '[':
          mode = Mode::CSI;
          break;

        case ']':
          mode = Mode::OSC;
          break;

        case ' ':
          mode = Mode::SPACE;
          break;

        // Two-character sequences
        case 'D':
        case 'E':
        case 'H':
        case 'M':
        case 'N':
        case 'c':
        case 'l':
        case 'm':
        case 'n':
        case 'r':
        case 't':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '0':
        case '=':
        case '>':
        case '<':
          mode = Mode::NORMAL;
          vterm_input_write(instance->vt, buffer.data(), buffer.size());
          buffer.clear();
          break;

        case '(':
        case ')':
        case '*':
        case '+':
          mode = Mode::CHARSET;  // Need a new mode
          break;

        case '#':
          mode = Mode::DEC_SPECIAL;  // Need a new mode
          break;

        case '%':
          mode = Mode::ENCODING;  // Need a new mode
          break;

        default:
          // Unknown or invalid escape sequence
          logger.infof("Unknown escape sequence:");
          for(char b : buffer)
          {
            logger.infof("%02X", b);
          }
          logger.infof(".");

          mode = Mode::NORMAL;
          vterm_input_write(instance->vt, buffer.data(), buffer.size());
          buffer.clear();
          break;
      }

      break;

    case Mode::CSI:
      buffer.push_back(c);

      if(c >= 0x40 && c <= 0x7E) // The range of CSI sequence terminators.
      {
        mode = Mode::NORMAL;
        vterm_input_write(instance->vt, buffer.data(), buffer.size());
        buffer.clear();
      }
      break;

    case Mode::OSC:
      buffer.push_back(c);

      if(c == 0x07) // BEL
      {
        mode = Mode::NORMAL;
        vterm_input_write(instance->vt, buffer.data(), buffer.size());
        buffer.clear();
      }
      else if(buffer.size() > 2 && buffer[buffer.size()-2] == 0x1B &&
         buffer[buffer.size()-1] == '\\')
      {
        mode = Mode::NORMAL;
        vterm_input_write(instance->vt, buffer.data(), buffer.size());
        buffer.clear();
      }
      break;

    case Mode::SPACE:
      mode = Mode::NORMAL;
      buffer.push_back(c);
      vterm_input_write(instance->vt, buffer.data(), buffer.size());
      buffer.clear();

    case Mode::CHARSET:
    case Mode::DEC_SPECIAL:
    case Mode::ENCODING:
      buffer.push_back(c);
      mode = Mode::NORMAL;
      vterm_input_write(instance->vt, buffer.data(), buffer.size());
      buffer.clear();
      break;
  }
}

void Tela::setTitle(std::string newTitle)
{
  if(title.empty())
  {
    logger.debug("TelaVterm::setTitle: (empty)");
  }
  else
  {
    logger.debugf("TelaVterm::setTitle: %s", newTitle.c_str());

    // Erase old title, in the case the new title is shorter and doesn't completely overwrite the old title
    for(int i = 0; i < title.size(); i++)
    {
      canvas.drawCharacter(0, i + 1, ' ');
    }
  }

  title = newTitle;

  for(int i = 0; i < title.size(); i++)
  {
    canvas.drawCharacter(0, i + 1, title.at(i));
  }
}

void Tela::update()
{
  if(bell)
  {
    bell->update();
  }

  if(cursor)
  {
    cursor->update();
  }
}

// vterm event callbacks

// Update an area of the screen
int redraw(VTermRect rect, void *user)
{
  if(Tela::instance == nullptr)
  {
    return -1;
  }

  Tela::instance->logger.debugf("redraw %d:%d %d:%d", rect.start_col, rect.end_col, rect.start_row, rect.end_row);

  // A rectangle from (start_row,start_col) to (end_row,end_col) changed
  for (int row = rect.start_row; row < rect.end_row; row++)
  {
    for (int col = rect.start_col; col < rect.end_col; col++)
    {
      VTermPos pos = {.row = row, .col = col};
      VTermScreenCell cell;

      // Get what should be at this position
      int cellFilled = vterm_screen_get_cell(Tela::instance->screen, pos, &cell);

      // Set colors/attributes
      //if (cell.attrs.bold) Serial.print("\033[1m");
      //if (cell.attrs.underline) Serial.print("\033[4m");
      //if (cell.attrs.reverse) Serial.print("\033[7m");

      // Print the character
      if(cellFilled && cell.chars[0] != 0)
      {
        Tela::instance->logger.debugf("redraw:%c %d,%d", cell.chars[0], col, row);
        Tela::instance->canvas.drawCharacter(col + 1, row + 1, cell.chars[0]);
      }
      else
      {
        Tela::instance->logger.debugf("redraw:(empty) %d,%d", col, row);
        Tela::instance->canvas.drawCharacter(col + 1, row + 1, ' ');
      }
    }
  }

  return 1;
}

// Move the cursor
int move_cursor(VTermPos pos, VTermPos oldpos, int visible, void *user)
{
  if(Tela::instance == nullptr)
  {
    return -1;
  }

  Tela::instance->logger.debug("move_cursor begin");

  if(Tela::instance->cursor)
  {
    Tela::instance->cursor->setPosition(pos.col + 1, pos.row + 1);
  }

  Tela::instance->logger.debug("move_cursor end");

  return 1;
}

// Ring the bell
int bell_rung(void *user)
{
  if(Tela::instance == nullptr)
  {
    return -1;
  }

  Tela::instance->logger.debug("bell_rung");

  if(Tela::instance->bell)
  {
    Tela::instance->bell->ring();
    return 1;
  }
  else
  {
    return -2;
  }
}

// Set a terminal property
int set_prop(VTermProp prop, VTermValue *val, void *user)
{
  if(Tela::instance == nullptr)
  {
    return -1;
  }

  Tela::instance->logger.debug("set_prop");

  switch(prop)
  {
    case VTERM_PROP_CURSORVISIBLE:
    {
      if(Tela::instance->cursor)
      {
        Tela::instance->cursor->setVisible(val->boolean);
      }
      break;
    }

    case VTERM_PROP_CURSORBLINK:
      if(Tela::instance->cursor)
      {
        Tela::instance->cursor->setBlinking(val->boolean);
      }
      break;

    case VTERM_PROP_TITLE:
    {
      std::string s(val->string.str, val->string.len);
      Tela::instance->setTitle(s);
      break;
    }

    case VTERM_PROP_ICONNAME:
      break; // Ignoring the icon name is standard on modern implementations

    case VTERM_PROP_ALTSCREEN:
      break; // FIXME

    case VTERM_PROP_REVERSE:
      break; // FIXME

    case VTERM_PROP_MOUSE:
      break; // FIXME

    default:
      return -2;
  }

  return 1;
}

// The terminal has some requested information to send back to the host.
void on_output(const char *cs, size_t len, void *user)
{
  if(Tela::instance == nullptr)
  {
    return;
  }

  Tela::instance->logger.info("on_output");

  Tela::instance->logger.infof("on_output: %zu bytes: ", len);
  for (size_t i = 0; i < len; i++) {
    unsigned char byte = cs[i];
    if (byte == 0x1B) {
      Tela::instance->logger.info(" ESC");
    } else if (byte >= 32 && byte < 127) {
      Tela::instance->logger.infof(" '%c'", byte);
    } else {
      Tela::instance->logger.infof(" %02X", byte);
    }
  }

  std::string s(cs, len);
  Tela::instance->connection->write(s);

  return;
}

int move(VTermRect dest, VTermRect src, void *user)
{
  if(Tela::instance == nullptr)
  {
    return 0;
  }

  return Tela::instance->canvas.move(src.start_col + 1, src.start_row + 1, src.end_col + 1, src.end_row + 1, dest.start_col + 1, dest.start_row + 1, dest.end_col + 1, dest.end_row + 1);
}

int pushline(int cols, const VTermScreenCell *cells, void *user)
{
  if(Tela::instance == nullptr)
  {
    return 0;
  }

  Tela::instance->logger.debugf("sb_pushline: saving line with %d cols", cols);

  // Create a new scrollback line
  ScrollbackLine line;
  line.cols = cols;
  line.cells = new VTermScreenCell[cols];

  // Copy the cells
  memcpy(line.cells, cells, cols * sizeof(VTermScreenCell));

  // Add to scrollback buffer
  Tela::instance->scrollback_buffer.push_back(line);

  // Limit scrollback size (ESP32 has limited RAM)
  if(Tela::instance->scrollback_buffer.size() > Tela::MAX_SCROLLBACK_LINES)
  {
    // Remove oldest line
    delete[] Tela::instance->scrollback_buffer.front().cells;
    Tela::instance->scrollback_buffer.erase(
        Tela::instance->scrollback_buffer.begin()
    );
  }

  return 1;
}

int popline(int cols, VTermScreenCell *cells, void *user)
{
  if(Tela::instance == nullptr)
  {
    return 0;
  }

  // Check if we have any lines in the scrollback buffer
  if(Tela::instance->scrollback_buffer.empty())
  {
    Tela::instance->logger.debugf("sb_popline: no lines in scrollback");
    return 0;  // No lines available
  }

  // Get the most recent line from scrollback
  ScrollbackLine& line = Tela::instance->scrollback_buffer.back();

  Tela::instance->logger.debugf("sb_popline: retrieving line with %d cols (requested %d)",
      line.cols, cols);

  // Copy the cells to the provided buffer
  int copy_cols = (line.cols < cols) ? line.cols : cols;
  memcpy(cells, line.cells, copy_cols * sizeof(VTermScreenCell));

  // If the scrollback line was shorter than requested, fill the rest with blanks
  if(line.cols < cols)
  {
    VTermScreenCell blank = {0};  // Initialize empty cell
    for(int i = line.cols; i < cols; i++)
    {
      cells[i] = blank;
    }
  }

  // Remove the line from scrollback buffer and free memory
  delete[] line.cells;
  Tela::instance->scrollback_buffer.pop_back();

  return 1;  // Successfully popped a line
}

// End vterm event callbacks