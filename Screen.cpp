#include "Screen.h"

Screen::Screen(int size_x, int size_y, bool full_screen, const char * name,
                bool vsync) :
    pixels(new Uint32[size_x * size_y]),
    width(size_x),
    height(size_y),
    rshift(16),
    gshift(8),
    bshift(0) {
    if (full_screen) {
        window = SDL_CreateWindow(name, 0, 0, width, height,
                        SDL_WINDOW_FULLSCREEN);
    } else {
        window = SDL_CreateWindow(name,
                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                        width, height, 0);
    }
    if (vsync) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED |
                                                  SDL_RENDERER_PRESENTVSYNC);
    } else {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    }
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                          SDL_TEXTUREACCESS_STREAMING,
                                          width, height);
    Color c{255, 255, 255};
    default_color = format_color(c);
}

Screen::~Screen() {
    delete [] pixels;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

inline Uint32& Screen::pixel_at(int x, int y) {
    return pixels[y * width + x];
}

Uint32 Screen::format_color(Uint8 r, Uint8 g, Uint8 b) {
    return (r << rshift) | (g << gshift) | (b << bshift);
}

Uint32 Screen::format_color(Color c) {
    return format_color(c.r, c.g, c.b);
}

void Screen::commit_screen() {
    SDL_UpdateTexture(texture, NULL, pixels, width * sizeof (Uint32));
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

bool Screen::on_screen(int x, int y) {
    return (x >= 0) && (x < width) && (y >= 0) && (y < height);
}

int Screen::clip_x(int x) {
    if (unlikely(x >= width)) {
        x = width - 1;
    }
    if (unlikely(x < 0)) {
        x = 0;
    }
    return x;
}

int Screen::clip_y(int y) {
    if (unlikely(y >= height)) {
        y = width - 1;
    }
    if (unlikely(y < 0)) {
        y = 0;
    }
    return y;
}

void Screen::set_color(Uint8 r, Uint8 g, Uint8 b) {
    default_color = format_color(r, g, b);
}

void Screen::set_color(Color c) {
    default_color = format_color(c);
}

void Screen::draw_pixel(int x, int y, Uint32 c) {
#if CLIPPED == 1
    if (unlikely(!on_screen(x, y))) {
        return;
    }
#endif
    pixel_at(x, y) = c;
}

void Screen::draw_pixel(int x, int y) {
    draw_pixel(x, y, default_color);
}

void Screen::draw_pixel(int x, int y, Color c) {
    draw_pixel(x, y, format_color(c));
}

void Screen::cls() {
    memset(pixels, 0, width * height * sizeof(Uint32));
}

void Screen::fill_screen(Uint32 c) {
    for (int i = 0; i < width * height; ++i) {
        pixels[i] = c;
    }
}

void Screen::fill_screen() {
    fill_screen(default_color);
}

void Screen::fill_screen(Color c) {
    fill_screen(format_color(c));
}

// Prefer drawing horizontal lines to vertical ones
void Screen::hor_line(int y, int x1, int x2, Uint32 c) {
    int iter_i;
    int end_i;
#if CLIPPED == 1
    if (unlikely(y >= height || y < 0)) {
        return;
    }
    x1 = clip_x(x1);
    x2 = clip_x(x2);
#endif
    if (x1 < x2) {
        iter_i = y * width + x1;
        end_i = iter_i + (x2 - x1);
    } else {
        iter_i = y * width + x2;
        end_i = iter_i + (x1 - x2);
    }
    while (true) {
        pixels[iter_i] = c;
        if (iter_i == end_i) {
            break;
        }
        iter_i++;
    }
}

void Screen::hor_line(int y, int x1, int x2) {
    hor_line(y, x1, x2, default_color);
}

void Screen::hor_line(int y, int x1, int x2, Color c) {
    hor_line(y, x1, x2, format_color(c));
}

void Screen::ver_line(int x, int y1, int y2, Uint32 c) {
    int iter_i;
    int end_i;
#if CLIPPED == 1
    if (unlikely(x >= width || x < 0)) {
        return;
    }
    y1 = clip_y(y1);
    y2 = clip_y(y2);
#endif
    if (y1 < y2) {
        iter_i = y1 * width + x;
        end_i = iter_i + (y2 - y1) * width;
    } else {
        iter_i = y2 * width + x;
        end_i = iter_i + (y1 - y2) * width;
    }
    while (true) {
        pixels[iter_i] = c;
        if (iter_i == end_i) {
            break;
        }
        iter_i += width;
    }
}

void Screen::ver_line(int x, int y1, int y2) {
    ver_line(x, y1, y2, default_color);
}

void Screen::ver_line(int x, int y1, int y2, Color c) {
    ver_line(x, y1, y2, format_color(c));
}

void Screen::draw_rect(int x1, int y1, int x2, int y2, Uint32 c) {
    hor_line(y1, x1, x2, c);
    hor_line(y2, x1, x2, c);
    ver_line(x1, y1, y2, c);
    ver_line(x2, y1, y2, c);
}

void Screen::draw_rect(int x1, int y1, int x2, int y2) {
    draw_rect(x1, y1, x2, y2, default_color);
}

void Screen::draw_rect(int x1, int y1, int x2, int y2, Color c) {
    draw_rect(x1, y1, x2, y2, format_color(c));
}

void Screen::fill_rect(int x1, int y1, int x2, int y2, Uint32 c) {
    int iter_y;
    int end_y;
    int start_x;
    int dx;
#if CLIPPED == 1
    if (unlikely(!on_screen(x1, y1) && !on_screen(x2, y2))) {
        return;
    }
    x1 = clip_x(x1);
    x2 = clip_x(x2);
    y1 = clip_y(y1);
    y2 = clip_y(y2);
#endif
    if (y1 < y2) {
        iter_y = y1;
        end_y = y2;
    } else {
        iter_y = y2;
        end_y = y1;
    }
    if (x1 < x2) {
        start_x = x1;
        dx = x2 - x1;
    } else {
        start_x = x2;
        dx = x1 - x2;
    }
    while (true) {
        int iter_x = iter_y * width + start_x;
        int end_x = iter_x + dx;
        while (true) {
            pixels[iter_x] = c;
            if (iter_x == end_x) {
                break;
            }
            iter_x++;
        }
        if (iter_y == end_y) {
            break;
        }
        iter_y++;
    }
}

void Screen::fill_rect(int x1, int y1, int x2, int y2) {
    fill_rect(x1, y1, x2, y2, default_color);
}

void Screen::fill_rect(int x1, int y1, int x2, int y2, Color c) {
    fill_rect(x1, y1, x2, y2, format_color(c));
}

// Code taken from Rosetta Code
void Screen::draw_line(int x1, int y1, int x2, int y2, Uint32 c) {
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = abs(y2 - y1), sy = y1 < y2 ? 1 : -1; 
    int err = (dx > dy ? dx : -dy) / 2, e2;
    while (true) {
        draw_pixel(x1, y1, c);
        if (x1 == x2 && y1 == y2) {
            break;
        }
        e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y1 += sy;
        }
    }
}

void Screen::draw_line(int x1, int y1, int x2, int y2) {
    draw_line(x1, y1, x2, y2, default_color);
}

void Screen::draw_line(int x1, int y1, int x2, int y2, Color c) {
    draw_line(x1, y1, x2, y2, format_color(c));
}

// Code taken from Rosetta Code
void Screen::draw_circle(int x, int y, int r, Uint32 c) {
    int f = 1 - r;
    int ddF_x = 0;
    int ddF_y = -2 * r;
    int iter_x = 0;
    int iter_y = r;

    draw_pixel(x, y + r, c);
    draw_pixel(x, y - r, c);
    draw_pixel(x + r, y, c);
    draw_pixel(x - r, y, c);

    while(iter_x < iter_y) {
        if(f >= 0) {
            iter_y--;
            ddF_y += 2;
            f += ddF_y;
        }
        iter_x++;
        ddF_x += 2;
        f += ddF_x + 1;    
        draw_pixel(x + iter_x, y + iter_y, c);
        draw_pixel(x - iter_x, y + iter_y, c);
        draw_pixel(x + iter_x, y - iter_y, c);
        draw_pixel(x - iter_x, y - iter_y, c);
        draw_pixel(x + iter_y, y + iter_x, c);
        draw_pixel(x - iter_y, y + iter_x, c);
        draw_pixel(x + iter_y, y - iter_x, c);
        draw_pixel(x - iter_y, y - iter_x, c);
    }
}

void Screen::draw_circle(int x, int y, int r) {
    draw_circle(x, y, r, default_color);
}

void Screen::draw_circle(int x, int y, int r, Color c) {
    draw_circle(x, y, r, format_color(c));
}

// Code taken from Rosetta Code
void Screen::fill_circle(int x, int y, int r, Uint32 c) {
    int f = 1 - r;
    int ddF_x = 0;
    int ddF_y = -2 * r;
    int iter_x = 0;
    int iter_y = r;

    draw_pixel(x, y + r, c);
    draw_pixel(x, y - r, c);
    hor_line(y, x - r, x + r, c);

    while(iter_x < iter_y) {
        if(f >= 0) {
            iter_y--;
            ddF_y += 2;
            f += ddF_y;
        }
        iter_x++;
        ddF_x += 2;
        f += ddF_x + 1;    
        hor_line(y - iter_y, x - iter_x, x + iter_x, c);
        hor_line(y - iter_x, x - iter_y, x + iter_y, c);
        hor_line(y + iter_x, x - iter_y, x + iter_y, c);
        hor_line(y + iter_y, x - iter_x, x + iter_x, c);
    }
}

void Screen::fill_circle(int x, int y, int r) {
    fill_circle(x, y, r, default_color);
}

void Screen::fill_circle(int x, int y, int r, Color c) {
    fill_circle(x, y, r, format_color(c));
}

