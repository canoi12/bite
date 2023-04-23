#include "bite.h"

#if defined(_WIN32)
    #include <windows.h>
    #include <tchar.h>
    #include <gl/GL.h>
    #include <gl/GLU.h>
#elif defined(__EMSCRIPTEN__)
    
    #include <GL/gl.h>
    // #include <GLES2/gl2.h>
#else
    #if defined(__EMSCRIPTEN__)
        #include <emscripten.h>
        #include <emscripten/html5.h>
    #else
        #include <unistd.h>
        #include <X11/Xlib.h>
        #include <X11/Xutil.h>
        #include <GL/glx.h>
    #endif
    #include <GL/gl.h>
    #include <time.h>
#endif

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

struct be_Render {
    int mode;
};

struct be_Context {
    int running;
    be_Window window;
    be_EventCallback callbacks[BITE_EVENTS];
};

static be_Context _context;

be_Context* _create_context(const be_Config* conf);
void _poll_events(be_Context* ctx);

#if defined(_WIN32)
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    be_Event e;
    e.type = 0;
    switch (msg) {
        case WM_CLOSE: {
            e.type = BITE_WINDOW_CLOSE;
        }
        break;
        case WM_DESTROY: PostQuitMessage(0); break;
        case WM_SYSKEYDOWN:
        case WM_KEYDOWN: {
            e.type = BITE_KEY_PRESSED;
            e.key.keycode = (int)wParam;
        }
        break;
    }
    be_EventCallback fn = _callbacks[e.type];
    if (fn) fn(&e);
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
#endif

be_Config bite_init_config(const char* title, int w, int h) {
    be_Config c = {0};
    title = title != NULL ? title : "bitEngine";
    strcpy(c.window.title, title);
    c.window.width = w;
    c.window.height = h;
    return c;
}

be_Context* bite_create(const be_Config* conf) {
    be_Context* ctx = _create_context(conf);
    if (!ctx) return ctx;
    ctx->running = 1;
    ctx->window.width = conf->window.width;
    ctx->window.height = conf->window.height;
    for (int i = 0; i < BITE_EVENTS; i++) {
        ctx->callbacks[i] = NULL;
    }
    return ctx;
}

void bite_destroy(be_Context* ctx) {
    if (!ctx) return;
    be_Window* window = &(ctx->window);
#if defined(_WIN32)
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(window->glContext);
    ReleaseDC(window->handle, window->devContext);
    DestroyWindow(window->handle);
#elif defined(__EMSCRIPTEN__)
    emscripten_webgl_destroy_context(window->handle);
#else
    glXDestroyContext(window->display, window->glContext);
    XDestroyWindow(window->display, window->handle);
    XCloseDisplay(window->display);
#endif
    free(ctx);
}

void bite_register_callback(be_Context* ctx, int type, be_EventCallback callback) {
    if (!ctx) return;
    if (type < BITE_QUIT || type >= BITE_EVENTS) return;
    ctx->callbacks[type] = callback;
}

int bite_should_close(be_Context* ctx) {
    if (!ctx) return 1;
    return !(ctx->running);
}

void bite_set_should_close(be_Context* ctx, int should_close) {
    if (!ctx) return;
    ctx->running = !should_close;
}

void bite_poll_events(be_Context* ctx) {
    if (!ctx) return;
    _poll_events(ctx);
}

void bite_swap(be_Context* ctx) {
    if (!ctx) return;
    be_Window* window = &(ctx->window);
#if defined(_WIN32)
    SwapBuffers(window->devContext);
#elif defined(__EMSCRIPTEN__)
    emscripten_webgl_commit_frame();
#else
    glXSwapBuffers(window->display, window->handle);
#endif
}

/*********************
 * Window
 *********************/
int bite_get_window_width(be_Context* ctx) {
    if (!ctx) return -1;
    return ctx->window.width;
}


/*********************
 * Timer
 *********************/
void bite_sleep(be_u64 ms) {
#if defined(_WIN32)
    Sleep(ms);
#elif defined(__EMSCRIPTEN__)
    emscripten_sleep((be_u32)ms);
#else
    sleep(ms / 1000);
#endif
}

be_u64 bite_tick(void) {
    be_u64 tick = 0;
#if defined(_WIN32)
    LARGE_INTEGER freq, count;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&count);
    tick = (be_u64)(count.QuadPart * 1000.0 / freq.QuadPart);
#else
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    tick = time.tv_sec * 1000 + (time.tv_nsec / 1000000);
#endif
    return tick;
}

/*********************
 * Internal
 *********************/
be_Context* _create_context(const be_Config* conf) {
    be_Context* ctx = NULL;
#if defined(_WIN32)
    HWND handle;
    HMODULE hInstance = GetModuleHandle(0);
    
    const TCHAR windowClass[] = _T("BitEngine");

    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = windowClass;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL,
            _T("Call to RegisterClassEx failed"),
            _T("bit Engine"),
            NULL);
        fprintf(stderr, "Error code: %d\n", GetLastError());
        return window;
    }

    handle = CreateWindow(
        windowClass,
        title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        w, h,
        NULL, NULL, hInstance, NULL
    );
    if (!handle) {
        MessageBox(NULL,
            _T("Call to CreateWindow failed"),
            _T("bit Engine"),
            NULL);
        fprintf(stderr, "Error code: %d\n", GetLastError());
        return window;
    }

    HDC hdc = GetDC(handle);

    PIXELFORMATDESCRIPTOR pfd;
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixelFormat, &pfd);

    HGLRC hglrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hglrc);

    ctx = malloc(sizeof(*ctx));
    be_Window* window = &(ctx->window);
    window->handle = handle;
    window->hInstance = hInstance;
    window->devContext = hdc;
    window->glContext = hglrc;

    ShowWindow(handle, SW_SHOWDEFAULT);
    UpdateWindow(handle);
