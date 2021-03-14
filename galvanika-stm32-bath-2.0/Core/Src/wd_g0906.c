#include <displays/font5x8.hpp>
#include <wd_g0906.h>
#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"

/* Video RAM */
volatile unsigned char lcd_video_ram[LCD_YMAX][LCD_XMAX];

/***************** LOW LEVEL ************************************************/

/*
 * N1110 LCD init routine
 */
void lcd_init(void) {
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Clock to LCD pins GPIOx */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* LCD GPIOx config */
  HAL_GPIO_WritePin(LCD_GPIO,
  LCD_CS_PIN | LCD_RESET_PIN | LCD_SDA_PIN | LCD_SCK_PIN, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = LCD_CS_PIN | LCD_RESET_PIN | LCD_SDA_PIN | LCD_SCK_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  HAL_GPIO_WritePin(LCD_GPIO, LCD_RESET_PIN | LCD_CS_PIN, GPIO_PIN_SET); // RESET, CS initially high
  HAL_GPIO_WritePin(LCD_GPIO, LCD_SCK_PIN | LCD_SDA_PIN, GPIO_PIN_RESET); // SCK, SDA initially low

  lcd_hw_reset(); // HW reset

  lcd_write(COMMAND, 0xE2); // Internal reset
  lcd_write(COMMAND, 0xEB); // Thermal comp. on
  lcd_write(COMMAND, 0x2F); // Supply mode
  lcd_write(COMMAND, 0xA1); // Horisontal reverse: Reverse - 0xA9, Normal - 0xA1
  //n1110_lcd_write(COMMAND, 0xC8); // Vertical reverse (comment if no need)
  lcd_write(COMMAND, 0xA4); // Clear screen
  lcd_write(COMMAND, 0xA6); // Positive - A7, Negative - A6
  lcd_write(COMMAND, 0x90); // Contrast 0x90...0x9F

  lcd_write(COMMAND, 0xAF); // Enable LCD
}

/*
 * N1110 LCD hardware reset
 * !!!NOTE: needs normal delay implementation
 */
void lcd_hw_reset(void) {
  volatile register uint32_t i;

  /* !RESET -> Wait -> RESET */
  HAL_GPIO_WritePin(LCD_GPIO, LCD_RESET_PIN, GPIO_PIN_RESET);
  for (i = 0; i < 24000; i++) {
  };
  HAL_GPIO_WritePin(LCD_GPIO, LCD_RESET_PIN, GPIO_PIN_SET);
}

/*
 * N1110 LCD write data or command
 */
void lcd_write(lcd_cd_t cd, uint8_t byte) {
  register uint32_t i;

  /* Slave select */
  HAL_GPIO_WritePin(LCD_GPIO, LCD_CS_PIN, GPIO_PIN_RESET); // SS fall

  /* DATA/COMMAND selection */
  if (cd == DATA)
    HAL_GPIO_WritePin(LCD_GPIO, LCD_SDA_PIN, GPIO_PIN_SET); // DATA
  else
    HAL_GPIO_WritePin(LCD_GPIO, LCD_SDA_PIN, GPIO_PIN_RESET); // COMMAND

  HAL_GPIO_WritePin(LCD_GPIO, LCD_SCK_PIN, GPIO_PIN_SET); // First SCK tick
  for (i = 0; i < 8; i++) {
    HAL_GPIO_WritePin(LCD_GPIO, LCD_SCK_PIN, GPIO_PIN_RESET); // SCK fall
    /* SDA */
    if (byte & 0x80)
      HAL_GPIO_WritePin(LCD_GPIO, LCD_SDA_PIN, GPIO_PIN_SET);
    else
      HAL_GPIO_WritePin(LCD_GPIO, LCD_SDA_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_GPIO, LCD_SCK_PIN, GPIO_PIN_SET); // SCK rise

    byte <<= 1;
  }
  HAL_GPIO_WritePin(LCD_GPIO, LCD_SCK_PIN, GPIO_PIN_RESET); // SCK fall

  /* Slave release */
  HAL_GPIO_WritePin(LCD_GPIO, LCD_CS_PIN, GPIO_PIN_SET); // SS rise
}

/*
 * Clear LCD screen
 */
void lcd_clear(void) {
  register uint32_t i, j;

  lcd_write(COMMAND, 0xAE); // Disable display

  for (i = 0; i < LCD_YMAX; i++) {
    /* Set row to clear */
    lcd_gotoxy(0, i);
    /* Fill row with zeros */
    for (j = 0; j < LCD_XMAX; j++)
      lcd_write(DATA, 0x00);
  }

  lcd_gotoxy(0, 0); // GOTO 0,0
  lcd_write(COMMAND, 0xAF); // Enable display
}

/*
 * Set current position
 */
void lcd_gotoxy(uint8_t x, uint8_t y) {
  lcd_write(COMMAND, (0xB0 | (y & 0x0F)));
  lcd_write(COMMAND, (0x10 | (x >> 0x04)));
  lcd_write(COMMAND, 0x0F & x);
}

/*
 * Put character to current position
 */
void lcd_putchar(const char c) {
  register uint32_t i;

  /* Put character image */
  for (i = 0; i < 5; i++)
//    lcd_write(DATA, lcd_font[(c * 5) + i]);
  /* Put whitespace */
  lcd_write(DATA, 0x00);
}

/*
 * Put string from RAM
 */
void lcd_putstr(const char *str) {
  char c;

  /* Send string */
  while ((c = (*str++)))
    lcd_putchar(c);
}

/* Puts 8xsize bar of section_width */
void lcd_putbar(const uint8_t size, const uint8_t filled, const uint8_t section_width) {
  register uint32_t i;
  uint8_t total_width = size * section_width;
  uint8_t total_filled_width = filled * section_width;

  /* Put border */
  lcd_write(DATA, 0x7F);
  for (i = 0; i < total_width; i++) {
    if (i < total_filled_width) {
      /* Put filled line */
      lcd_write(DATA, (i + 1) % section_width ? 0x7F : 0x41);
    } else {
      /* Put empty line */
      lcd_write(DATA, 0x41);
    }
  }
  /* Put border */
  lcd_write(DATA, 0x7F);
}

/*
 * Update LCD image from VRAM
 */
void lcd_vram_update(void) {
  register uint32_t i, j;

  for (i = 0; i < LCD_YMAX; i++) {
    lcd_gotoxy(0, i);
    for (j = 0; j < LCD_XMAX; j++)
      lcd_write(DATA, lcd_video_ram[i][j]);
  }
}

/**************** HIGH LEVEL ************************************************/

/*
 * Puts point with coordinates p to VRAM, returns error if exceed boundaries
 */
gr_error_t vram_put_point(point_t p, point_mode_t mode) {
  /* Assertion of point value */
  if (p.x_point > (LCD_POINT_XMAX - 1))
    return GRAPH_ERROR;
  if (p.y_point > (LCD_POINT_YMAX - 1))
    return GRAPH_ERROR;

  /* Set point in video buffer by mode option */
  switch (mode) {
  case PIXEL_CLEAR: /* Clears pixel in VRAM */
    lcd_video_ram[p.y_point / 8][p.x_point] &= ~(1 << p.y_point % 8);
    break;

  case PIXEL_SET: /* Sets pixel in VRAM */
    lcd_video_ram[p.y_point / 8][p.x_point] |= (1 << p.y_point % 8);
    break;

  case PIXEL_XOR: /* Sets pixel inverted in VRAM */
    lcd_video_ram[p.y_point / 8][p.x_point] ^= (1 << p.y_point % 8);
    break;

  default:
    break;
  }

  return NO_GRAPH_ERROR;
}

/*
 * Puts line from p1 to p2, returns error if exceed boundaries
 * (normal PIXEL_XOR mode implementation)
 */
gr_error_t vram_put_line(point_t p1, point_t p2, point_mode_t mode) {
  register int32_t dx, dy, stepx, stepy, frac;
  gr_error_t err = NO_GRAPH_ERROR;

  /* Calculate DX, DY */
  dx = (int32_t) p2.x_point - (int32_t) p1.x_point;
  dy = (int32_t) p2.y_point - (int32_t) p1.y_point;

  if (dx < 0) {
    dx = -dx;
    stepx = -1;
  } else
    stepx = 1;
  if (dy < 0) {
    dy = -dy;
    stepy = -1;
  } else
    stepy = 1;

  dx *= 2;
  dy *= 2;

  /* Draw first point */
  if (vram_put_point(p1, mode) == GRAPH_ERROR)
    err = GRAPH_ERROR;

  /* Calculate and draw trace by Brezenhem algorithm */
  if (dx > dy) {
    frac = dy - (dx / 2);
    while (p1.x_point != p2.x_point) {
      if (frac >= 0) {
        p1.y_point += stepy;
        frac -= dx;
      }
      p1.x_point += stepx;
      frac += dy;

      if (vram_put_point(p1, mode) == GRAPH_ERROR)
        err = GRAPH_ERROR;
    }
  } else { /* dx < dy */
    frac = dx - (dy / 2);
    while (p1.y_point != p2.y_point) {
      if (frac >= 0) {
        p1.x_point += stepx;
        frac -= dy;
      }
      p1.y_point += stepy;
      frac += dx;

      if (vram_put_point(p1, mode) == GRAPH_ERROR)
        err = GRAPH_ERROR;
    }
  }

  return err;
}

/*
 * Draws rectangle in VRAM from p1 to p2, returns error if exceed boundaries
 * (PIXEL_XOR mode works not well)
 */
gr_error_t vram_put_rect(point_t p1, point_t p2, point_mode_t mode) {
  register int32_t dx, dy;
  point_t __p2;
  static uint8_t rec_flag = 2;
  gr_error_t err = NO_GRAPH_ERROR;

  /* Calculate DX, DY */
  dx = (int32_t) p2.x_point - (int32_t) p1.x_point;
  dy = (int32_t) p2.y_point - (int32_t) p1.y_point;

  /* X line */
  __p2.x_point = p1.x_point + dx;
  __p2.y_point = p1.y_point;
  if (vram_put_line(p1, __p2, mode) == GRAPH_ERROR)
    err = GRAPH_ERROR;

  /* Y line */
  __p2.x_point = p1.x_point;
  __p2.y_point = p1.y_point + dy;
  if (vram_put_line(p1, __p2, mode) == GRAPH_ERROR)
    err = GRAPH_ERROR;

  rec_flag--;

  if (rec_flag) { /* Swap p1<->p2 and draw second part of rectangle by recoursive call */
    if (vram_put_rect(p2, p1, mode) == GRAPH_ERROR)
      err = GRAPH_ERROR;
  } else
    rec_flag = 2;

  return err;
}

/*
 * Draws filled rectangle in VRAM from p1 to p2
 * (normal PIXEL_XOR mode implementation)
 */
gr_error_t vram_put_bar(point_t p1, point_t p2, point_mode_t mode) {
  register int32_t dx, dy;
  point_t __p2;
  gr_error_t err = NO_GRAPH_ERROR;

  /* Calculate DX, DY */
  dx = (int32_t) p2.x_point - (int32_t) p1.x_point;
  dy = (int32_t) p2.y_point - (int32_t) p1.y_point;

  if (labs(dx) < labs(dy)) { /* dy > dx - fill lines along X axis */

    /* Set __p2 */
    __p2.x_point = p1.x_point;
    __p2.y_point = p1.y_point + dy;

    while (!((__p2.x_point == p2.x_point) && (__p2.y_point == p2.y_point))) { /* Draw lines until __p2 reaches p2 */
      if (vram_put_line(p1, __p2, mode) == GRAPH_ERROR)
        err = GRAPH_ERROR;

      /* Select direction */
      if (dx > 0) {
        p1.x_point++;
        __p2.x_point++;
      } else {
        p1.x_point--;
        __p2.x_point--;
      };
    }
    /* Put last line */
    if (vram_put_line(p1, __p2, mode) == GRAPH_ERROR)
      err = GRAPH_ERROR;
  } else { /* dx > dy - fill lines along Y axis */

    /* Set __p2 */
    __p2.y_point = p1.y_point;
    __p2.x_point = p1.x_point + dx;

    while (!((__p2.x_point == p2.x_point) && (__p2.y_point == p2.y_point))) { /* Draw lines until __p2 reaches p2 */
      if (vram_put_line(p1, __p2, mode) == GRAPH_ERROR)
        err = GRAPH_ERROR;

      /* Select direction */
      if (dy > 0) {
        p1.y_point++;
        __p2.y_point++;
      } else {
        p1.y_point--;
        __p2.y_point--;
      };
    }
    /* Put last line */
    if (vram_put_line(p1, __p2, mode) == GRAPH_ERROR)
      err = GRAPH_ERROR;
  }

  return err;
}

/*
 * Draws circle in VRAM from center with radius
 * (PIXEL_XOR works not well)
 */
gr_error_t vram_put_circle(point_t center, uint8_t radius, point_mode_t mode) {
  register int32_t xc = 0, yc, p;
  point_t __p;
  gr_error_t err = NO_GRAPH_ERROR;

  yc = radius;
  p = 3 - (radius * 2);
  while (xc <= yc) {
    __p.x_point = center.x_point + xc;
    __p.y_point = center.y_point + yc;
    if (vram_put_point(__p, mode) == GRAPH_ERROR)
      err = GRAPH_ERROR;

    __p.y_point = center.y_point - yc;
    if (vram_put_point(__p, mode) == GRAPH_ERROR)
      err = GRAPH_ERROR;

    __p.x_point = center.x_point - xc;
    if (vram_put_point(__p, mode) == GRAPH_ERROR)
      err = GRAPH_ERROR;

    __p.y_point = center.y_point + yc;
    if (vram_put_point(__p, mode) == GRAPH_ERROR)
      err = GRAPH_ERROR;

    __p.x_point = center.x_point + yc;
    __p.y_point = center.y_point + xc;
    if (vram_put_point(__p, mode) == GRAPH_ERROR)
      err = GRAPH_ERROR;

    __p.y_point = center.y_point - xc;
    if (vram_put_point(__p, mode) == GRAPH_ERROR)
      err = GRAPH_ERROR;

    __p.x_point = center.x_point - yc;
    if (vram_put_point(__p, mode) == GRAPH_ERROR)
      err = GRAPH_ERROR;

    __p.y_point = center.y_point + xc;
    if (vram_put_point(__p, mode) == GRAPH_ERROR)
      err = GRAPH_ERROR;

    if (p < 0)
      p += ((xc++) * 4) + 6;
    else
      p += (((xc++) - (yc--)) * 4) + 10;
  }

  return err;
}

/*
 *
 */
gr_error_t vram_put_char(point_t p, const font_info_t *font, const char c,
    point_mode_t mode) {
  const unsigned char *bitmap_ptr;
  const unsigned char *__bitmap_ptr;
  unsigned char tmp;
  register uint32_t i, j, k, font_height_bytes, current_char_width;
  point_t __p = p;
  gr_error_t err = NO_GRAPH_ERROR;

  if ((c < (font->start_char)) || (c > (font->end_char)))
    return GRAPH_ERROR;

  /* Get height in bytes */
  font_height_bytes = (font->font_height) / 8;
  if (((font->font_height) % 8))
    font_height_bytes++;

  /* Get width of current character */
  current_char_width = font->descr_array[c - (font->start_char)].char_width;

  /* Get pointer to data */
  bitmap_ptr = &(font->font_bitmap_array[(font->descr_array[c
      - (font->start_char)].char_offset)]);
  /* Copy to temporary ptr */
  __bitmap_ptr = bitmap_ptr;

  for (k = 0; k < current_char_width; k++) { /* Width of character */

    for (i = 0; i < font_height_bytes; i++) { /* Height of character */
      /* Get data to "shift" */
      tmp = *__bitmap_ptr;
      __bitmap_ptr += current_char_width;

      for (j = 0; j < 8; j++) { /* Push byte */

        if ((tmp & 0x01)) {
          if (vram_put_point(__p, mode) == GRAPH_ERROR)
            err = GRAPH_ERROR;
        };
        tmp >>= 1;
        __p.y_point++;
      }
    }
    __p.x_point++;
    __p.y_point = p.y_point;
    __bitmap_ptr = ++bitmap_ptr;
  }

  return err;
}

/*
 *
 */
gr_error_t vram_put_str(point_t p, const font_info_t *font, const char *str,
    point_mode_t mode) {
  unsigned char c;
  register uint32_t current_char_width;
  gr_error_t err = NO_GRAPH_ERROR;

  while ((c = *str++)) {
    /* Check if current symbol is printable */
    if ((c < (font->start_char)) || (c > (font->end_char))) {
      err = GRAPH_ERROR;
      continue;
    };;
    /* Get width of current character */
    current_char_width = font->descr_array[c - (font->start_char)].char_width;

    /* Put character */
    if (vram_put_char(p, font, c, mode) == GRAPH_ERROR)
      err = GRAPH_ERROR;

    /* Increment X by last character width + 1*/
    p.x_point += current_char_width + 1;
  }

  return err;
}

