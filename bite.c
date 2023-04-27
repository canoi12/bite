#include "bite.h"

#if defined(_WIN32)
    #include <windows.h>
    #ifndef WINDOWS_LEAN_AND_MEAN
        #define WINDOWS_LEAN_AND_MEAN 1
    #endif
    #include <tchar.h>
    #include <gl/GL.h>
    #include <gl/GLU.h>
#else
    #if defined(__EMSCRIPTEN__)
        #include <emscripten.h>
        #include <emscripten/html5.h>
    #else
        #include <unistd.h>
        #include <X11/Xlib.h>
        #include <X11/Xutil.h>
        #include <GL/glx.h>
        #include <dlfcn.h>
    #endif
    #include <GL/gl.h>
    #include <time.h>
#endif

struct be_Window {
    int x, y;
    int width, height;
#if defined(_WIN32)
    HWND handle;
    HDC dev_context;
#elif defined(__EMSCRIPTEN__)
#else
    Display* display;
    Window handle;
    Colormap map;

    GLXFBConfig fbconf;
#endif
};

typedef struct be_Render be_Render;
struct be_Render {
#if defined(_WIN32)
    HGLRC gl_context;
#elif defined(__EMSCRIPTEN__)
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE gl_context;
#else
    GLXContext gl_context;
#endif
    int mode;
};

struct be_Context {
    int running;
    be_Window window;
    be_Render render;
    be_EventCallback callbacks[BITE_EVENTS];
};

static be_Context _context;

static const be_Config _conf = {
    .window = {
        .title = "bitEngine "BITE_VERSION,
        .width = 640, .height = 380,
        .flags = 0
    }
};

#if defined(_WIN32)
typedef HGLRC WINAPI wglCreateContextAttribsARBProc(HDC hdc, HGLRC hShareContext, const int* attribList);
wglCreateContextAttribsARBProc* wglCreateContextAttribsARB;
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

