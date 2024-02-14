#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <stdbool.h>
#include <stdint.h>

#include "fonts/fonts.h"

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT DISPLAY_WIDTH

/**
 * Description:
 *  Sets up display. Should be called before any display_* functions are called.
 *
 * Arguments:
 *  None
 */
void display_init();

/**
 * Description:
 *  Cleans up the display. This should be one of the last functions called when you exit.
 *
 * Arguments:
 *  None
 */
void display_exit();

/**
 * Description:
 *  Clears the screen with one color.
 *
 * Arguments:
 *  color: The color you would like the screen to be cleared to.
 */
void display_clear(uint16_t color);

/**
 * Description:
 *  Runs a demo that shows off the capabilities of the library.
 *
 * Arguments:
 *  None
 */
void display_demo();

/**
 * Description:
 *  Draws one point. The point can be various sizes.
 *
 * Arguments:
 *  x_point: The x-coordinate of the point.
 *  y_point: The y-coordinate of the point.
 *  color: The color of the point.
 *  dot_weight: The thickness of the point. A 1 will color a 1x1 point, a 2 will color a 2x2 point,
 *              a 3 will color a 3x3 point, etc.
 */
void display_draw_point(uint16_t x_point, uint16_t y_point, uint16_t color, uint8_t dot_weight);

/**
 * Description:
 *  Draws a line. The line can be various weights.
 *
 * Arguments:
 *  x_start: The start x-coordinate of the line.
 *  y_start: The start y-coordinate of the line.
 *  x_end: The end x-coordinate of the line.
 *  y_end: The end y-coordinate of the line.
 *  color: The color of the line.
 *  line_weight: The thickness of the line.
 */
void display_draw_line(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end,
                       uint16_t color, uint8_t line_weight);

/**
 * Description:
 *  Draws a rectangle. The rectangle's line can be various weights and the recetangle can be filled
 *  in.
 *
 * Arguments:
 *  x_start: The top left x-coordinate of the rectangle.
 *  y_start: The top left y-coordinate of the rectangle.
 *  x_end: The bottom right x-coordinate of the rectangle.
 *  y_end: The bottom right y-coordinate of the rectangle.
 *  color: The color of the rectangle.
 *  filled: Whether the rectangle should be filled in or not.
 *  line_weight: The thickness of the line.
 */
void display_draw_rectangle(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end,
                            uint16_t color, bool filled, uint8_t line_weight);

/**
 * Description:
 *  Draws a circle. The circles's line can be various weights and the circle can be filled in.
 *
 * Arguments:
 *  x_center: The center x-coordinate of the circle.
 *  y_center: The center y-coordinate of the circle.
 *  radius: The radius of the circle.
 *  color: The color of the circle.
 *  filled: Whether the circle should be filled in or not.
 *  line_weight: The thickness of the line.
 */
void display_draw_circle(uint16_t x_center, uint16_t y_center, uint16_t radius, uint16_t color,
                         bool filled, uint8_t line_weight);

/**
 * Description:
 *  Draw a character. This is useful if you only want to print one character. Use
 *  display_draw_string otherwise.
 *
 * Arguments:
 *  x_start: The top left x-coordinate of the character.
 *  y_start: The top left y-coordinate of the character.
 *  character: The character to draw.
 *  font: A pointer to the font to use. This will determine the size of the text.
 *  background_color: The background color of the text.
 *  foreground_color: The color of the text.
 */
void display_draw_char(uint16_t x_start, uint16_t y_start, const char character, sFONT *font,
                       uint16_t background_color, uint16_t foreground_color);

/**
 * Description:
 *  Draw a string. This will automatically wrap long strings.
 *
 * Arguments:
 *  x_start: The top left x-coordinate of the start of the string.
 *  y_start: The top left y-coordinate of the start of the string.
 *  str: The string to draw.
 *  font: A pointer to the font to use. This will determine the size of the text.
 *  background_color: The background color of the text.
 *  foreground_color: The color of the text.
 */
void display_draw_string(uint16_t x_start, uint16_t y_start, const char *str, sFONT *font,
                         uint16_t background_color, uint16_t foreground_color);

/**
 * Description:
 *  Draw a number. This is useful if you only want to print a number. Use display_draw_string
 *  otherwise.
 *
 * Arguments:
 *  x_start: The top left x-coordinate of the number.
 *  y_start: The top left y-coordinate of the number.
 *  number: The number to draw.
 *  font: A pointer to the font to use. This will determine the size of the text.
 *  background_color: The background color of the text.
 *  foreground_color: The color of the text.
 */
void display_draw_number(uint16_t x_point, uint16_t y_point, int32_t number, sFONT *font,
                         uint16_t background_color, uint16_t foreground_color);

/**
 * Description:
 *  Draw an image. The image must be 127x127 and must be in the BMP format.
 *
 * Arguments:
 *  file_name: The name and path of the image file.
 */
uint8_t display_draw_image(char *file_name);

#endif
