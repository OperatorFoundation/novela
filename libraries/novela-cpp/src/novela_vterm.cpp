//
// Created by Dr. Brandon Wiley on 5/26/25.
//

#include "novela_vterm.h"

#define CHAR_WIDTH 8
#define CHAR_HEIGHT 8

NovelaVterm* NovelaVterm::instance = nullptr;

NovelaVterm::NovelaVterm(Canvas& canvas, Connection& connection, Clock& clock, Logger& logger) : canvas(canvas), connection(connection), clock(clock), logger(logger)
{
  instance = this;
}

// The begin function allows for nothing much to happen until Arduino setup() is concluded.
// Therefore, this should be called at the end of setup().
void NovelaVterm::begin()
{
  int hpx = canvas.getHeight() - 1;
  int wpx = canvas.getWidth() - 1;
  int hc = hpx / CHAR_HEIGHT;
  int wc = wpx / CHAR_WIDTH;
  int rows = hc - 2;
  int cols = wc - 2;

  vt = vterm_new(rows, cols);
  if (!vt)
  {
    // FIXME - report error
    return;
  }

  // Set output callback
  vterm_output_set_callback(vt, on_output, NULL);

  // Get screen interface
  screen = vterm_obtain_screen(vt);

  cursor.emplace(Cursor(canvas, clock, screen));
  bell.emplace(Bell(canvas, clock));

  // Enable features
  // FIXME - implement altscreen support
  // vterm_screen_enable_altscreen(screen, 1);

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

void NovelaVterm::process(uint8_t b)
{
  logger.debug("NovelaVterm::process");

  const char c = static_cast<char>(b);
  vterm_input_write(instance->vt, &c, 1);
}

void NovelaVterm::setTitle(std::string newTitle)
{
  logger.debug("NovelaVterm::setTitle");
  logger.debug(newTitle.c_str());

  if(!title.empty())
  {
    logger.debug("empty");
    for(int i = 0; i < title.size(); i++)
    {
      canvas.drawPixel(0, i + 1, ' ');
    }
  }

  logger.debug(".");

  title = newTitle;

  for(int i = 0; i < title.size(); i++)
  {
    canvas.drawPixel(0, i + 1, title.at(i));
  }
}

void NovelaVterm::update()
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
  if(NovelaVterm::instance == nullptr)
  {
    return -1;
  }

  NovelaVterm::instance->logger.debug("redraw");

  // A rectangle from (start_row,start_col) to (end_row,end_col) changed
  for (int row = rect.start_row; row < rect.end_row; row++)
  {
    for (int col = rect.start_col; col < rect.end_col; col++)
    {
      VTermPos pos = {.row = row, .col = col};
      VTermScreenCell cell;

      // Get what should be at this position
      if (vterm_screen_get_cell(NovelaVterm::instance->screen, pos, &cell) == 0)
      {
        continue;
      }

      // Move cursor to position
      //Serial.printf("\033[%d;%dH", row + 1, col + 1);
      NovelaVterm::instance->canvas.setX(col + 1);
      NovelaVterm::instance->canvas.setY(row + 1);

      if(NovelaVterm::instance->cursor)
      {
        NovelaVterm::instance->cursor->setPosition(col + 1, row + 1);
      }

      // Set colors/attributes
      //if (cell.attrs.bold) Serial.print("\033[1m");
      //if (cell.attrs.underline) Serial.print("\033[4m");
      //if (cell.attrs.reverse) Serial.print("\033[7m");

      // Print the character
      if(cell.chars[0] == 0)
      {
        //Serial.print(' ');
        NovelaVterm::instance->canvas.drawPixel(col + 1, row + 1, ' ');
      }
      else
      {
        //Serial.print('?'); // FIXME
        NovelaVterm::instance->canvas.drawPixel(col + 1, row + 1, cell.chars[0]);
      }
    }
  }

  return 1;
}

// Move the cursor
int move_cursor(VTermPos pos, VTermPos oldpos, int visible, void *user)
{
  if(NovelaVterm::instance == nullptr)
  {
    return -1;
  }

  NovelaVterm::instance->logger.debug("move_cursor");

  NovelaVterm::instance->canvas.setX(pos.col + 1);
  NovelaVterm::instance->canvas.setY(pos.row + 1);

  if(NovelaVterm::instance->cursor)
  {
    NovelaVterm::instance->cursor->setPosition(pos.col + 1, pos.row + 1);
  }

  return 1;
}

// Ring the bell
int bell_rung(void *user)
{
  if(NovelaVterm::instance == nullptr)
  {
    return -1;
  }

  NovelaVterm::instance->logger.debug("bell_rung");

  if(NovelaVterm::instance->bell)
  {
    NovelaVterm::instance->bell->ring();
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
  if(NovelaVterm::instance == nullptr)
  {
    return -1;
  }

  NovelaVterm::instance->logger.debug("set_prop");

  switch(prop)
  {
    case VTERM_PROP_CURSORVISIBLE:
    {
      if(NovelaVterm::instance->cursor)
      {
        NovelaVterm::instance->cursor->setVisible(val->boolean);
      }
      break;
    }

    case VTERM_PROP_CURSORBLINK:
      if(NovelaVterm::instance->cursor)
      {
        NovelaVterm::instance->cursor->setBlinking(val->boolean);
      }

    case VTERM_PROP_TITLE:
    {
      std::string s(val->string.str, val->string.len);
      NovelaVterm::instance->setTitle(s);
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
  if(NovelaVterm::instance == nullptr)
  {
    return;
  }

  NovelaVterm::instance->logger.debug("on_output");

  std::string s(cs, len);
  NovelaVterm::instance->connection.write(s);
}

// End vterm event callbacks