typedef BOOL WINAPI wglChoosePixelFormatARBProc(HDC hdc, const int *piAttribIList,
        const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
wglChoosePixelFormatARBProc *wglChoosePixelFormatARB;

// See https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_pixel_format.txt for all values
#define WGL_DRAW_TO_WINDOW_ARB                    0x2001
#define WGL_ACCELERATION_ARB                      0x2003
#define WGL_SUPPORT_OPENGL_ARB                    0x2010
#define WGL_DOUBLE_BUFFER_ARB                     0x2011
#define WGL_PIXEL_TYPE_ARB                        0x2013
#define WGL_COLOR_BITS_ARB                        0x2014
#define WGL_DEPTH_BITS_ARB                        0x2022
#define WGL_STENCIL_BITS_ARB                      0x2023

#define WGL_FULL_ACCELERATION_ARB                 0x2027
#define WGL_TYPE_RGBA_ARB                         0x202B
#else
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092
#endif

#define DEFINE_GL(ret, name, ...)\
typedef ret name##Proc(__VA_ARGS__);\
static name##Proc* name = 0

#if !defined(__EMSCRIPTEN__)
#define GL_VERSION 0x1F02
#define GL_SHADING_LANGUAGE_VERSION 0x8B8C

/* data types */
#define GL_BYTE           0x1400
#define GL_UNSIGNED_BYTE  0x1401
#define GL_SHORT          0x1402
#define GL_UNSIGNED_SHORT 0x1403
#define GL_INT            0x1404
#define GL_UNSIGNED_INT   0x1405
#define GL_FLOAT          0x1406
#define GL_2_BYTES        0x1407
#define GL_3_BYTES        0x1408
#define GL_4_BYTES        0x1409
#define GL_DOUBLE         0x140A

/* Clear buffer bits */
#define GL_DEPTH_BUFFER_BIT   0x00000100
#define GL_ACCUM_BUFFER_BIT   0x00000200
#define GL_STENCIL_BUFFER_BIT 0x00000400
#define GL_COLOR_BUFFER_BIT   0x00004000

#define GL_RGB         0x1907
#define GL_RGBA        0x1908

/* bgra */
#define GL_BGR  0x80E0
#define GL_BGRA 0x80E1

/* Primitives */
#define GL_POINTS         0x0000
#define GL_LINES          0x0001
#define GL_TRIANGLES      0x0004

#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893

#define GL_STREAM_DRAW  0x88E0
#define GL_STREAM_READ  0x88E1
#define GL_STREAM_COPY  0x88E2
#define GL_STATIC_DRAW  0x88E4
#define GL_STATIC_READ  0x88E5
#define GL_STATIC_COPY  0x88E6
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_DYNAMIC_READ 0x88E9
#define GL_DYNAMIC_COPY 0x88EA

#define GL_TEXTURE_2D 0x0DE1
// #define GL_TEXTURE_MIN_FILTER 0x2800
// #define GL_TEXTURE_MAG_FILTER 0x2801
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803

#define GL_NEAREST 0x2600
#define GL_REPEAT 0x2901
#define GL_CLAMP 0x2900

#define GL_CLAMP_TO_EDGE 0x812F /* 1.2 */
#define GL_CLAMP_TO_BORDER  0x812D /* 1.3 */

// Core
// DEFINE_GL(void, glClearColor, float, float, float, float);
// DEFINE_GL(void, glClear, GLenum);

// Transformation
// DEFINE_GL(void, glViewport, GLint, GLint, GLint, GLint);

// VAO
DEFINE_GL(void, glGenVertexArrays, GLsizei, GLuint*); // glGenVertexArrays
DEFINE_GL(void, glBindVertexArray, GLuint); // glBindVertexArray
DEFINE_GL(void, glDeleteVertexArrays, GLsizei, GLuint*); // glDeleteVertexArrays
DEFINE_GL(void, glVertexAttribPointer, GLuint, GLint, GLuint, GLint, GLint, const void*);
DEFINE_GL(void, glEnableVertexAttribArray, GLuint);
DEFINE_GL(void, glDisableVertexAttribArray, GLuint);

// Buffers
DEFINE_GL(void, glGenBuffers, GLsizei, GLuint*); // glGenBuffers
DEFINE_GL(void, glBindBuffer, GLenum, GLuint); // glBindBuffer
DEFINE_GL(void, glDeleteBuffers, GLsizei, GLuint*); // glDeleteBuffers
DEFINE_GL(void, glBufferData, GLenum, GLsizei, const void*, GLenum); // glBufferData(GLenum target, GLsizeptr size, const void* data, GLenum usage)
DEFINE_GL(void, glBufferSubData, GLenum, GLsizei, GLsizei, const void*); // glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr, size, const void* data)

// DEFINE_GL(void, glDrawArrays, GLenum, GLint, GLsizei); // glDrawArrays

// SHADERS
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31

#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82

DEFINE_GL(GLuint, glCreateShader, GLenum); // glCreateShader
DEFINE_GL(void, glShaderSource, GLuint, GLsizei, const char**, const GLint*);  // glShaderSource
DEFINE_GL(void, glCompileShader, GLuint); // glCompileShader

#define GL_LINK_STATUS 0x8B82
DEFINE_GL(void, glGetShaderiv, GLuint, GLenum, GLint*); // glGetShaderiv
const int t = GL_LINK_STATUS;
DEFINE_GL(void, glGetShaderInfoLog, GLuint, GLsizei, GLsizei*, char*); // glGetShaderInfoLog
DEFINE_GL(void, glDeleteShader, GLuint); // glDeleteShader
DEFINE_GL(GLuint, glCreateProgram, void); // glCreateProgram
DEFINE_GL(void, glAttachShader, GLuint, GLuint); // glAttachShader
DEFINE_GL(void, glLinkProgram, GLuint); // glLinkProgram
DEFINE_GL(void, glGetProgramiv, GLuint, GLenum, GLint*); // glGetProgramiv
DEFINE_GL(void, glGetProgramInfoLog, GLuint, GLsizei, GLsizei*, char*); // glGetProgramInfoLog
DEFINE_GL(void, glUseProgram, GLuint); // glUseProgram

DEFINE_GL(void, glGetActiveUniform, GLuint, GLuint, GLint, GLint*, GLint*, GLint*, char*);
DEFINE_GL(GLint, glGetUniformLocation, GLuint, const char*);

#define DEFINE_GL_UNIFORM(X, T)\
DEFINE_GL(void, glUniform1##X, GLuint, T);\
DEFINE_GL(void, glUniform2##X, GLint, T, T);\
DEFINE_GL(void, glUniform3##X, GLint, T, T, T);\
DEFINE_GL(void, glUniform4##X, GLint, T, T, T, T);\
DEFINE_GL(void, glUniform1##X##v, GLint, GLint, const T*);\
DEFINE_GL(void, glUniform2##X##v, GLint, GLint, const T*);\
DEFINE_GL(void, glUniform3##X##v, GLint, GLint, const T*);\
DEFINE_GL(void, glUniform4##X##v, GLint, GLint, const T*)

// DEFINE_GL_UNIFORM(f, float);
// DEFINE_GL_UNIFORM(i, GLint);

DEFINE_GL(void, glUniformMatrix2fv, GLint, GLint, GLint, const float*);
DEFINE_GL(void, glUniformMatrix3fv, GLint, GLint, GLint, const float*);
DEFINE_GL(void, glUniformMatrix4fv, GLint, GLint, GLint, const float*);

DEFINE_GL(GLint, glGetAttribLocation, GLuint, const char*);
DEFINE_GL(void, glGetActiveAttribe, GLuint, GLuint, GLint, GLint*, GLint*, GLuint*, char*);
DEFINE_GL(void, glBindAttribLocation, GLuint, GLuint, const char*);

#if defined(_WIN32)
    static HMODULE _gl_sym;
#elif defined(__EMSCRIPTEN__)
#else
    static void* _gl_sym;
    #ifndef RTLD_LAZY
        #define RTLD_LAZY 0x00001
    #endif
    #ifndef RTLD_GLOBAL
        #define RTLD_GLOBAL 0x00100
    #endif
#endif

static void* _get_proc(const char* name);
BITE_RESULT _load_gl(void);
void _setup_gl(void);
void _close_gl(void);
#endif

BITE_RESULT _init_context(be_Context* ctx, const be_Config* conf);
BITE_RESULT _init_window(be_Window* window, const be_Config* conf);
BITE_RESULT _init_render(be_Render* render, const be_Window* window, const be_Config* conf);
void _poll_events(be_Context* ctx);


#if defined(_WIN32)
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    be_Context* ctx = (be_Context*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    be_Event e;
    e.type = 0;
    be_EventCallback fn = NULL;
    switch (msg) {
        case WM_CREATE: return 0;
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
        case WM_SYSKEYUP:
        case WM_KEYUP: {
            e.type = BITE_KEY_RELEASED;
            e.key.keycode = (int)wParam;
        }
        break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    
    if (ctx) fn = ctx->callbacks[e.type];
    if (fn) fn(ctx, &e);
    printf("Teste\n");
    return 0;
}
#endif

void bite_simple_triangle(void) {
    glClearColor(0.3f, 0.4f, 0.4f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
    glColor3f(1.f, 0.f, 0.f);
    glVertex2f(0.f, 0.5f);
    glColor3f(0.f, 1.f, 0.f);
    glVertex2f(-0.5f, -0.5f);
    glColor3f(0.f, 0.f, 1.f);
    glVertex2f(0.5f, -0.5f);
    glEnd();
}

be_Config bite_init_config(const char* title, int w, int h) {
    be_Config c = {0};
    title = title != NULL ? title : "bitEngine "BITE_VERSION;
    size_t len = strlen(title);
    memcpy(c.window.title, title, len);
    c.window.width = w;
    c.window.height = h;
    return c;
}

be_Context* bite_create(const be_Config* conf) {
    conf = conf ? conf : &(_conf);
    be_Context* ctx = malloc(sizeof(*ctx));
    if (!ctx) {
        fprintf(stderr, "Failed to alloc memory for context\n");
        return NULL;
    }
    int mag, min;
    // glGetIntegerv(GL_MAJOR_VERSION, &mag);
    // glGetIntegerv(GL_MINOR_VERSION, &min);
    // printf("OpenGL loaded: %d.%d\n", mag, min);
    ctx->running = 1;
    for (int i = 0; i < BITE_EVENTS; i++) {
        ctx->callbacks[i] = NULL;
    }
    if (_init_context(ctx, conf) < 0) {
        free(ctx);
        return NULL;
    }
    const char* ver = glGetString(GL_VERSION);
    printf("OpenGL loaded: %s\n", ver);
    return ctx;
}

void bite_destroy(be_Context* ctx) {
    if (!ctx) return;
    be_Window* window = &(ctx->window);
    be_Render* render = &(ctx->render);
#if defined(_WIN32)
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(render->gl_context);
    ReleaseDC(window->handle, window->dev_context);
    DestroyWindow(window->handle);
#elif defined(__EMSCRIPTEN__)
    emscripten_webgl_destroy_context(window->handle);
#else
    glXDestroyContext(window->display, render->gl_context);
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
    SwapBuffers(window->dev_context);
#elif defined(__EMSCRIPTEN__)
    emscripten_webgl_commit_frame();
#else
    glXSwapBuffers(window->display, window->handle);
#endif
}

/*********************
 * Window
 *********************/
void bite_set_window_width(be_Context* ctx, int width) {

}


int bite_get_window_width(be_Context* ctx) {
    if (!ctx) return -1;
    return ctx->window.width;
}

/*********************
 * Render
 *********************/

static GLuint _create_program(GLuint vert, GLuint frag) {
#if 0
    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);
    
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {}

    return program;
#endif
    return 0;
}

be_Shader* bite_create_shader(const char* vert, const char* frag) {
    be_Shader* shader = NULL;
    return shader;
}

void bite_render_clear_color(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

void bite_render_clear(void) {
    glClear(GL_COLOR_BUFFER_BIT);
}

struct be_Texture {
    GLuint handle;
    int width, height;
    int filter[2];
    int wrap[2];
};

struct be_Framebuffer {
    GLuint handle;
    be_Texture* texture;
};

struct be_Shader {
    GLuint handle;
    int world_uniform;
    int modelview_uniform;
};

void bite_bind_framebuffer(be_Framebuffer* fbo) {
#if 0
    if (!fbo) glBindFramebuffer(GL_FRAMEBUFFER, 0);
    else glBindFramebuffer(GL_FRAMEBUFFER, fbo->handle);
#endif
}

void bite_bind_texture(be_Texture* tex) {
    if (!tex) glBindTexture(GL_TEXTURE_2D, 0);
    else glBindTexture(GL_TEXTURE_2D, tex->handle);
}

void bite_use_shader(be_Shader* shader) {
    if (!shader) glUseProgram(0);
    else glUseProgram(shader->handle);
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

// static void init_opengl_extensions(be_Context* ctx);

#if defined(_WIN32)
static void init_opengl_extensions(void) {
    WNDCLASSA window_class = {
        .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
        .lpfnWndProc = DefWindowProcA,
        .hInstance = GetModuleHandle(0),
        .lpszClassName = "Dummy_WGL_window"
    };

    if (!RegisterClassA(&window_class)) {
        fprintf(stderr, "Failed to register dummy OpenGL window\n");
        exit(EXIT_FAILURE);
    }

    HWND dummy_window = CreateWindowExA(
        0,
        window_class.lpszClassName,
        "Dummy OpenGL Window",
        0,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        0, 0,
        window_class.hInstance,
        0
    );

    if (!dummy_window) {
        fprintf(stderr, "Failed to create dummy OpenGL window\n");
        exit(EXIT_FAILURE);
    }

    HDC dummy_dc = GetDC(dummy_window);

    PIXELFORMATDESCRIPTOR pfd = {
        .nSize = sizeof(pfd),
        .nVersion = 1,
        .iPixelType = PFD_TYPE_RGBA,
        .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        .cColorBits = 32,
        .cAlphaBits = 8,
        .iLayerType = PFD_MAIN_PLANE,
        .cDepthBits = 24,
        .cStencilBits = 8
    };

    int pixel_format = ChoosePixelFormat(dummy_dc, &pfd);
    if (!pixel_format) {
        fprintf(stderr, "Failed to find a suitable format\n");
        exit(EXIT_FAILURE);
    }
    if (!SetPixelFormat(dummy_dc, pixel_format, &pfd)) {
        fprintf(stderr, "Failed to set the pixel format\n");
        exit(EXIT_FAILURE);
    }
    
    HGLRC dummy_context = wglCreateContext(dummy_dc);
    if (!dummy_context) {
        fprintf(stderr, "Failed to create dummy OpenGL context\n");
        exit(EXIT_FAILURE);
    }

    if (!wglMakeCurrent(dummy_dc, dummy_context)) {
        fprintf(stderr, "Failed to activate dummy OpenGL context\n");
        exit(EXIT_FAILURE);
    }
    wglCreateContextAttribsARB = (wglCreateContextAttribsARBProc*)wglGetProcAddress("wglCreateContextAttribsARB");
    wglChoosePixelFormatARB = (wglChoosePixelFormatARBProc*)wglGetProcAddress("wglChoosePixelFormatARB");

    printf("functions: %p %p\n", wglCreateContextAttribsARB, wglChoosePixelFormatARB);

    wglMakeCurrent(dummy_dc, 0);
    wglDeleteContext(dummy_context);
    ReleaseDC(dummy_window, dummy_dc);
    DestroyWindow(dummy_window);
}
#else
static BITE_BOOL is_extension_supported(const char *extList, const char *extension) {
  const char *start;
  const char *where, *terminator;
  
  /* Extension names should not have spaces. */
  where = strchr(extension, ' ');
  if (where || *extension == '\0')
    return BITE_FALSE;

  /* It takes a bit of care to be fool-proof about parsing the
     OpenGL extensions string. Don't be fooled by sub-strings,
     etc. */
  for (start=extList;;) {
    where = strstr(start, extension);

    if (!where)
      break;

    terminator = where + strlen(extension);

    if ( where == start || *(where - 1) == ' ' )
      if ( *terminator == ' ' || *terminator == '\0' )
        return BITE_TRUE;

    start = terminator;
  }
}
#endif

BITE_RESULT _init_context(be_Context* ctx, const be_Config* conf) {
    if (_init_window(&(ctx->window), conf) != BITE_OK) return BITE_ERROR;
    if (_init_render(&(ctx->render), &(ctx->window), conf) != BITE_OK) return BITE_ERROR;
    return BITE_OK;
}

BITE_RESULT _init_window(be_Window* window, const be_Config* conf) {
#if defined(_WIN32)
    HWND handle;
    HMODULE hInstance = GetModuleHandle(0);

    const TCHAR windowClass[] = _T("bitEngine");

    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = windowClass;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL,
            _T("Call to RegisterClassEx failed"),
            _T("bitEngine"),
            NULL);
        return BITE_ERROR;
    }

    handle = CreateWindow(
        windowClass,
        conf->window.title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        conf->window.width, conf->window.height,
        NULL, NULL, hInstance, NULL
    );
    if (!handle) {
        MessageBox(NULL,
            _T("Class to CreateWindow failed"),
            _T("bitEngine"),
            NULL
        );
        return BITE_ERROR;
    }
    SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)window);
    window->handle = handle;
    window->dev_context = GetDC(handle);

    ShowWindow(handle, SW_SHOWDEFAULT);
    UpdateWindow(handle);
#elif (__EMSCRIPTEN__)
#else
    Display* dpy;
    Window handle;
    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
        fprintf(stderr, "Could not open X11 display\n");
        return BITE_ERROR;
    }
    int scrId = DefaultScreen(dpy);

    static int visual_attribs[] =
    {
      GLX_X_RENDERABLE    , True,
      GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
      GLX_RENDER_TYPE     , GLX_RGBA_BIT,
      GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
      GLX_RED_SIZE        , 8,
      GLX_GREEN_SIZE      , 8,
      GLX_BLUE_SIZE       , 8,
      GLX_ALPHA_SIZE      , 8,
      GLX_DEPTH_SIZE      , 24,
      GLX_STENCIL_SIZE    , 8,
      GLX_DOUBLEBUFFER    , True,
      //GLX_SAMPLE_BUFFERS  , 1,
      //GLX_SAMPLES         , 4,
      None
    };

    GLint major, minor;
    glXQueryVersion(dpy, &major, &minor);
    if ((major == 1 && minor < 3) || (major < 1)) {
        fprintf(stderr, "Invalid GLX version\n");
        XCloseDisplay(dpy);
        return BITE_ERROR;
    }

    int fbcount;
    GLXFBConfig* fbc = glXChooseFBConfig(dpy, scrId, visual_attribs, &fbcount);
    if (!fbc) {
        fprintf(stderr, "Failed to retrieve a framebuffer config\n");
        return BITE_ERROR;
    }

    int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

    int i;
    for (i=0; i<fbcount; ++i) {
        XVisualInfo *vi = glXGetVisualFromFBConfig(dpy, fbc[i]);
        if ( vi )
        {
            int samp_buf, samples;
            glXGetFBConfigAttrib(dpy, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
            glXGetFBConfigAttrib(dpy, fbc[i], GLX_SAMPLES, &samples);
            
            // printf( "  Matching fbconfig %d, visual ID 0x%2x: SAMPLE_BUFFERS = %d,"
            //         " SAMPLES = %d\n", 
            //         i, vi -> visualid, samp_buf, samples );

            if ( best_fbc < 0 || samp_buf && samples > best_num_samp )
            best_fbc = i, best_num_samp = samples;
            if ( worst_fbc < 0 || !samp_buf || samples < worst_num_samp )
            worst_fbc = i, worst_num_samp = samples;
        }
        XFree( vi );
    }
    GLXFBConfig bestFbc = fbc[best_fbc];
    XFree(fbc);

    XVisualInfo* vi = glXGetVisualFromFBConfig(dpy, bestFbc);
    XSetWindowAttributes swa;
    Colormap map;
    swa.colormap = XCreateColormap(dpy, RootWindow(dpy, vi->screen), vi->visual, AllocNone);
    swa.background_pixmap = None;
    swa.border_pixel = 0;
    swa.event_mask = ExposureMask | StructureNotifyMask;

    handle = XCreateWindow(
        dpy, RootWindow(dpy, vi->screen),
        0, 0, conf->window.width, conf->window.height,
        0, vi->depth, InputOutput,
        vi->visual,
        CWBorderPixel | CWColormap | CWEventMask, &swa
    );

    if (!handle) {
        fprintf(stderr, "Failed to create X11 window\n");
        return BITE_ERROR;
    }

    XFree(vi);
    XStoreName(dpy, handle, conf->window.title);
    XMapWindow(dpy, handle);

    window->handle = handle;
    window->display = dpy;
    window->map = swa.colormap;
    window->fbconf = bestFbc;
#endif
    window->width = conf->window.width;
    window->height = conf->window.height;
    return BITE_OK;
}

BITE_RESULT _init_render(be_Render* render, const be_Window* window, const be_Config* conf) {
#if defined(_WIN32)
    init_opengl_extensions();
    HDC hdc = window->dev_context;
    int pixel_format_attribs[] = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
        0, 0
    };

    int pixel_format;
    UINT num_formats;
    wglChoosePixelFormatARB(hdc, pixel_format_attribs, 0, 1, &pixel_format, &num_formats);
    if (!num_formats) {
        MessageBox(
            NULL,
            _T("Failed to set Modern OpenGL pixel format"),
            _T("bitEngine"),
            NULL
        );
        DestroyWindow(window->handle);
        return BITE_ERROR;
    }

    PIXELFORMATDESCRIPTOR pfd;
    DescribePixelFormat(hdc, pixel_format, sizeof(pfd), &pfd);
    if (!SetPixelFormat(hdc, pixel_format, &pfd)) {
        MessageBox(
            NULL,
            _T("Failed to set Modern OpenGL pixel format\n"),
            _T("bitEngine"),
            NULL
        );
        DestroyWindow(window->handle);
        return BITE_ERROR;
    }

    int gl_attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 2,
        WGL_CONTEXT_MINOR_VERSION_ARB, 1,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
        0, 0
    };

    HGLRC hglrc = wglCreateContextAttribsARB(hdc, 0, gl_attribs);
    if (!hglrc) {
        MessageBox(
            NULL,
            _T("Failed to create Modern OpenGL context"),
            _T("bitEngine"),
            NULL
        );
        DestroyWindow(window->handle);
        return BITE_ERROR;
    }
    wglMakeCurrent(hdc, hglrc);

    if (_load_gl() != BITE_OK) {
        MessageBox(
            NULL,
            _T("Failed to init OpenGL loader"),
            _T("bitEngine"),
            NULL
        );
        DestroyWindow(window->handle);
        return BITE_ERROR;
    }
    _setup_gl();
    _close_gl();
#elif defined(__EMSCRIPTEN__)
    EmscriptenWebGLContextAttributes attr;
    emscripten_webgl_init_context_attributes(&attr);
    attr.alpha = 0;
    ctx->window.handle = emscripten_webgl_create_context("#canvas", &attr);
    emscripten_webgl_make_context_current(window->handle);
#else
    Display* dpy = window->display;
    int scrId = DefaultScreen(dpy);
    const char* glxExts = glXQueryExtensionsString(dpy, scrId);
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");

    GLXContext ctx = 0;

    if (!glXCreateContextAttribsARB) {
        fprintf(stderr, "Failed to load glXCreateContextAttribsARB(), loading legacy OpenGL context\n");
        ctx = glXCreateNewContext(dpy, window->fbconf, GLX_RGBA_TYPE, 0, True);
    } else {
        int context_attribs[] = {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 2,
            GLX_CONTEXT_MINOR_VERSION_ARB, 1,
            GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
            None
        };

        ctx = glXCreateContextAttribsARB(dpy, window->fbconf, 0, True, context_attribs);
        XSync(dpy, False);
        if (!ctx) {
            fprintf(stderr, "Failed to create 3.0 OpenGL context\n");
            XDestroyWindow(dpy, window->handle);
            XFreeColormap(dpy, window->map);
            XCloseDisplay(dpy);
            return BITE_ERROR;
        }
        glXMakeCurrent(dpy, window->handle, ctx);
        render->gl_context = ctx;
        if (_load_gl() != BITE_OK) {
            fprintf(stderr, "Failed to init OpenGL loader\n");
            XDestroyWindow(dpy, window->handle);
            XFreeColormap(dpy, window->map);
            XCloseDisplay(dpy);
            return BITE_ERROR;
        }
    _setup_gl();
    _close_gl();
    }
#endif
    return BITE_OK;
}

#if 0
int _init_context(be_Context* ctx, const be_Config* conf) {
#if defined(_WIN32)
    HWND handle;
    HMODULE hInstance = GetModuleHandle(0);
    
    const TCHAR windowClass[] = _T("bitEngine");

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
        return -1;
    }

    handle = CreateWindow(
        windowClass,
        conf->window.title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        conf->window.width, conf->window.height,
        NULL, NULL, hInstance, ctx
    );
    if (!handle) {
        MessageBox(NULL,
            _T("Call to CreateWindow failed"),
            _T("bit Engine"),
            NULL);
        fprintf(stderr, "Error code: %d\n", GetLastError());
        return -1;
    }
    SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)ctx);

    HDC hdc = GetDC(handle);

    init_opengl_extensions();

    int pixel_format_attribs[] = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
        0
    };

    int pixel_format;
    UINT num_formats;
    wglChoosePixelFormatARB(hdc, pixel_format_attribs, 0, 1, &pixel_format, &num_formats);
    if (!num_formats) {
        fprintf(stderr, "Failed to set Modern OpenGL pixel format\n");
        DestroyWindow(handle);
        return -1;
    }

    PIXELFORMATDESCRIPTOR pfd;
    DescribePixelFormat(hdc, pixel_format, sizeof(pfd), &pfd);
    if (!SetPixelFormat(hdc, pixel_format, &pfd)) {
        fprintf(stderr, "Failed to set Modern OpenGL pixel format\n");
        DestroyWindow(handle);
        return -1;
    }

    int gl_attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 2,
        WGL_CONTEXT_MINOR_VERSION_ARB, 1,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    HGLRC hglrc = wglCreateContextAttribsARB(hdc, 0, gl_attribs);
    if (!hglrc) {
        fprintf(stderr, "Failed to create Modern OpenGL context\n");
        DestroyWindow(handle);
        return -1;
    }
    wglMakeCurrent(hdc, hglrc);

    // ctx = malloc(sizeof(*ctx));
    be_Window* window = &(ctx->window);
    window->handle = handle;
    window->dev_context = hdc;

    ShowWindow(handle, SW_SHOWDEFAULT);
    UpdateWindow(handle);
#elif defined(__EMSCRIPTEN__)
    be_Window* window = &(ctx->window);
    EmscriptenWebGLContextAttributes attr;
    emscripten_webgl_init_context_attributes(&attr);
    attr.alpha = 0;
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
        return -1;
    }
    scr = DefaultScreenOfDisplay(dpy);
    scrId = DefaultScreen(dpy);
    
    GLint majorGLX, minorGLX;
    glXQueryVersion(dpy, &majorGLX, &minorGLX);
    if (majorGLX <= 1 && minorGLX < 2) {
        fprintf(stderr, "GLX 1.2 or greater is required\n");
        XCloseDisplay(dpy);
        return -1;
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
        return -1;
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
        return -1;
    }
    GLXContext context = glXCreateContext(dpy, visual, NULL, GL_TRUE);
    if (!context) {
        fprintf(stderr, "Failed to create GLX context\n");
        XDestroyWindow(dpy, handle);
        XCloseDisplay(dpy);
        return -1;
    }
    glXMakeCurrent(dpy, handle, context);
    be_Window* window = &(ctx->window);
    window->display = dpy;
    window->handle = handle;
    window->glContext = context;
    XSelectInput(dpy, handle, ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask);
    XClearWindow(dpy, handle);
    XMapRaised(dpy, handle);
    XStoreName(dpy, handle, conf->window.title);
#endif

    if (_init_gl(ctx) != BITE_OK) {
        fprintf(stderr, "Failed to init OpenGL loader\n");
        return NULL;
    }
    _setup_gl();
    _close_gl();


    ctx->window.width = conf->window.width;
    ctx->window.height = conf->window.height;
    return BITE_OK;
}
#endif
void _poll_events(be_Context* ctx) {
#if defined(_WIN32)
    MSG message;
    if (GetMessage(&message, NULL, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
#elif defined(__EMSCRIPTEN__)
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

// OpenGL loader

#if !defined(__APPLE__) && !defined(__HAIKU__)
void* (*_proc_address)(const char*);
#endif

#define GET_GL_PROC(name)\
name = (name##Proc*)_get_proc(#name)

void* _get_proc(const char*);

BITE_RESULT _load_gl(void) {
#if defined(_WIN32)
    _gl_sym = LoadLibrary("opengl32.dll");
    if (_gl_sym == NULL) {
        fprintf(stderr, "Failed to load OpenGL32.dll\n");
        return BITE_ERROR;
    }
#else
    #if defined(__APPLE__)
        const char *names[] = {
            "../Frameworks/OpenGL.framework/OpenGL",
            "/Library/Frameworks/OpenGL.framework/Opengl",
            "/System/Library/Frameworks/OpenGL.framework/OpenGL",
            "/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL",
            NULL,
        };
    #else
        const char *names[] = {
            "libGL.so.1",
            "libGL.so",
            NULL,
        };
    #endif
        int index;
        for (index = 0; names[index] != NULL; ++index) {
            _gl_sym = dlopen(names[index], RTLD_LAZY | RTLD_GLOBAL);
            if (_gl_sym != NULL) {
    #if defined(__APPLE__) || defined(__HAIKU__)
                return BITE_OK;
    #else
                _proc_address = (void*(*)(const char*))dlsym(_gl_sym, "glXGetProcAddress");
                return _proc_address != NULL ? BITE_OK : BITE_ERROR;
    #endif
                break;
            }
        }
#endif
    return BITE_OK;
}

void _close_gl(void) {
    if (_gl_sym != NULL) {
#if defined(_WIN32)
        FreeLibrary(_gl_sym);
#elif defined(__EMSCRIPTEN_)
#else
        dlclose(_gl_sym);
#endif
        _gl_sym = NULL;
    }
}

void _setup_gl(void) {
    const char* version = glGetString(GL_VERSION);
    const char* glsl = glGetString(GL_SHADING_LANGUAGE_VERSION);
    if (!version) {
        fprintf(stderr, "Failed to get OpenGL version\n");
    }
    const char* prefixes[] = {
        "OpenGL ES-CM ",
        "OpenGL ES-CL ",
        "OpenGL ES ",
        NULL
    };

    if (version) {
        const char* ver = version;
        for (int i = 0; prefixes[i] != NULL; i++) {
            if (strncmp(ver, prefixes[i], strlen(prefixes[i])) == 0) {
                ver += strlen(prefixes[i]);
                fprintf(stdout, "Using OpenGL ES\n");
            }
        }
    }
    
    fprintf(stdout, "OpenGL: %s\n", version);

    GET_GL_PROC(glGenVertexArrays);
    GET_GL_PROC(glBindVertexArray);
    GET_GL_PROC(glDeleteVertexArrays);

    GET_GL_PROC(glVertexAttribPointer);
    GET_GL_PROC(glEnableVertexAttribArray);
    GET_GL_PROC(glDisableVertexAttribArray);

    GET_GL_PROC(glGenBuffers);
    GET_GL_PROC(glBindBuffer);
    GET_GL_PROC(glDeleteBuffers);
    GET_GL_PROC(glBufferData);
    GET_GL_PROC(glBufferSubData);

    GET_GL_PROC(glCreateShader);
    GET_GL_PROC(glShaderSource);
    GET_GL_PROC(glCompileShader);
    GET_GL_PROC(glGetShaderiv);
    GET_GL_PROC(glGetShaderInfoLog);
    GET_GL_PROC(glCreateProgram);
    GET_GL_PROC(glAttachShader);
    GET_GL_PROC(glLinkProgram);
    GET_GL_PROC(glGetProgramiv);
    GET_GL_PROC(glGetProgramInfoLog);

    fprintf(stdout, "glCreateProgram: %p\n", glCreateProgram);
}

BITE_BOOL _load_procs(void) {
    // GET_GL_PROC(glClear);
    // GET_GL_PROC(glClearColor);
    // GET_GL_PROC(glViewport);

    

    // GET_GL_PROC(glDrawArrays);

    return BITE_TRUE;
}

static inline void* _get_proc(const char* name) {
    void* sym = NULL;
    if (_gl_sym == NULL) return sym;
#if !defined(__APPLE__) && !defined(__HAIKU__)
    if (_proc_address != NULL) sym = _proc_address(name);
#endif
    if (sym == NULL) {
#if defined(_WIN32) || defined(__CYGWWIN__)
        sym = (void*)GetProcAddress(_gl_sym, name);
#else
        sym = (void*)dlsym(_gl_sym, name);
#endif
    }
    return sym;
}