#elif defined(__EMSCRIPTEN__)
    EmscriptenWebGLContextAttributes attr;
    emscripten_webgl_init_context_attributes(&attr);
    attr.alpha = 0;
    ctx = malloc(sizeof(*ctx));
    ctx->window.handle = emscripten_webgl_create_context("#canvas", &attr);
    emscripten_webgl_make_context_current(window->handle);
#else
    Display* dpy;
    Window handle;
    Screen* scr;
    int scrId;
    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
        fprintf(stderr, "Could not open X11 display\n");
        return ctx;
    }
    scr = DefaultScreenOfDisplay(dpy);
    scrId = DefaultScreen(dpy);
    
    GLint majorGLX, minorGLX;
    glXQueryVersion(dpy, &majorGLX, &minorGLX);
    if (majorGLX <= 1 && minorGLX < 2) {
        fprintf(stderr, "GLX 1.2 or greater is required\n");
        XCloseDisplay(dpy);
        return ctx;
    } else {
        fprintf(stdout, "GLX version: %d.%d\n", majorGLX, minorGLX);
    }

    GLint glxAttribs[] = {
		GLX_RGBA,
		GLX_DOUBLEBUFFER,
		GLX_DEPTH_SIZE,     24,
		GLX_STENCIL_SIZE,   8,
		GLX_RED_SIZE,       8,
		GLX_GREEN_SIZE,     8,
		GLX_BLUE_SIZE,      8,
		GLX_SAMPLE_BUFFERS, 0,
		GLX_SAMPLES,        0,
		None
	};
	XVisualInfo* visual = glXChooseVisual(dpy, scrId, glxAttribs);
    if (!visual) {
        fprintf(stderr, "Failed to create correct visual window\n");
        XCloseDisplay(dpy);
        return ctx;
    }

    XSetWindowAttributes windowAttribs;
    windowAttribs.border_pixel = BlackPixel(dpy, scrId);
    windowAttribs.background_pixel = WhitePixel(dpy, scrId);
    windowAttribs.override_redirect = 1;
    windowAttribs.colormap = XCreateColormap(dpy, RootWindow(dpy, scrId), visual->visual, AllocNone);
    windowAttribs.event_mask = ExposureMask;

    handle = XCreateWindow(dpy, RootWindowOfScreen(scr), 0, 0, conf->window.width, conf->window.height, 0, visual->depth, InputOutput, visual->visual, CWBackPixel | CWColormap | CWBorderPixel | CWEventMask, &windowAttribs);
    if (!handle) {
        fprintf(stderr, "Failed to create X11 window\n");
        XCloseDisplay(dpy);
        return ctx;
    }
    GLXContext context = glXCreateContext(dpy, visual, NULL, GL_TRUE);
    if (!context) {
        fprintf(stderr, "Failed to create GLX context\n");
        XDestroyWindow(dpy, handle);
        XCloseDisplay(dpy);
        return ctx;
    }
    glXMakeCurrent(dpy, handle, context);
    ctx = malloc(sizeof(*ctx));
    be_Window* window = &(ctx->window);
    window->display = dpy;
    window->handle = handle;
    window->glContext = context;
    XSelectInput(dpy, handle, ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask);
    XClearWindow(dpy, handle);
    XMapRaised(dpy, handle);
    XStoreName(dpy, handle, conf->window.title);
#endif
    return ctx;
}

void _poll_events(be_Context* ctx) {
#if defined(_WIN32)
    MSG message;
    if (GetMessage(&message, NULL, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
#else
    XEvent ev;
    be_Window* window = &(ctx->window);
    while (XPending(window->display)) {
        XNextEvent(window->display, &ev);
        be_Event e = {0};
        be_EventCallback fn = NULL;
        switch(ev.type) {
            case ClientMessage: {
                if (ev.xclient.data.l[0] == XInternAtom(window->display, "WM_DELETE_WINDOW", 0)) {
                    e.type = BITE_WINDOW_CLOSE;
                }
            }
            break;
            case DestroyNotify: {
                e.type = BITE_QUIT;
            }
            break;
            case ConfigureNotify: {
                XConfigureEvent xce = ev.xconfigure;
                if ((xce.x != window->x) || (xce.y != window->y)) {
                    window->x = xce.x;
                    window->y = xce.y;
                    e.type = BITE_WINDOW_MOVE;
                    e.window.x = xce.x;
                    e.window.y = xce.y;
                    e.window.handle = window;
                }

                if ((xce.width != window->width) || (xce.height != window->height)) {
                    window->width = xce.width;
                    window->height = xce.height;
                    e.type = BITE_WINDOW_RESIZE;
                    e.window.x = xce.width;
                    e.window.y = xce.height;
                    e.window.handle = window;
                }
            }
            break;
            case KeyPress: {
                e.type = BITE_KEY_PRESSED;
                e.key.keycode = ev.xkey.keycode;
            }
            break;
            case KeyRelease: {
                e.type = BITE_KEY_RELEASED;
                e.key.keycode = ev.xkey.keycode;
            }
            break;
        }
        fn = ctx->callbacks[e.type];
        if (fn) fn(ctx, &e);
    }
#endif
}