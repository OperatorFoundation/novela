//
// Created by Dr. Brandon Wiley on 5/29/25.
//

#include "sdl_cursor.h"
#include "lgfx_sdl_canvas.h"

SDLCursor::SDLCursor(Clock& clock, Canvas& canvas, Logger& logger, LGFX* screen)
    : Cursor(clock), canvas(canvas), logger(logger), gfx_screen(screen)
{
    initializeFontMetrics();

    // Get the current theme color from the canvas
    auto* sdl_canvas = dynamic_cast<LGFXSDLCanvas*>(&canvas);
    if (sdl_canvas) {
        cursor_color = sdl_canvas->getCurrentThemeColor();
    }
}

void SDLCursor::initializeFontMetrics()
{
    if (gfx_screen) {
        char_width = gfx_screen->textWidth("M");  // This should match the canvas font
        char_height = gfx_screen->fontHeight();   // This should match the canvas font

        // Allocate buffer for saving cursor area
        size_t buffer_size = char_width * char_height;
        saved_pixels = new uint16_t[buffer_size];

        logger.debugf("SDLCursor: Font metrics - width: %d, height: %d", char_width, char_height);
    }
}

void SDLCursor::saveCursorArea()
{
    if (gfx_screen && saved_pixels) {
        gfx_screen->readRect(pixelX(), pixelY(), char_width, char_height, saved_pixels);
        has_saved_content = true;
    }
}

void SDLCursor::restoreCursorArea()
{
    if (gfx_screen && saved_pixels && has_saved_content) {
        gfx_screen->pushImage(pixelX(), pixelY(), char_width, char_height, saved_pixels);
        has_saved_content = false;
    }
}

void SDLCursor::drawCursorShape()
{
    if (!gfx_screen) return;

    int x = pixelX();
    int y = pixelY();

    switch (shape) {
        case block:
            // Filled rectangle
            gfx_screen->fillRect(x, y, char_width, char_height, cursor_color);
            break;

        case underline:
        case underline_steady:
            // Line at bottom of character cell
            gfx_screen->fillRect(x, y + char_height - 2, char_width, 2, cursor_color);
            break;

        case bar:
        case bar_steady:
        case bar_blinking:
            // Vertical line at left edge of character cell
            gfx_screen->fillRect(x, y, 2, char_height, cursor_color);
            break;

        default:
            // Default to block cursor
            gfx_screen->fillRect(x, y, char_width, char_height, cursor_color);
            break;
    }
}

void SDLCursor::show()
{
    logger.debugf("SDLCursor::show@(%d,%d) shape:%d", col, row, shape);

    if (!gfx_screen) {
        logger.debug("SDLCursor::show - no screen device");
        return;
    }

    // Save what's currently under the cursor
    saveCursorArea();

    // For block cursor, we need to handle the character underneath differently
    if (shape == block) {
        // For a block cursor, we invert the character colors
        if (screen) {
            VTermScreenCell cell;
            VTermPos pos = {.row = row, .col = col};

            int result = vterm_screen_get_cell(screen, pos, &cell);
            if (result && cell.chars[0]) {
                // Draw character in inverted colors (background color as foreground)
                uint16_t bg_color = 0x0000;
                gfx_screen->drawChar(pixelX(), pixelY(), cell.chars[0],
                                   bg_color, cursor_color, 1.0f);
                return;
            }
        }

        // Fallback: draw solid block
        drawCursorShape();
    } else {
        // For other cursor types, draw the character normally then overlay the cursor
        if (screen) {
            VTermScreenCell cell;
            VTermPos pos = {.row = row, .col = col};

            int result = vterm_screen_get_cell(screen, pos, &cell);
            if (result && cell.chars[0]) {
                // First draw the character normally
                canvas.drawCharacter(col, row, cell.chars[0]);
            }
        }

        // Then overlay the cursor shape
        drawCursorShape();
    }
}

void SDLCursor::hide()
{
    logger.debugf("SDLCursor::hide@(%d,%d)", col, row);

    // Restore the saved content, or redraw the character if we don't have saved content
    if (has_saved_content) {
        restoreCursorArea();
    } else {
        // Fallback: redraw the character that should be at this position
        if (screen) {
            VTermScreenCell cell;
            VTermPos pos = {.row = row, .col = col};

            int result = vterm_screen_get_cell(screen, pos, &cell);
            if (result && cell.chars[0]) {
                logger.debugf("SDLCursor::hide - redrawing char: %c", cell.chars[0]);
                canvas.drawCharacter(col, row, cell.chars[0]);
                return;
            }
        }

        // Ultimate fallback: clear the area
        logger.debug("SDLCursor::hide - fallback clear");
        canvas.drawCharacter(col, row, ' ');
    }
}

void SDLCursor::move()
{
    logger.debugf("SDLCursor::move->(%d,%d)", col, row);
    // Move is typically handled by hide() at old position, then show() at new position
    // The base class handles this logic in setPosition()
}

// New methods required by the refactored Cursor API
void SDLCursor::doSaveUnder()
{
    // This is called by the base class before showing the cursor
    // Your existing saveCursorArea() already does this
    saveCursorArea();
}

void SDLCursor::doRestoreUnder()
{
    // This is called by the base class after hiding the cursor
    // Your existing restoreCursorArea() already does this
    restoreCursorArea();
}