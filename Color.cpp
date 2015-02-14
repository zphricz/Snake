#include <algorithm>
#include "Color.h"

using namespace std;

// Performs rounding division of unsigned integers
static Uint32 idiv(Uint32 num, Uint32 den) {
    return num / den + ((num % den) * 2 >= den);
}

Color::Color() {
}

Color::Color(Uint8 r, Uint8 g, Uint8 b) :
    r(r),
    g(g),
    b(b),
    a(255) {
}

Color::Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a) :
    r(r),
    g(g),
    b(b),
    a(a) {
}

Color Color::operator*(float factor) const {
    Uint8 out_r = min((int)(r * factor), 255);
    Uint8 out_g = min((int)(g * factor), 255);
    Uint8 out_b = min((int)(b * factor), 255);
    return {out_r, out_g, out_b, a};
}

Color Color::operator/(float factor) const {
    return {Uint8(r / factor), Uint8(g / factor), Uint8(b / factor), a};
}

Color& Color::operator*=(float factor) {
    return *this = *this * factor;
}

Color& Color::operator/=(float factor) {
    return *this = *this / factor;
}

Color blend(const Color& fg, const Color& bg) {
    Uint32 temp_a = fg.a * 255 + bg.a * (255 - fg.a);
    Uint32 out_a = fg.a + idiv(bg.a * (255 - fg.a), 255);
    if (temp_a == 0) {
        return {0, 0, 0, 0};
    }
    Uint32 out_r = idiv(fg.r * fg.a * 255 + bg.r * bg.a * (255 - fg.a), temp_a);
    Uint32 out_g = idiv(fg.g * fg.a * 255 + bg.g * bg.a * (255 - fg.a), temp_a);
    Uint32 out_b = idiv(fg.b * fg.a * 255 + bg.b * bg.a * (255 - fg.a), temp_a);
    return {Uint8(out_r), Uint8(out_g), Uint8(out_b), Uint8(out_a)};
}

