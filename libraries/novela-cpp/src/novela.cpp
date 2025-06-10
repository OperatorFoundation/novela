//
// Created by Dr. Brandon Wiley on 5/26/25.
//

#include "novela.h"

#include "colors.h"

#define CHAR_WIDTH 8
#define CHAR_HEIGHT 8

Novela* Novela::instance = nullptr;

Novela::Novela(Canvas& canvas, Connection& connection, Clock& clock, Logger& logger, Cursor *cursor) : canvas(canvas), connection(connection), clock(clock), logger(logger), cursor(cursor)
{
  instance = this;
}

// The begin function allows for nothing much to happen until Arduino setup() is concluded.
// Therefore, this should be called at the end of setup().
void Novela::begin()
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
  cursor->setScreen(screen);

  bell.emplace(Bell(canvas, clock));

  // Enable features
  // FIXME - implement altscreen support
  // vterm_screen_enable_altscreen(screen, 1);

  VTermState *state = vterm_obtain_state(vt);
  vterm_state_set_unrecognised_fallbacks(state, nullptr, nullptr);

  // Set callbacks
  screen_callbacks = {
    .damage = redraw,
    .moverect = NULL,
    .movecursor = move_cursor,
    .settermprop = set_prop,
    .bell = bell_rung,
    .resize = NULL,
    .sb_pushline = NULL,
    .sb_popline = NULL
  };
  vterm_screen_set_callbacks(screen, &screen_callbacks, NULL);

  // Initialize screen
  vterm_screen_reset(screen, 1);
}

void Novela::process(std::vector<char> bs)
{
  logger.debug("NovelaVterm::process");

  vterm_input_write(instance->vt, bs.data(), bs.size());

  // for(auto c : bs)
  // {
  //   processCharacter(c);
  // }
}

void Novela::processCharacter(char c)
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

      if((c == 0x07)) // BEL
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

void Novela::setTitle(std::string newTitle)
{
  if(title.empty())
  {
    logger.debug("NovelaVterm::setTitle: (empty)");
  }
  else
  {
    logger.debugf("NovelaVterm::setTitle: %s", newTitle.c_str());

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

void Novela::update()
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
  if(Novela::instance == nullptr)
  {
    return -1;
  }

  Novela::instance->logger.debugf("redraw %d:%d %d:%d", rect.start_col, rect.end_col, rect.start_row, rect.end_row);

  // A rectangle from (start_row,start_col) to (end_row,end_col) changed
  for (int row = rect.start_row; row < rect.end_row; row++)
  {
    for (int col = rect.start_col; col < rect.end_col; col++)
    {
      VTermPos pos = {.row = row, .col = col};
      VTermScreenCell cell;

      // Get what should be at this position
      if (vterm_screen_get_cell(Novela::instance->screen, pos, &cell) == 0)
      {
        continue;
      }

      // Set colors/attributes
      //if (cell.attrs.bold) Serial.print("\033[1m");
      //if (cell.attrs.underline) Serial.print("\033[4m");
      //if (cell.attrs.reverse) Serial.print("\033[7m");

      // Print the character
      if(cell.chars[0] == 0)
      {
        Novela::instance->logger.debugf("redraw:(empty) %d,%d", col, row);
        Novela::instance->canvas.drawCharacter(col + 1, row + 1, ' ');
      }
      else
      {
        Novela::instance->logger.debugf("redraw:%c %d,%d", cell.chars[0], col, row);
        Novela::instance->canvas.drawCharacter(col + 1, row + 1, cell.chars[0]);
      }
    }
  }

  return 1;
}

// Move the cursor
int move_cursor(VTermPos pos, VTermPos oldpos, int visible, void *user)
{
  if(Novela::instance == nullptr)
  {
    return -1;
  }

  Novela::instance->logger.debug("move_cursor begin");

  if(Novela::instance->cursor)
  {
    Novela::instance->cursor->setPosition(pos.col + 1, pos.row + 1);
  }

  Novela::instance->logger.debug("move_cursor end");

  return 1;
}

// Ring the bell
int bell_rung(void *user)
{
  if(Novela::instance == nullptr)
  {
    return -1;
  }

  Novela::instance->logger.debug("bell_rung");

  if(Novela::instance->bell)
  {
    Novela::instance->bell->ring();
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
  if(Novela::instance == nullptr)
  {
    return -1;
  }

  Novela::instance->logger.debug("set_prop");

  switch(prop)
  {
    case VTERM_PROP_CURSORVISIBLE:
    {
      if(Novela::instance->cursor)
      {
        Novela::instance->cursor->setVisible(val->boolean);
      }
      break;
    }

    case VTERM_PROP_CURSORBLINK:
      if(Novela::instance->cursor)
      {
        Novela::instance->cursor->setBlinking(val->boolean);
      }

    case VTERM_PROP_TITLE:
    {
      std::string s(val->string.str, val->string.len);
      Novela::instance->setTitle(s);
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
  if(Novela::instance == nullptr)
  {
    return;
  }

  Novela::instance->logger.info("on_output");

  Novela::instance->logger.infof("on_output: %zu bytes: ", len);
  for (size_t i = 0; i < len; i++) {
    unsigned char byte = cs[i];
    if (byte == 0x1B) {
      Novela::instance->logger.info(" ESC");
    } else if (byte >= 32 && byte < 127) {
      Novela::instance->logger.infof(" '%c'", byte);
    } else {
      Novela::instance->logger.infof(" %02X", byte);
    }
  }

  std::string s(cs, len);
  Novela::instance->connection.write(s);

  return;
}

// End vterm event callbacks