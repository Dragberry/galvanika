#ifndef INC_DISPLAYS_GRAPHIC_DISPLAY_HPP_
#define INC_DISPLAYS_GRAPHIC_DISPLAY_HPP_

#include <displays/display.hpp>

struct Point {
  uint8_t x;
  uint8_t y;
};

enum PointMode {
  PIXEL_CLEAR = 0,
  PIXEL_SET = 1,
  PIXEL_XOR = 2
};

enum GraphicState {
  GRAPH_OK = 0,
  GRAPH_ERROR = 1
};

/* Font descriptor type */
struct FontDescriptor {
  uint16_t char_width;
  uint16_t char_offset;
};

/* Font info type */
struct FontInfo {
  uint8_t font_height;
  uint8_t start_char;
  uint8_t end_char;
  const FontDescriptor *descr_array;
  const uint8_t *font_bitmap_array;
};

template <uint8_t Width, uint8_t Height>
class GraphicDisplay : public Display {
private:
  uint8_t vram[(Height + 7) / 8][Width];

public:
  GraphicDisplay<Width, Height>(DisplayInterface * const interface) :
    Display(interface) {
  }

  void vram_update() {
    for (uint8_t y = 0; y < interface->rows; y++) {
      interface->set_cursor(0, y);
      for (uint8_t x = 0; x < interface->width; x++) {
        interface->write_data(vram[y][x]);
      }
    }
  }

  void vram_fill(bool filled) {
    for (uint8_t y = 0; y < interface->rows; y++) {
      for (uint8_t x = 0; x < interface->width; x++) {
        vram[y][x] = filled ? 0xFF: 0x00;
      }
    }
  }

  GraphicState vram_put_point(Point point, PointMode mode) {
    if (point.x >= interface->width) {
      return GRAPH_ERROR;
    }
    if (point.y >= interface->height) {
      return GRAPH_ERROR;
    }

    switch (mode) {
    case PIXEL_CLEAR:
      vram[point.y / 8][point.x] &= ~(1 << point.y % 8);
      break;
    case PIXEL_SET:
      vram[point.y / 8][point.x] |= (1 << point.y % 8);
      break;
    case PIXEL_XOR:
      vram[point.y / 8][point.x] ^= (1 << point.y % 8);
      break;
    default:
      break;
    }
    return GRAPH_OK;
  }

  GraphicState vram_put_line(Point p1, Point p2, PointMode mode) {
    int16_t dx, dy, stepx, stepy, frac;
    GraphicState err = GRAPH_OK;

    /* Calculate DX, DY */
    dx = (int16_t) p2.x - (int16_t) p1.x;
    dy = (int16_t) p2.y - (int16_t) p1.y;

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
    if (vram_put_point(p1, mode) == GRAPH_ERROR) {
      err = GRAPH_ERROR;
    }

    /* Calculate and draw trace by Brezenhem algorithm */
    if (dx > dy) {
      frac = dy - (dx / 2);
      while (p1.x != p2.x) {
        if (frac >= 0) {
          p1.y += stepy;
          frac -= dx;
        }
        p1.x += stepx;
        frac += dy;

        if (vram_put_point(p1, mode) == GRAPH_ERROR)
          err = GRAPH_ERROR;
      }
    } else { /* dx < dy */
      frac = dx - (dy / 2);
      while (p1.y != p2.y) {
        if (frac >= 0) {
          p1.x += stepx;
          frac -= dy;
        }
        p1.y += stepy;
        frac += dx;

        if (vram_put_point(p1, mode) == GRAPH_ERROR) {
          err = GRAPH_ERROR;
        }
      }
    }

    return err;
  }

