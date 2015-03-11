#ifndef COLOR_H
#define COLOR_H

#include <SDL2/SDL.h>

SDL_Color blend(const SDL_Color& fg, const SDL_Color& bg);
SDL_Color operator*(const SDL_Color& lhs, float factor);
SDL_Color& operator*=(SDL_Color& lhs, float factor);
SDL_Color operator/(const SDL_Color& lhs, float factor);
SDL_Color& operator/=(SDL_Color& lhs, float factor);

#endif
