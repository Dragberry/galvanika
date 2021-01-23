#ifndef INC_WD_G0906_H_
#define INC_WD_G0906_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

/* GPIOx config */
#define LCD_GPIO_RCC  RCC_APB2Periph_GPIOA
#define LCD_GPIO    GPIOB
#define LCD_RESET_PIN GPIO_PIN_9
#define LCD_SCK_PIN   GPIO_PIN_6
#define LCD_SDA_PIN   GPIO_PIN_7
#define LCD_CS_PIN    GPIO_PIN_8

/**************************************
 * Display macro
 **************************************/

/* LCD memory definitions */
#define LCD_XMAX    96
#define LCD_YMAX    9

/* LCD point array definitions */
#define LCD_POINT_XMAX  96
#define LCD_POINT_YMAX  68

/**************************************
 * Type definitions
 **************************************/

/* Command or data */
typedef enum {
  COMMAND = 0, DATA = 1
} lcd_cd_t;

/* Point str type */
typedef struct {
  uint16_t x_point;
  uint16_t y_point;
} point_t;

/* Graph error type */
typedef enum {
  NO_GRAPH_ERROR = 0, GRAPH_ERROR = 1
} gr_error_t;

/* Point mode type */
typedef enum {
  PIXEL_CLEAR = 0, PIXEL_SET = 1, PIXEL_XOR = 2 // Note: PIXEL_XOR mode affects only to vram_put_point().
// Graph functions only transmit parameter to vram_put_point() and result of graph functions
// depends on it's implementation. However Line() and Bar() functions work well.
} point_mode_t;

/* Font descriptor type */
typedef struct {
  uint16_t char_width;
  uint16_t char_offset;
} font_descriptor_t;

/* Font info type */
typedef struct {
  uint16_t font_height;
  unsigned char start_char;
  unsigned char end_char;
  const font_descriptor_t *descr_array;
  const unsigned char *font_bitmap_array;
} font_info_t;

/**************************************
 * Prototypes
 **************************************/

/**** Low level ****/

/* Init display */
extern void lcd_init(void);
/* Performes hardware reset */
extern void lcd_hw_reset(void);
/* Write command or data byte to LCD */
extern void lcd_write(lcd_cd_t cd, uint8_t byte);
/* Fills visual LCD RAM with zeros and sets current cursor position to 0,0 */
extern void lcd_clear(void);
/* Set cursor position. From 0, 0 to (LCD_XMAX-1), (LCD_YMAX-1) */
extern void lcd_gotoxy(uint8_t x, uint8_t y);
/* Puts 8x5 char directly to current cursor position */
extern void lcd_putchar(const char c);
/* Puts 8x5 string with whitespace to current cursor position */
extern void lcd_putstr(const char *str);
/* Puts 8xsize bar of section_width */
extern void lcd_putbar(const uint8_t size, const uint8_t filled, const uint8_t section_width);
/* Update LCD VRAM from software VRAM */
extern void lcd_vram_update(void);

/**** High level ****/

/* All graph functions return error if exceed boundaries.
 * p1 and p2 may be entered in any sequence */
/* Puts point with coordinates p to VRAM */
extern gr_error_t vram_put_point(point_t p, point_mode_t mode);
/* Draws line in VRAM from p1 to p2 */
extern gr_error_t vram_put_line(point_t p1, point_t p2, point_mode_t mode);
/* Draws rectangle in VRAM from p1 to p2 */
extern gr_error_t vram_put_rect(point_t p1, point_t p2, point_mode_t mode);
/* Draws filled rectangle in VRAM from p1 to p2 */
extern gr_error_t vram_put_bar(point_t p1, point_t p2, point_mode_t mode);
/* Draws circle in VRAM from center with radius */
extern gr_error_t vram_put_circle(point_t center, uint8_t radius,
    point_mode_t mode);

/* Puts character from font type pointer (begins from top left).
 * Returns error if exceeds boundaries or no char available */
extern gr_error_t vram_put_char(point_t p, const font_info_t *font,
    const char c, point_mode_t mode);
/* Puts string from font type pointer. Returns error if exceeds boundaries or no char available */
extern gr_error_t vram_put_str(point_t p, const font_info_t *font,
    const char *str, point_mode_t mode);

#ifdef __cplusplus
}
#endif

#endif
