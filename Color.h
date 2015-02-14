#ifndef COLOR_H
#define COLOR_H

#include <SDL2/SDL.h>

struct Color {
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;

    Color();
    Color(Uint8 r, Uint8 g, Uint8 b);
    Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

    Color operator*(float factor) const;
    Color operator/(float factor) const;
    Color& operator*=(float factor);
    Color& operator/=(float factor);
};

Color blend(const Color& fg, const Color& bg);

#endif
