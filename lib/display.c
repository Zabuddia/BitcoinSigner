#include <stdio.h>
#include <stdlib.h>

#include "colors.h"
#include "device.h"
#include "display.h"
#include "lcd.h"
#include "log.h"

#define ARRAY_LEN 255

/****************************** Bitmap standard information*************************************/
/*Bitmap file header   14bit*/
typedef struct {
    uint16_t type;      // File identifier
    uint32_t size;      // The size of the file
    uint16_t reserved1; // Reserved value, must be set to 0
    uint16_t reserved2; // Reserved value, must be set to 0
    uint32_t offset;    // The offset from the beginning of the file header to the beginning of
                        // the image data bit
} __attribute__((packed)) bmp_header; // 14bit

/*Bitmap information header  40bit*/
typedef struct {
    uint32_t size;               // The size of the header
    uint32_t width;              // The width of the image
    int32_t height;              // The height of the image
    uint16_t planes;             // The number of planes in the image
    uint16_t bit_count;          // The number of bits per pixel
    uint32_t compression;        // Compression type
    uint32_t image_size;         // The size of the image, in bytes
    uint32_t x_pixels_per_meter; // Horizontal resolution
    uint32_t y_pixels_per_meter; // Vertical resolution
    uint32_t colors_used;        // The number of colors used
    uint32_t colors_important;   // The number of important colors
} __attribute__((packed)) bmp_info;

/*color table: palette */
typedef struct {
    unsigned char rgbBlue;     // Blue intensity
    unsigned char rgbGreen;    // Green strength
    unsigned char rgbRed;      // Red intensity
    unsigned char rgbReversed; // Reserved value
} __attribute__((packed)) rgb_data;
/**************************************** end ***********************************************/

LCD_DIS sLCD_DIS;
static bool initialized = false;

void display_init() {
    initialized = true;
    lcd_init();
}

void display_exit() {
    if (initialized) {
        DEV_ModuleExit();
        initialized = false;
    }
}

void display_clear(uint16_t color) { LCD_SetArealColor(0, 0, LCD_WIDTH, LCD_HEIGHT, color); }

void DEV_SPI_Write_nByte(char *pData, uint32_t Len) {
    char rData[Len];
    bcm2835_spi_transfernb(pData, rData, Len);
}

void swap(uint16_t Point1, uint16_t Point2) {
    uint16_t Temp;
    Temp = Point1;
    Point1 = Point2;
    Point2 = Temp;
}

void display_draw_point(uint16_t x_point, uint16_t y_point, uint16_t color, uint8_t dot_weight) {
    if (x_point > sLCD_DIS.LCD_Dis_Column || y_point > sLCD_DIS.LCD_Dis_Page) {
        return;
    }

    uint16_t XDir_Num, YDir_Num;
    for (XDir_Num = 0; XDir_Num < 2 * dot_weight - 1; XDir_Num++) {
        for (YDir_Num = 0; YDir_Num < 2 * dot_weight - 1; YDir_Num++) {
            LCD_SetPointlColor(x_point + XDir_Num - dot_weight, y_point + YDir_Num - dot_weight,
                               color);
        }
    }
}

void display_draw_line(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end,
                       uint16_t color, uint8_t line_weight) {

    if (x_start > sLCD_DIS.LCD_Dis_Column || y_start > sLCD_DIS.LCD_Dis_Page ||
        x_end > sLCD_DIS.LCD_Dis_Column || y_end > sLCD_DIS.LCD_Dis_Page) {
        return;
    }

    if (x_start > x_end) {
        swap(x_start, x_end);
    }
    if (y_start > y_end) {
        swap(y_start, y_end);
    }

    uint16_t x_point = x_start;
    uint16_t y_point = y_start;
    int32_t dx = (int32_t)x_end - (int32_t)x_start >= 0 ? x_end - x_start : x_start - x_end;
    int32_t dy = (int32_t)y_end - (int32_t)y_start <= 0 ? y_end - y_start : y_start - y_end;

    // Increment direction, 1 is positive, -1 is counter;
    int32_t XAddway = x_start < x_end ? 1 : -1;
    int32_t YAddway = y_start < y_end ? 1 : -1;

    // Cumulative error
    int32_t Esp = dx + dy;

    for (;;) {
        display_draw_point(x_point, y_point, color, line_weight);

        if (2 * Esp >= dy) {
            if (x_point == x_end) {
                break;
            }
            Esp += dy;
            x_point += XAddway;
        }
        if (2 * Esp <= dx) {
            if (y_point == y_end) {
                break;
            }
            Esp += dx;
            y_point += YAddway;
        }
    }
}

