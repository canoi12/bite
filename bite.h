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

typedef struct be_Context be_Context;

typedef struct be_Window be_Window;
typedef struct be_Event be_Event;

typedef struct be_Texture be_Texture;
typedef struct be_Framebuffer be_Framebuffer;
typedef struct be_Shader be_Shader;
typedef struct be_Font be_Font;

typedef void(*be_EventCallback)(be_Context*, be_Event*);

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
    BITEK_ESCAPE,
    BITEK_RETURN,
    BITEK_UP,
    BITEK_RIGHT,
    BITEK_DOWN,
    BITEK_LEFT
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
    
BITE_API void bite_simple_triangle(void);

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

BITE_API void bite_render_clear(void);

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