  GraphicState vram_put_rect(Point p1, Point p2, PointMode mode) {
    int16_t dx, dy;
    Point __p2;
    static uint8_t rec_flag = 2;
    GraphicState err = GRAPH_OK;

    /* Calculate DX, DY */
    dx = (int16_t) p2.x - (int16_t) p1.x;
    dy = (int16_t) p2.y - (int16_t) p1.y;

    /* X line */
    __p2.x = p1.x + dx;
    __p2.y = p1.y;
    if (vram_put_line(p1, __p2, mode) == GRAPH_ERROR) {
      err = GRAPH_ERROR;
    }

    /* Y line */
    __p2.x = p1.x;
    __p2.y = p1.y + dy;
    if (vram_put_line(p1, __p2, mode) == GRAPH_ERROR) {
      err = GRAPH_ERROR;
    }

    rec_flag--;

    if (rec_flag) { /* Swap p1<->p2 and draw second part of rectangle by recoursive call */
      if (vram_put_rect(p2, p1, mode) == GRAPH_ERROR) {
        err = GRAPH_ERROR;
      }
    } else {
      rec_flag = 2;
    }
    return err;
  }

  GraphicState vram_put_circle(Point center, uint8_t radius, PointMode mode) {
    int16_t xc = 0, yc, p;
    Point __p;
    GraphicState err = GRAPH_OK;

    yc = radius;
    p = 3 - (radius * 2);
    while (xc <= yc) {
      __p.x = center.x + xc;
      __p.y = center.y + yc;
      if (vram_put_point(__p, mode) == GRAPH_ERROR) {
        err = GRAPH_ERROR;
      }

      __p.y = center.y - yc;
      if (vram_put_point(__p, mode) == GRAPH_ERROR) {
        err = GRAPH_ERROR;
      }

      __p.x = center.x - xc;
      if (vram_put_point(__p, mode) == GRAPH_ERROR) {
        err = GRAPH_ERROR;
      }

      __p.y = center.y + yc;
      if (vram_put_point(__p, mode) == GRAPH_ERROR) {
        err = GRAPH_ERROR;
      }

      __p.x = center.x + yc;
      __p.y = center.y + xc;
      if (vram_put_point(__p, mode) == GRAPH_ERROR) {
        err = GRAPH_ERROR;
      }

      __p.y = center.y - xc;
      if (vram_put_point(__p, mode) == GRAPH_ERROR) {
        err = GRAPH_ERROR;
      }

      __p.x = center.x - yc;
      if (vram_put_point(__p, mode) == GRAPH_ERROR) {
        err = GRAPH_ERROR;
      }

      __p.y = center.y + xc;
      if (vram_put_point(__p, mode) == GRAPH_ERROR) {
        err = GRAPH_ERROR;
      }

      if (p < 0)
        p += ((xc++) * 4) + 6;
      else
        p += (((xc++) - (yc--)) * 4) + 10;
    }

    return err;
  }

  GraphicState vram_put_char(Point p, const FontInfo *font, const char c, PointMode mode) {
    const unsigned char *bitmap_ptr;
    const unsigned char *__bitmap_ptr;
    unsigned char tmp;
    uint16_t i, j, k, font_height_bytes, current_char_width;
    Point __p = p;
    GraphicState err = GRAPH_OK;

    if ((c < (font->start_char)) || (c > (font->end_char))) {
      return GRAPH_ERROR;
    }

    /* Get height in bytes */
    font_height_bytes = (font->font_height) / 8;
    if (((font->font_height) % 8)) {
      font_height_bytes++;
    }

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
            if (vram_put_point(__p, mode) == GRAPH_ERROR) {
              err = GRAPH_ERROR;
            }
          };
          tmp >>= 1;
          __p.y++;
        }
      }
      __p.x++;
      __p.y = p.y;
      __bitmap_ptr = ++bitmap_ptr;
    }
    return err;
  }

  GraphicState vram_put_str(Point p, const FontInfo *font, const char *str, PointMode mode) {
    unsigned char c;
    uint16_t current_char_width;
    GraphicState err = GRAPH_OK;

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
      p.x += current_char_width + 1;
    }

    return err;
  }

  GraphicState vram_put_image(const char* img) {
    for (register uint8_t y = 0; y < interface->rows; y++) {
      interface->set_cursor(0, y);
      for (register uint8_t x = 0; x < interface->width; x++) {
        vram[y][x] = img[interface->width * y + x];
      }
    }
  }
};

#endif