void display_draw_rectangle(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end,
                            uint16_t color, bool filled, uint8_t line_weight) {
    if (x_start > sLCD_DIS.LCD_Dis_Column || y_start > sLCD_DIS.LCD_Dis_Page ||
        x_end > sLCD_DIS.LCD_Dis_Column || y_end > sLCD_DIS.LCD_Dis_Page) {
        return;
    }
    // printf("sLCD_DIS.LCD_Dis_Column = %d\r\n",sLCD_DIS.LCD_Dis_Column);
    // printf("sLCD_DIS.LCD_Dis_Page = %d\r\n",sLCD_DIS.LCD_Dis_Page);
    if (x_start > x_end) {
        swap(x_start, x_end);
    }
    if (y_start > y_end) {
        swap(y_start, y_end);
    }

    uint16_t y_point;
    if (filled) {
        for (y_point = y_start; y_point < y_end; y_point++) {
            display_draw_line(x_start, y_point, x_end, y_point, color, 1);
        }
    } else {
        display_draw_line(x_start, y_start, x_end, y_start, color, line_weight);
        display_draw_line(x_start, y_start, x_start, y_end, color, line_weight);
        display_draw_line(x_end, y_end, x_end, y_start, color, line_weight);
        display_draw_line(x_end, y_end, x_start, y_end, color, line_weight);
    }
}

void display_draw_circle(uint16_t x_center, uint16_t y_center, uint16_t radius, uint16_t color,
                         bool filled, uint8_t line_weight) {
    if (x_center > sLCD_DIS.LCD_Dis_Column || y_center >= sLCD_DIS.LCD_Dis_Page) {
        return;
    }

    // Draw a circle from(0, R) as a starting point
    int16_t XCurrent, YCurrent;
    XCurrent = 0;
    YCurrent = radius;

    // Cumulative error,judge the next point of the logo
    int16_t Esp = 3 - (radius << 1);

    int16_t sCountY;
    if (filled) {
        while (XCurrent <= YCurrent) { // Realistic circles
            for (sCountY = XCurrent; sCountY <= YCurrent; sCountY++) {
                display_draw_point(x_center + XCurrent, y_center + sCountY, color, 1); // 1
                display_draw_point(x_center - XCurrent, y_center + sCountY, color, 1); // 2
                display_draw_point(x_center - sCountY, y_center + XCurrent, color, 1); // 3
                display_draw_point(x_center - sCountY, y_center - XCurrent, color, 1); // 4
                display_draw_point(x_center - XCurrent, y_center - sCountY, color, 1); // 5
                display_draw_point(x_center + XCurrent, y_center - sCountY, color, 1); // 6
                display_draw_point(x_center + sCountY, y_center - XCurrent, color, 1); // 7
                display_draw_point(x_center + sCountY, y_center + XCurrent, color, 1);
            }
            if (Esp < 0) {
                Esp += 4 * XCurrent + 6;
            } else {
                Esp += 10 + 4 * (XCurrent - YCurrent);
                YCurrent--;
            }
            XCurrent++;
        }
    } else { // Draw a hollow circle
        while (XCurrent <= YCurrent) {
            display_draw_point(x_center + XCurrent, y_center + YCurrent, color, line_weight); // 1
            display_draw_point(x_center - XCurrent, y_center + YCurrent, color, line_weight); // 2
            display_draw_point(x_center - YCurrent, y_center + XCurrent, color, line_weight); // 3
            display_draw_point(x_center - YCurrent, y_center - XCurrent, color, line_weight); // 4
            display_draw_point(x_center - XCurrent, y_center - YCurrent, color, line_weight); // 5
            display_draw_point(x_center + XCurrent, y_center - YCurrent, color, line_weight); // 6
            display_draw_point(x_center + YCurrent, y_center - XCurrent, color, line_weight); // 7
            display_draw_point(x_center + YCurrent, y_center + XCurrent, color, line_weight); // 0

            if (Esp < 0) {
                Esp += 4 * XCurrent + 6;
            } else {
                Esp += 10 + 4 * (XCurrent - YCurrent);
                YCurrent--;
            }
            XCurrent++;
        }
    }
}

