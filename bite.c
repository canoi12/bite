#include "bite.h"

int bite_init(int flags) {
    return 0;
}

void bite_quit() {}

#if defined(_WIN32)
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CLOSE: DestroyWindow(hwnd); break;
        case WM_DESTROY: PostQuitMessage(0); break;
        default: return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
#else
#endif

be_Window* bite_create_window(const char* title, int w, int h, int flags) {
    be_Window* window = NULL;
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

    window = malloc(sizeof(*window));
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
    window = malloc(sizeof(*window));
    window->handle = emscripten_webgl_create_context("#canvas", &attr);
    emscripten_webgl_make_context_current(window->handle);
#else
    Display* dpy;
    Window handle;
    Screen* scr;
    int scrId;
    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
        fprintf(stderr, "Could not open X11 display\n");
        return window;
    }
    scr = DefaultScreenOfDisplay(dpy);
    scrId = DefaultScreen(dpy);
    
    GLint majorGLX, minorGLX;
    glXQueryVersion(dpy, &majorGLX, &minorGLX);
    if (majorGLX <= 1 && minorGLX < 2) {
        fprintf(stderr, "GLX 1.2 or greater is required\n");
        XCloseDisplay(dpy);
        return window;
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
        return window;
    }

    XSetWindowAttributes windowAttribs;
    windowAttribs.border_pixel = BlackPixel(dpy, scrId);
    windowAttribs.background_pixel = WhitePixel(dpy, scrId);
    windowAttribs.override_redirect = 1;
    windowAttribs.colormap = XCreateColormap(dpy, RootWindow(dpy, scrId), visual->visual, AllocNone);
    windowAttribs.event_mask = ExposureMask;

    handle = XCreateWindow(dpy, RootWindowOfScreen(scr), 0, 0, w, h, 0, visual->depth, InputOutput, visual->visual, CWBackPixel | CWColormap | CWBorderPixel | CWEventMask, &windowAttribs);
    if (!handle) {
        fprintf(stderr, "Failed to create X11 window\n");
        XCloseDisplay(dpy);
        return window;
    }
    GLXContext context = glXCreateContext(dpy, visual, NULL, GL_TRUE);
    if (!context) {
        fprintf(stderr, "Failed to create GLX context\n");
        XDestroyWindow(dpy, handle);
        XCloseDisplay(dpy);
        return window;
    }
    glXMakeCurrent(dpy, handle, context);
    window = malloc(sizeof(*window));
    window->display = dpy;
    window->handle = handle;
    window->glContext = context;
    XClearWindow(dpy, handle);
    XMapRaised(dpy, handle);
#endif
    return window;
}

void bite_destroy_window(be_Window* window) {
    if (!window) return;
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
    free(window);
}

void bite_window_swap(be_Window* window) {
#if defined(_WIN32)
    SwapBuffers(window->devContext);
#elif defined(__EMSCRIPTEN__)
    emscripten_webgl_commit_frame();
#else
    glXSwapBuffers(window->display, window->handle);
#endif
}

void bite_timer_sleep(be_u64 ms) {
#if defined(_WIN32)
    Sleep(ms);
#elif defined(__EMSCRIPTEN__)
    emscripten_sleep((be_u32)ms);
#else
    sleep(ms / 1000);
#endif
}