#ifndef SCREEN_H
#define SCREEN_H

#include <SDL2/SDL.h>

#define CLIPPED 0
#define likely(x)   __builtin_expect((x),1)
#define unlikely(x) __builtin_expect((x),0)

struct Color {
    Uint8 r;
    Uint8 g;
    Uint8 b;
};

class Screen {
    private:
        Uint32* pixels;
        Uint32 default_color;
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* texture;

        inline Uint32& pixel_at(int x, int y);

    public:
        const int width;
        const int height;
        const int rshift;
        const int gshift;
        const int bshift;
        const bool vsynced;
        const bool direct_draw; /* Set to true for increased drawing
                                   performance. This may result in visual
                                   artifacts if drawing right after calling
                                   commit_screen */

        Screen(int size_x, int size_y, bool full_screen, const char * name,
               bool vsync, bool direct = false);
        ~Screen();

        void commit_screen();
        bool on_screen(int x, int y);
        int clip_x(int x);
        int clip_y(int y);
        Uint32 format_color(Uint8 r, Uint8 g, Uint8 b);
        Uint32 format_color(Color c);
        void set_color(Uint8 r, Uint8 g, Uint8 b);
        void set_color(Color c);
        void draw_pixel(int x, int y, Uint32 c);
        void draw_pixel(int x, int y);
        void draw_pixel(int x, int y, Color c);
        void cls();
        void fill_screen(Uint32 c);
        void fill_screen();
        void fill_screen(Color c);
        void hor_line(int y, int x1, int x2, Uint32 c);
        void hor_line(int y, int x1, int x2);
        void hor_line(int y, int x1, int x2, Color c);
        void ver_line(int x, int y1, int y2, Uint32 c);
        void ver_line(int x, int y1, int y2);
        void ver_line(int x, int y1, int y2, Color c);
        void draw_rect(int x1, int y1, int x2, int y2, Uint32 c);
        void draw_rect(int x1, int y1, int x2, int y2);
        void draw_rect(int x1, int y1, int x2, int y2, Color c);
        void fill_rect(int x1, int y1, int x2, int y2, Uint32 c);
        void fill_rect(int x1, int y1, int x2, int y2);
        void fill_rect(int x1, int y1, int x2, int y2, Color c);
        void draw_line(int x1, int y1, int x2, int y2, Uint32 c);
        void draw_line(int x1, int y1, int x2, int y2);
        void draw_line(int x1, int y1, int x2, int y2, Color c);
        void draw_circle(int x, int y, int r, Uint32 c);
        void draw_circle(int x, int y, int r);
        void draw_circle(int x, int y, int r, Color c);
        void fill_circle(int x, int y, int r, Uint32 c);
        void fill_circle(int x, int y, int r);
        void fill_circle(int x, int y, int r, Color c);
        void write_tga(const char * name);
};

#endif
