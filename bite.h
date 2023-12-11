#ifndef _BIT_ENGINE_
#define _BIT_ENGINE_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BITE_VERSION "0.1.0"
#define BITE_API extern

typedef unsigned char be_u8;
typedef unsigned short be_u16;
typedef unsigned int be_u32;
typedef unsigned long int be_u64;

typedef struct {float x, y, w, h; } be_Rect;

typedef struct be_Context be_Context;

typedef struct be_Window be_Window;
typedef struct be_Event be_Event;

typedef struct be_Texture be_Texture;
typedef struct be_Framebuffer be_Framebuffer;
typedef struct be_Shader be_Shader;
typedef struct be_Font be_Font;

typedef void(*be_EventCallback)(be_Context*, be_Event*);

#define BITE_TRUE  1
#define BITE_FALSE 0

#define BITE_OK 0
#define BITE_ERROR -1

#define BITE_RESULT int
#define BITE_BOOL be_u8

enum {
    BITE_NONE = 0,
    BITE_QUIT,

    BITE_KEY_PRESSED,
    BITE_KEY_RELEASED,

    BITE_WINDOW_RESIZE,
    BITE_WINDOW_MOVE,
    BITE_WINDOW_MINIMIZE,
    BITE_WINDOW_MAXIMIZE,
    BITE_WINDOW_RESTORE,
    BITE_WINDOW_CLOSE,

    BITE_EVENTS
};

enum {
    BITEK_NONE = 0,

    BITEK_BACKSPACE,
    BITEK_TAB,
    BITEK_RETURN,
    BITEK_SHIFT,
    BITEK_CONTROL,

    BITEK_ESCAPE,

    BITEK_SPACE,

    BITEK_PAGEUP,
    BITEK_PAGEDOWN,

    BITEK_LEFT,
    BITEK_UP,
    BITEK_RIGHT,
    BITEK_DOWN,

    BITEK_END,
    BITEK_HOME,
    BITEK_INSERT,
    BITEK_DELETE,

    BITEK_0,
    BITEK_1,
    BITEK_2,
    BITEK_3,
    BITEK_4,
    BITEK_5,
    BITEK_6,
    BITEK_7,
    BITEK_8,
    BITEK_9,

    BITEK_A,
    BITEK_B,
    BITEK_C,
    BITEK_D,
    BITEK_E,
    BITEK_F,
    BITEK_G,
    BITEK_H,
    BITEK_I,
    BITEK_J,
    BITEK_K,
    BITEK_L,
    BITEK_M,
    BITEK_N,
    BITEK_O,
    BITEK_P,
    BITEK_Q,
    BITEK_R,
    BITEK_S,
    BITEK_T,
    BITEK_U,
    BITEK_V,
    BITEK_W,
    BITEK_X,
    BITEK_Y,
    BITEK_Z,

    BITEK_LWIN,
    BITEK_RWIN,
    BITEK_APPS,

    BITEK_NUMPAD0,
    BITEK_NUMPAD1,
    BITEK_NUMPAD2,
    BITEK_NUMPAD3,
    BITEK_NUMPAD4,
    BITEK_NUMPAD5,
    BITEK_NUMPAD6,
    BITEK_NUMPAD7,
    BITEK_NUMPAD8,
    BITEK_NUMPAD9,
    BITEK_MULTIPLY,
    BITEK_ADD,
};

enum {
    BITE_POINTS = 0,
    BITE_LINES,
    BITE_TRIANGLES
};

struct be_Event {
    int type;
    union {
        struct {
            be_Window* handle;
            const char title[128];
            int x, y;
            void* data0;
            void* data1;
        } window;
        struct {
            int keycode;
        } key;
    };
};

typedef struct {
    struct {
        char title[128];
        int width, height;
        int flags;
    } window;
} be_Config;

#if defined(__cplusplus)
extern "C" {
#endif
    
BITE_API void bite_simple_triangle(be_Context* ctx);

/*********************
 * Core
 *********************/
BITE_API be_Config bite_init_config(const char* title, int w, int h);

BITE_API be_Context* bite_create(const be_Config* conf);
BITE_API void bite_destroy(be_Context* ctx);

BITE_API void bite_register_callback(be_Context* ctx, int type, be_EventCallback callback);

BITE_API int bite_should_close(be_Context* ctx);
BITE_API void bite_set_should_close(be_Context* ctx, int should_close);

BITE_API void bite_poll_events(be_Context* ctx);

BITE_API void bite_swap(be_Context* ctx);

/*********************
 * Window
 *********************/
BITE_API void bite_set_window_width(be_Context* ctx, int width);
BITE_API void bite_set_window_height(be_Context* ctx, int height);
BITE_API void bite_set_window_size(be_Context* ctx, int width, int height);

BITE_API int bite_get_window_width(be_Context* ctx);
BITE_API int bite_get_window_height(be_Context* ctx);
BITE_API void bite_get_window_size(int* w, int* h);

BITE_API void bite_get_mouse_pos(int* x, int* y);

/*********************
 * Render
 *********************/

BITE_API be_Shader* bite_create_shader(const char* vert, const char* src);
BITE_API void bite_destroy_shader(be_Shader* shader);

BITE_API be_Texture* bite_create_texture(int w, int h, int format, void* data);
BITE_API void bite_destroy_texture(be_Texture* texture);

BITE_API be_Framebuffer* bite_create_framebuffer(void);
BITE_API void bite_destroy_framebuffer(be_Framebuffer* fbo);
BITE_API BITE_RESULT bite_framebuffer_attach_texture(be_Framebuffer* fbo, be_Texture* texture);

BITE_API void bite_render_clear_color(float r, float g, float b, float a);
BITE_API void bite_render_clear(void);

BITE_API void bite_bind_framebuffer(be_Framebuffer* fbo);
BITE_API void bite_bind_texture(be_Texture* tex);
BITE_API void bite_use_shader(be_Shader* shader);

BITE_API void bite_render_point(float x, float y);
BITE_API void bite_render_line(float x0, float y0, float x1, float y1);
BITE_API void bite_render_rect(be_Rect* rect);
BITE_API void bite_render_rectangle(float x, float y, float w, float h);
BITE_API void bite_render_circle(float x, float y, float radius);
BITE_API void bite_render_triangle(float x0, float y0, float x1, float y1, float x2, float y2);

BITE_API void bite_render_texture(be_Texture* tex, be_Rect* src, be_Rect* dest);

/*********************
 * Timer
 *********************/
BITE_API void bite_sleep(be_u64 ms);
BITE_API be_u64 bite_tick(void);

BITE_API void bite_timer_sleep(be_u64 ms);
BITE_API be_u64 bite_timer_tick(void);

#if defined(__cplusplus)
}
#endif

#endif /* _BIT_ENGINE_ */