void display_draw_char(uint16_t x_point, uint16_t y_point, const char character, sFONT *font,
                       uint16_t background_color, uint16_t foreground_color) {
    uint16_t Page, Column;

    if (x_point >= sLCD_DIS.LCD_Dis_Column || y_point >= sLCD_DIS.LCD_Dis_Page) {
        return;
    }

    uint32_t Char_Offset =
        (character - ' ') * font->Height * (font->Width / 8 + (font->Width % 8 ? 1 : 0));
    const unsigned char *ptr = &font->table[Char_Offset];

    for (Page = 0; Page < font->Height; Page++) {
        for (Column = 0; Column < font->Width; Column++) {

            if (*ptr & (0x80 >> (Column % 8))) {
                display_draw_point(x_point + Column, y_point + Page, foreground_color, 1);
            } else {
                display_draw_point(x_point + Column, y_point + Page, background_color, 1);
            }

            // One pixel is 8 bits
            if (Column % 8 == 7) {
                ptr++;
            }
        } /* Write a line */
        if (font->Width % 8 != 0) {
            ptr++;
        }
    } /* Write all */
}

void display_draw_string(uint16_t x_start, uint16_t y_start, const char *str, sFONT *font,
                         uint16_t background_color, uint16_t foreground_color) {

    uint16_t x_point = x_start;
    uint16_t y_point = y_start;

    if (x_start >= sLCD_DIS.LCD_Dis_Column || y_start >= sLCD_DIS.LCD_Dis_Page) {
        return;
    }

    while (*str != '\0') {
        // Skip over non-displayable characters
        if (*str == '\a' || *str == '\b' || *str == '\r' || *str == '\f' || *str == '\t' ||
            *str == '\n' || *str == '\v') {
            str++;
            continue;
        }
        // if X direction filled , reposition to(x_start,y_point),y_point is Y direction plus the
        // height of the character
        if ((x_point + font->Width) > sLCD_DIS.LCD_Dis_Column) {
            x_point = x_start;
            y_point += font->Height;
        }

        // // If the Y direction is full, reposition to(x_start, y_start)
        // if ((y_point + font->Height) > sLCD_DIS.LCD_Dis_Page) {
        //     x_point = x_start;
        //     y_point = y_start;
        // }
        display_draw_char(x_point, y_point, *str, font, background_color, foreground_color);

        // The next character of the address
        str++;

        // The next word of the abscissa increases the font of the broadband
        x_point += font->Width + 1;
    }
}

void display_draw_number(uint16_t x_point, uint16_t y_point, int32_t number, sFONT *font,
                         uint16_t background_color, uint16_t foreground_color) {

    int16_t Num_Bit = 0, Str_Bit = 0;
    uint8_t Str_Array[ARRAY_LEN] = {0}, Num_Array[ARRAY_LEN] = {0};
    uint8_t *pStr = Str_Array;

    if (x_point >= sLCD_DIS.LCD_Dis_Column || y_point >= sLCD_DIS.LCD_Dis_Page) {
        return;
    }

    // Converts a number to a string
    while (number) {
        Num_Array[Num_Bit] = number % 10 + '0';
        Num_Bit++;
        number /= 10;
    }

    // The string is inverted
    while (Num_Bit > 0) {
        Str_Array[Str_Bit] = Num_Array[Num_Bit - 1];
        Str_Bit++;
        Num_Bit--;
    }

    // show
    display_draw_string(x_point, y_point, (const char *)pStr, font, background_color,
                        foreground_color);
}

