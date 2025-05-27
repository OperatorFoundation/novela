#include <Arduino.h>
#include <string>

#include <vterm.h>

#define ROWS 23
#define COLS 80

VTerm *vt;
VTermScreen *screen;

int output_callback(const char *bytes, size_t len, void *user)
{
  // This is for terminal responses (like cursor position reports)
  // Send back through your communication channel
  Serial.write((const uint8_t*)bytes, len);
  return len;
}

int damage_callback(VTermRect rect, void *user)
{
  // A rectangle from (start_row,start_col) to (end_row,end_col) changed

  for (int row = rect.start_row; row < rect.end_row; row++)
  {
    for (int col = rect.start_col; col < rect.end_col; col++)
    {
      VTermPos pos = {.row = row, .col = col};
      VTermScreenCell cell;

      // Get what should be at this position
      if (vterm_screen_get_cell(screen, pos, &cell) == 0)
      {
        continue;
      }

      // Move cursor to position
      Serial.printf("\033[%d;%dH", row + 1, col + 1);

      // Set colors/attributes
      if (cell.attrs.bold) Serial.print("\033[1m");
      if (cell.attrs.underline) Serial.print("\033[4m");
      if (cell.attrs.reverse) Serial.print("\033[7m");

      // Print the character
      if (cell.chars[0] == 0)
      {
        Serial.print(' ');
      }
      else
      {
        Serial.print('?'); // FIXME
      }

      // Reset attributes
      Serial.print("\033[m");
    }
  }
  return 1;
}

int movecursor_callback(VTermPos pos, VTermPos oldpos, int visible, void *user)
{
  // Terminal wants cursor at new position
  Serial.printf("\033[%d;%dH", pos.row + 1, pos.col + 1);
  if (visible)
  {
    Serial.print("\033[?25h");  // Show cursor
  }
  else
  {
    Serial.print("\033[?25l");  // Hide cursor
  }

  return 1;
}

int bell_callback(void *user)
{
  Serial.print('\007');  // ASCII bell
  // Or: tone(BUZZER_PIN, 1000, 100);
  return 1;
}

int screen_settermprop(VTermProp prop, VTermValue *val, void *user)
{
  switch (prop) {
    case VTERM_PROP_CURSORVISIBLE:
      Serial.print(val->boolean ? "\033[?25h" : "\033[?25l");
      break;
    case VTERM_PROP_TITLE:
      /* Correct way to handle VTermStringFragment */
      Serial.print("\033]2;");
      if (val->string.str && val->string.len > 0) {
        Serial.write((const uint8_t *)val->string.str,
            val->string.len);
      }
      Serial.print("\007");
      break;
    case VTERM_PROP_ICONNAME:
      /* Also a VTermStringFragment */
      Serial.print("\033]1;");
      if (val->string.str && val->string.len > 0) {
        Serial.write((const uint8_t *)val->string.str,
            val->string.len);
      }
      Serial.print("\007");
      break;
    case VTERM_PROP_ALTSCREEN:
      Serial.print(val->boolean ? "\033[?47h" : "\033[?47l");
      break;
    case VTERM_PROP_MOUSE:
      /* Mouse mode changes */
      if (val->number == VTERM_PROP_MOUSE_NONE) {
        Serial.print("\033[?1000l");
      } else if (val->number == VTERM_PROP_MOUSE_CLICK) {
        Serial.print("\033[?1000h");
      }
      break;
  }
  return (1);
}

void on_output(const char *s, size_t len, void *user)
{
  Serial.write((const uint8_t *)s, len);
}

// Callback structure
static VTermScreenCallbacks screen_callbacks = {
  .damage = damage_callback,
  .moverect = NULL,
  .movecursor = movecursor_callback,
  .settermprop = screen_settermprop,
  .bell = bell_callback,
  .resize = NULL,
  .sb_pushline = NULL,
  .sb_popline = NULL
};

void setup()
{
  Serial.begin(9600);
  Serial.println("Hello, Operator.");

  vt = vterm_new(ROWS, COLS);
  if (!vt) {
    Serial.println("Failed to create vterm");
    return;
  }

  // Set output callback
  vterm_output_set_callback(vt, on_output, NULL);

  // Get screen interface
  screen = vterm_obtain_screen(vt);

  // Enable features
  vterm_screen_enable_altscreen(screen, 1);

  // Set callbacks
  vterm_screen_set_callbacks(screen, &screen_callbacks, NULL);

  // Initialize screen
  vterm_screen_reset(screen, 1);

  // Clear physical display
  Serial.print("\033[2J\033[H");
  Serial.println("Terminal ready!");
}

void loop()
{
  // Handle input from serial
  while (Serial.available())
  {
    char c = Serial.read();

    // Special keys - send escape sequences to libvterm
    if (c == '\r')
    {
      // Enter key
      vterm_input_write(vt, "\r", 1);
    }
    else
    {
      // Normal character
      vterm_input_write(vt, &c, 1);
    }
  }

  // You can also write output to the terminal
  static unsigned long last = 0;
  if (millis() - last > 5000)
  {
    last = millis();

    // Write some text
    const char *msg = "Hello from RP2350! \033[31mRed text\033[m\r\n";
    vterm_input_write(vt, msg, strlen(msg));
  }
}
