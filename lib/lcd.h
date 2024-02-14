#ifndef __LCD_H
#define __LCD_H

#include <stdbool.h>
#include <stdint.h>

#define LCD_WIDTH 128  // LCD width
#define LCD_HEIGHT 128 // LCD height
#define LCD_X 2
#define LCD_Y 1

#define LCD_X_MAXPIXEL 132 // LCD width maximum memory
#define LCD_Y_MAXPIXEL 162 // LCD height maximum memory

typedef enum {
    L2R_U2D = 0, // The display interface is displayed , left to right, up to down
    L2R_D2U,
    R2L_U2D,
    R2L_D2U,

    U2D_L2R,
    U2D_R2L,
    D2U_L2R,
    D2U_R2L,
} LCD_SCAN_DIR;

typedef struct {
    uint16_t LCD_Dis_Column; // COLUMN
    uint16_t LCD_Dis_Page;   // PAGE
    LCD_SCAN_DIR LCD_Scan_Dir;
    uint16_t LCD_X_Adjust; // LCD x actual display position calibration
    uint16_t LCD_Y_Adjust; // LCD y actual display position calibration
} LCD_DIS;

void LCD_SetGramScanWay(LCD_SCAN_DIR Scan_dir);

// LCD set cursor + windows + color
void LCD_SetWindows(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end);
void LCD_SetCursor(uint16_t x_point, uint16_t y_point);
void LCD_SetColor(uint16_t color, uint16_t x_point, uint16_t y_point);
void LCD_SetPointlColor(uint16_t x_point, uint16_t y_point, uint16_t color);
void LCD_SetArealColor(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end,
                       uint16_t color);

void lcd_init();

#endif
