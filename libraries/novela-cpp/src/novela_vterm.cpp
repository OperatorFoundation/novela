//
// Created by Dr. Brandon Wiley on 5/26/25.
//

#include "novela_vterm.h"

#define CHAR_WIDTH 8
#define CHAR_HEIGHT 8

NovelaVterm* NovelaVterm::instance = nullptr;

NovelaVterm::NovelaVterm(Canvas& canvas, Connection& connection, Clock& clock) : canvas(canvas), connection(connection), clock(clock)
{
  instance = this;

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
    .damage = damage_callback,
    .moverect = NULL,
    .movecursor = movecursor_callback,
    .settermprop = screen_settermprop,
    .bell = bell_callback,
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
  const char c = static_cast<char>(b);
  vterm_input_write(instance->vt, &c, 1);
}


void NovelaVterm::setTitle(std::string newTitle)
{
  if(!title.empty())
  {
    for(int i = 0; i < title.size(); i++)
    {
      canvas.drawPixel(0, i + 1, ' ');
    }
  }

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

int damage_callback(VTermRect rect, void *user)
{
  if(NovelaVterm::instance == nullptr)
  {
    return -1;
  }

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

int movecursor_callback(VTermPos pos, VTermPos oldpos, int visible, void *user)
{
  if(NovelaVterm::instance == nullptr)
  {
    return -1;
  }

  NovelaVterm::instance->canvas.setX(pos.col + 1);
  NovelaVterm::instance->canvas.setY(pos.row + 1);

  if(NovelaVterm::instance->cursor)
  {
    NovelaVterm::instance->cursor->setPosition(pos.col + 1, pos.row + 1);
  }

  return 1;
}

int bell_callback(void *user)
{
  if(NovelaVterm::instance == nullptr)
  {
    return -1;
  }

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

int screen_settermprop(VTermProp prop, VTermValue *val, void *user)
{
  if(NovelaVterm::instance == nullptr)
  {
    return -1;
  }

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
      break; // FIXME

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

void on_output(const char *cs, size_t len, void *user)
{
  if(NovelaVterm::instance == nullptr)
  {
    return;
  }

  std::string s(cs, len);
  NovelaVterm::instance->connection.write(s);
}
