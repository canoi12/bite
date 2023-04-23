#ifndef _BIT_ENGINE_
#define _BIT_ENGINE_

#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32)
    #include <windows.h>
    #include <tchar.h>
    #include <gl/GL.h>
    #include <gl/GLU.h>
#elif defined(__EMSCRIPTEN__)
    #include <emscripten.h>
    #include <emscripten/html5.h>
    #include <GL/gl.h>
    // #include <GLES2/gl2.h>
#else
    #include <unistd.h>
    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
    #include <GL/gl.h>
    #include <GL/glx.h>
#endif

#define BITE_VERSION "0.1.0"
#define BITE_API extern

typedef unsigned char be_u8;
typedef unsigned short be_u16;
typedef unsigned int be_u32;
typedef unsigned long int be_u64;

typedef struct be_Window be_Window;
typedef struct be_Event be_Event;

typedef struct be_Texture be_Texture;
typedef struct be_Framebuffer be_Framebuffer;
typedef struct be_Shader be_Shader;
typedef struct be_Font be_Font;

typedef void(*be_EventCallback)(be_Event*);

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

struct be_Window {
    int x, y;
    int width, height;
#if defined(_WIN32)
    HWND handle;
    HINSTANCE hInstance;
    HDC devContext;
    HGLRC glContext;
#elif defined(__EMSCRIPTEN__)
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE handle;
#else
    Display* display;
    Window handle;
    GLXContext glContext;
    XVisualInfo* visual;
#endif
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

#if defined(__cplusplus)
extern "C" {
#endif

/*********************
 * Core
 *********************/
BITE_API int bite_init(int flags);
BITE_API void bite_quit(void);

BITE_API void bite_poll_events(void);

/*********************
 * Window
 *********************/
BITE_API be_Window* bite_create_window(const char* title, int w, int h, int flags);
BITE_API void bite_destroy_window(be_Window* window);

BITE_API void bite_window_swap(be_Window* window);

BITE_API void bite_register_callback(int type, be_EventCallback ev);

BITE_API void bite_window_get_size(be_Window* window, int* w, int* h);
BITE_API int bite_window_get_mouse_button(be_Window* window, int btn);
BITE_API void bite_window_get_mouse_pos(be_Window* window, int* x, int* y);

/*********************
 * Render
 *********************/

BITE_API void bite_render_clear(void);

/*********************
 * Timer
 *********************/
BITE_API void bite_timer_sleep(be_u64 ms);
BITE_API be_u64 bite_timer_tick(void);

#if defined(__cplusplus)
}
#endif

#endif /* _BIT_ENGINE_ */