void display_demo(void) {
    display_clear(WHITE);

    display_draw_line(0, 10, LCD_WIDTH, 10, BYU_NAVY, 1);
    display_draw_line(0, 15, LCD_WIDTH, 15, BYU_ROYAL, 2);
    display_draw_line(0, 22, LCD_WIDTH, 22, BYU_NAVY, 3);
    display_draw_line(0, 30, LCD_WIDTH, 30, BYU_ROYAL, 4);

    display_draw_rectangle(25, 40, LCD_WIDTH - 35, 60, BYU_ORANGE, false, 2);
    display_draw_rectangle(30, 50, LCD_WIDTH - 30, 70, BYU_ORANGE, true, 1);

    display_draw_circle(15, 85, 10, BYU_NAVY, false, 1);
    display_draw_circle(35, 85, 10, BYU_ROYAL, true, 1);
    display_draw_circle(55, 85, 10, BYU_BURGUNDY, false, 1);
    display_draw_circle(75, 85, 10, BYU_EGGPLANT, true, 1);
    display_draw_circle(95, 85, 10, BYU_FOREST, false, 1);
    display_draw_circle(115, 85, 10, BYU_BLUE_GRAY, true, 1);

    display_draw_string(25, 100, "Welcome to", &Font12, WHITE, BYU_PLUM);

    int start = 22;
    int spacing = 10;
    display_draw_char(start, 115, 'E', &Font16, WHITE, BYU_BLUE);
    display_draw_char(start + spacing * 1, 115, 'C', &Font16, WHITE, BYU_PLUM);
    display_draw_char(start + spacing * 2, 115, 'E', &Font16, WHITE, BYU_ORANGE);
    display_draw_char(start + spacing * 3, 115, 'n', &Font16, WHITE, BYU_GREEN);
    display_draw_number(start + spacing * 5, 115, 224, &Font16, WHITE, BYU_ROYAL);

    delay_ms(2000);

    display_draw_image("../pic/byu.bmp");

    delay_ms(2000);

    display_draw_image("../pic/time.bmp");
}

uint8_t display_draw_image(char *file_name) {
    FILE *fp;          // Define a file pointer
    bmp_header header; // Define a bmp file header structure
    bmp_info info;     // Define a bmp bitmap header structure

    // Binary file open
    if ((fp = fopen(file_name, "rb")) == NULL) {
        printf("Can't open the file!\n");
        return 1;
    }

    fread(&header, sizeof(bmp_header), 1, fp);
    fread(&info, sizeof(bmp_info), 1, fp);

    // file header information
    log_trace("********************************************************");
    log_trace("file header information:");
    log_trace(" FileSize         : %ld", header.size);
    log_trace(" Reserv1          : %d", header.reserved1);
    log_trace(" Reserv2          : %d", header.reserved2);
    log_trace(" FileOffset       : %ld", header.offset);

    // bitmap header information
    log_trace("********************************************************");
    log_trace("bitmap header information:");
    log_trace(" DIBHeaderSize    : %lu", info.size);
    log_trace(" ImageWidth       : %lu", info.width);
    log_trace(" ImageHeight      : %lu", info.height);
    log_trace(" Planes           : %u", info.planes);
    log_trace(" BPP              : %u", info.bit_count);
    log_trace(" Compression      : %lu", info.compression);
    log_trace(" ImageSize        : %lu", info.image_size);
    log_trace(" XPPM             : %lu", info.x_pixels_per_meter);
    log_trace(" YPPM             : %lu", info.y_pixels_per_meter);
    log_trace(" CCT              : %lu", info.colors_used);
    log_trace(" ICC              : %lu", info.colors_important);

    int32_t row;
    uint32_t col;
    short data;
    rgb_data rgb;
    size_t len = info.bit_count / 8; // RGB888,one 3 byte = 1 bitbmp

    // Deal with negative heights which are allowed
    // https://stackoverflow.com/questions/22850436/can-the-width-field-in-a-bmp-with-bitmapinfoheader-be-negative
    if (info.height < 0) {
        info.height *= -1;
    }

    // Just to image part of file
    fseek(fp, header.offset, SEEK_SET);

    LCD_SetWindows(0, 0, sLCD_DIS.LCD_Dis_Column, sLCD_DIS.LCD_Dis_Page);
    for (row = 0; row < info.height; row++) {
        for (col = 0; col < info.width; col++) {
            if (fread((char *)&rgb, 1, len, fp) != len) {
                perror("get bmpdata:\r\n");
                return -1;
                break;
            }
            data = RGB((rgb.rgbRed), (rgb.rgbGreen), (rgb.rgbBlue));
            LCD_SetColor(data, 1, 1);
        }
    }
    fclose(fp);
    return 0;
}
