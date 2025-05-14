//
// Created by Dr. Brandon Wiley on 5/13/25.
//

#include "novela.h"

void Novela::process(uint8_t c)
{
  switch(mode)
  {
    case display:
      process_display(c);

    case esc:
      if(c == '[')
      {
        mode = command;
      }
      else
      {
        mode = display;
        process_display(c);
      }

    case command:
      process_command(c);

    default:
      mode = display;
  }
}

void Novela::process_display(uint8_t c)
{
  if(c < 32) // 0-32
  {
    process_control(c);
  }
  else if(c == 32) // space
  {
    process_space();
  }
  else if(c < 127) // 33-126
  {
    process_printable(c);
  }
  else if(c == 127) // delete
  {
    // Delete is ignored by modern terminal emulators
  }
  else
  {
    process_high(c); // 127-255
  }
}

void Novela::process_control(uint8_t c)
{
  switch(c)
  {
    case 0: // NULL
    case 1: // Start of heading
    case 2: // Start of text
    case 3: // End of text
    case 4: // End of transmission
    case 5: // Enquiry
    case 6: // Acknowledge
    case 7: // bell - FIXME
      return;
    case 8: // backspace
      process_backspace();
      break;
    case 9: // horizontal tab
      canvas.setX(canvas.getX() + 8);
      break;
    case 10: // \n, newline
      canvas.setY(canvas.getY() + 1);
      checkScroll();
      break;
    case 11: // vertical tab
      // Move the cursor down one line.
      // Maintain the same column position.
      canvas.setY(canvas.getY() + 1);
      checkScroll();
      break;
    case 12: // form feed
      canvas.setX(0);
      canvas.setY(0);
      checkScroll();
      break;
    case 13: // \r, carriage return
      canvas.setX(0);
      break;
    case 14: // shift out
    case 15: // shift in
      return; // FIXME
    case 16: // data link escape
    case 17: // XON
    case 18: // Device Control Two
    case 19: // XOFF
    case 20: // Device Control 4
    case 21: // NAK
    case 22: // Synchronous Idle
    case 23: // End of transmission block
    case 24: // Cancel
    case 25: // End of medium
    case 26: // Substitute
      return;
    case 27: // Escape
      process_escape();
      break;
    case 28: // File separator
    case 29: // Group separator
    case 30: // Record separator
    case 31: // Unit separator
      return;

    default:
      return;
  }
}

void Novela::process_space()
{
  canvas.drawPixel(canvas.getX(), canvas.getY(), ' ');
  canvas.incrementX();

  checkWrap();
}

void Novela::process_printable(uint8_t c)
{
  canvas.drawPixel(canvas.getX(), canvas.getY(), c);
}

void Novela::process_backspace()
{
  // Move the cursor one position to the left.
  // If the cursor is at the left edge, do nothing.
  // Do not erase any character, only reposition the cursor.
  if(canvas.getX() > 0)
  {
    canvas.setX(canvas.getX() - 1);
  }
}

void Novela::process_high(uint8_t c)
{
  // FIXME
}

void Novela::process_escape()
{
  mode = esc;
}

void Novela::process_command(uint8_t c)
{
  switch(c)
  {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    {
      const int parameter = c - '0';
      if(parameters.empty())
      {
        parameters.push_back(parameter);
      }
      else
      {
        const std::optional<unsigned int> oldParameter = parameters.back();
        parameters.pop_back();

        if(oldParameter)
        {
          const unsigned int newParameter = (*oldParameter) * 10 + parameter;
          parameters.emplace_back(newParameter);
        }
        else
        {
          parameters.emplace_back(parameter);
        }
      }
      break;
    }
    case ';':
    {
      parameters.emplace_back(std::nullopt);
      break;
    }
    case 'H': // home
    {
      unsigned int x = 0;
      unsigned int y = 0;

      if(!parameters.empty())
      {
        if(parameters.size() == 2)
        {
          if(const std::optional<unsigned int> yp = parameters[0])
          {
            y = *yp;
          }

          if(const std::optional<unsigned int> xp = parameters[1])
          {
            x = *xp;
          }
        }
      }

      canvas.setX(x);
      canvas.setY(y);
      break;
    }
  }
}

bool Novela::checkWrap()
{
  return canvas.getX() >= canvas.getWidth();
}

bool Novela::checkScroll()
{
  return canvas.getY() >= (canvas.getHeight() - 1);
}

void Novela::scroll()
{
  canvas.verticalScroll(1);
  canvas.drawFastHLine(0, canvas.getHeight() - 1, canvas.getWidth(), ' '); // Clear bottom line
  canvas.setY(canvas.getHeight() - 1);
}