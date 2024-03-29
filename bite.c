#include "bite.h"

#if defined(_WIN32)
#include <windows.h>
#ifndef WINDOWS_LEAN_AND_MEAN
#define WINDOWS_LEAN_AND_MEAN 1
#endif
#include <gl/GL.h>
#include <gl/GLU.h>
#include <tchar.h>
#elif defined(__EMSCRIPTEN__)
#include <GLES2/gl2.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <time.h>
#else
#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <dlfcn.h>
#include <time.h>
#include <unistd.h>
#endif

#if defined(_WIN32)
char keys[512] = {
  [VK_BACK] = BITEK_BACKSPACE,
  [VK_TAB] = BITEK_TAB,
  [VK_RETURN] = BITEK_RETURN,
  [VK_SHIFT] = BITEK_SHIFT,
  [VK_CONTROL] = BITEK_CONTROL,

  [VK_ESCAPE] = BITEK_ESCAPE,
  [VK_SPACE] = BITEK_SPACE,
  [VK_PRIOR] = BITEK_PAGEUP,
  [VK_NEXT] = BITEK_PAGEDOWN,

  [VK_LEFT] = BITEK_LEFT,
  [VK_UP] = BITEK_UP,
  [VK_RIGHT] = BITEK_RIGHT,
  [VK_DOWN] = BITEK_DOWN,

  [VK_END] = BITEK_END,
  [VK_HOME] = BITEK_HOME,
  [VK_INSERT] = BITEK_INSERT,
  [VK_DELETE] = BITEK_DELETE,
  
  [0x30] = BITEK_0,
  [0x31] = BITEK_1,
  [0x32] = BITEK_2,
  [0x33] = BITEK_3,
  [0x34] = BITEK_4,
  [0x35] = BITEK_5,
  [0x36] = BITEK_6,
  [0x37] = BITEK_7,
  [0x38] = BITEK_8,
  [0x39] = BITEK_9,

  [0x41] = BITEK_A,
  [0x42] = BITEK_B,
  [0x43] = BITEK_C,
  [0x44] = BITEK_D,
  [0x45] = BITEK_E,
  [0x46] = BITEK_F,
  [0x47] = BITEK_G,
  [0x48] = BITEK_H,
  [0x49] = BITEK_I,
  [0x4A] = BITEK_J,
  [0x4B] = BITEK_K,
  [0x4C] = BITEK_L,
  [0x4D] = BITEK_M,
  [0x4E] = BITEK_N,
  [0x4F] = BITEK_O,
  [0x50] = BITEK_P,
  [0x51] = BITEK_Q,
  [0x52] = BITEK_R,
  [0x53] = BITEK_S,
  [0x54] = BITEK_T,
  [0x55] = BITEK_U,
  [0x56] = BITEK_V,
  [0x57] = BITEK_W,
  [0x58] = BITEK_X,
  [0x59] = BITEK_Y,
  [0x5A] = BITEK_Z,
};
#else
char keys[] = {
  [XK_BackSpace] = BITEK_BACKSPACE,
  [XK_Tab] = BITEK_TAB,
  [XK_Return] = BITEK_RETURN,
  [XK_Escape] = BITEK_ESCAPE,
  [XK_Delete] = BITEK_DELETE,
  [XK_space] = BITEK_SPACE,
  [XK_Alt_L] = BITEK_LALT,
  [XK_Alt_R] = BITEK_RALT,
  [XK_Shift_L] = BITEK_LSHIFT,
  [XK_Shift_R] = BITEK_RSHIFT,
  [XK_Control_L] = BITEK_LCONTROL,
  [XK_Control_R] = BITEK_RCONTROL,
  [XK_Super_L] = BITEK_LSUPER,
  [XK_Super_R] = BITEK_RSUPER,

  [XK_Left] = BITEK_LEFT,
  [XK_Up] = BITEK_UP,
  [XK_Right] = BITEK_RIGHT,
  [XK_Down] = BITEK_DOWN,

  [XK_Page_Up] = BITEK_PAGEUP,
  [XK_Page_Down] = BITEK_PAGEDOWN,

  [XK_End] = BITEK_END,
  [XK_Home] = BITEK_HOME,

  [XK_0] = BITEK_0,
  [XK_1] = BITEK_1,
  [XK_2] = BITEK_2,
  [XK_3] = BITEK_3,
  [XK_4] = BITEK_4,
  [XK_5] = BITEK_5,
  [XK_6] = BITEK_6,
  [XK_7] = BITEK_7,
  [XK_8] = BITEK_8,
  [XK_9] = BITEK_9,

  [XK_A] = BITEK_A,
  [XK_B] = BITEK_B,
  [XK_C] = BITEK_C,
  [XK_D] = BITEK_D,
  [XK_E] = BITEK_E,
  [XK_F] = BITEK_F,
  [XK_G] = BITEK_G,
  [XK_H] = BITEK_H,
  [XK_I] = BITEK_I,
  [XK_J] = BITEK_J,
  [XK_K] = BITEK_K,
  [XK_L] = BITEK_L,
  [XK_M] = BITEK_M,
  [XK_N] = BITEK_N,
  [XK_O] = BITEK_O,
  [XK_P] = BITEK_P,
  [XK_Q] = BITEK_Q,
  [XK_R] = BITEK_R,
  [XK_S] = BITEK_S,
  [XK_T] = BITEK_T,
  [XK_U] = BITEK_U,
  [XK_V] = BITEK_V,
  [XK_W] = BITEK_W,
  [XK_X] = BITEK_X,
  [XK_Y] = BITEK_Y,
  [XK_Z] = BITEK_Z,

  [XK_a] = BITEK_A,
  [XK_b] = BITEK_B,
  [XK_c] = BITEK_C,
  [XK_d] = BITEK_D,
  [XK_e] = BITEK_E,
  [XK_f] = BITEK_F,
  [XK_g] = BITEK_G,
  [XK_h] = BITEK_H,
  [XK_i] = BITEK_I,
  [XK_j] = BITEK_J,
  [XK_k] = BITEK_K,
  [XK_l] = BITEK_L,
  [XK_m] = BITEK_M,
  [XK_n] = BITEK_N,
  [XK_o] = BITEK_O,
  [XK_p] = BITEK_P,
  [XK_q] = BITEK_Q,
  [XK_r] = BITEK_R,
  [XK_s] = BITEK_S,
  [XK_t] = BITEK_T,
  [XK_u] = BITEK_U,
  [XK_v] = BITEK_V,
  [XK_w] = BITEK_W,
  [XK_x] = BITEK_X,
  [XK_y] = BITEK_Y,
  [XK_z] = BITEK_Z,

  [XK_KP_0] = BITEK_NUMPAD0,
  [XK_KP_1] = BITEK_NUMPAD1,
  [XK_KP_2] = BITEK_NUMPAD2,
  [XK_KP_3] = BITEK_NUMPAD3,
  [XK_KP_4] = BITEK_NUMPAD4,
  [XK_KP_5] = BITEK_NUMPAD5,
  [XK_KP_6] = BITEK_NUMPAD6,
  [XK_KP_7] = BITEK_NUMPAD7,
  [XK_KP_8] = BITEK_NUMPAD8,
  [XK_KP_9] = BITEK_NUMPAD9,

  [XK_F1] = BITEK_F1,
  [XK_F2] = BITEK_F2,
  [XK_F3] = BITEK_F3,
  [XK_F4] = BITEK_F4,
  [XK_F5] = BITEK_F5,
  [XK_F6] = BITEK_F6,
  [XK_F7] = BITEK_F7,
  [XK_F8] = BITEK_F8,
  [XK_F9] = BITEK_F9,
  [XK_F10] = BITEK_F10,
  [XK_F11] = BITEK_F11,
  [XK_F12] = BITEK_F12,

  [XK_minus] = BITEK_MINUS,
  [XK_plus] = BITEK_PLUS,
  [XK_less] = BITEK_LESS,
  [XK_equal] = BITEK_EQUAL,
  [XK_greater] = BITEK_GREATER,
};
#endif

struct be_Shader {
  GLuint handle;
  GLint world_uniform, modelview_uniform;
};

struct be_Texture {
  GLuint handle;
  int width, height;
  int filter[2];
  int wrap[2];
};

struct be_Framebuffer {
  GLuint handle;
  be_Texture texture;
};

struct be_Window {
  int x, y;
  int width, height;
#if defined(_WIN32)
  HWND handle;
  HDC dev_context;
#elif defined(__EMSCRIPTEN__)
#else
  Display *display;
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
  GLuint vao, vbo;
};

struct be_Context {
  int running;
  be_Window window;
  be_Render render;
  be_EventCallback callbacks[BITE_EVENTS];
};

static be_Context _context;

static const be_Config _conf = {.window = {.title = "bitEngine " BITE_VERSION,
                                           .width = 640,
                                           .height = 380,
                                           .flags = 0}};

#if defined(_WIN32)
typedef HGLRC WINAPI wglCreateContextAttribsARBProc(HDC hdc,
                                                    HGLRC hShareContext,
                                                    const int *attribList);
wglCreateContextAttribsARBProc *wglCreateContextAttribsARB;
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

typedef BOOL WINAPI wglChoosePixelFormatARBProc(
    HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList,
    UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
wglChoosePixelFormatARBProc *wglChoosePixelFormatARB;

// See
// https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_pixel_format.txt
// for all values
#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_ACCELERATION_ARB 0x2003
#define WGL_SUPPORT_OPENGL_ARB 0x2010
#define WGL_DOUBLE_BUFFER_ARB 0x2011
#define WGL_PIXEL_TYPE_ARB 0x2013
#define WGL_COLOR_BITS_ARB 0x2014
#define WGL_DEPTH_BITS_ARB 0x2022
#define WGL_STENCIL_BITS_ARB 0x2023

#define WGL_FULL_ACCELERATION_ARB 0x2027
#define WGL_TYPE_RGBA_ARB 0x202B
#elif defined(__EMSCRIPTEN__)
#else
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display *, GLXFBConfig,
                                                     GLXContext, Bool,
                                                     const int *);
#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092
#endif

#define DEFINE_GL(ret, name, ...)                                              \
  typedef ret name##Proc(__VA_ARGS__);                                         \
  static name##Proc *name = 0

#if !defined(__EMSCRIPTEN__)
#define GL_VERSION 0x1F02
#define GL_SHADING_LANGUAGE_VERSION 0x8B8C

/* data types */
#define GL_BYTE 0x1400
#define GL_UNSIGNED_BYTE 0x1401
#define GL_SHORT 0x1402
#define GL_UNSIGNED_SHORT 0x1403
#define GL_INT 0x1404
#define GL_UNSIGNED_INT 0x1405
#define GL_FLOAT 0x1406
#define GL_2_BYTES 0x1407
#define GL_3_BYTES 0x1408
#define GL_4_BYTES 0x1409
#define GL_DOUBLE 0x140A

/* Clear buffer bits */
#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_ACCUM_BUFFER_BIT 0x00000200
#define GL_STENCIL_BUFFER_BIT 0x00000400
#define GL_COLOR_BUFFER_BIT 0x00004000

#define GL_RGB 0x1907
#define GL_RGBA 0x1908

/* bgra */
#define GL_BGR 0x80E0
#define GL_BGRA 0x80E1

/* Primitives */
#define GL_POINTS 0x0000
#define GL_LINES 0x0001
#define GL_TRIANGLES 0x0004

#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893

#define GL_STREAM_DRAW 0x88E0
#define GL_STREAM_READ 0x88E1
#define GL_STREAM_COPY 0x88E2
#define GL_STATIC_DRAW 0x88E4
#define GL_STATIC_READ 0x88E5
#define GL_STATIC_COPY 0x88E6
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

#define GL_CLAMP_TO_EDGE 0x812F   /* 1.2 */
#define GL_CLAMP_TO_BORDER 0x812D /* 1.3 */

// Core
// DEFINE_GL(void, glClearColor, float, float, float, float);
// DEFINE_GL(void, glClear, GLenum);

// Transformation
// DEFINE_GL(void, glViewport, GLint, GLint, GLint, GLint);

// VAO
DEFINE_GL(void, glGenVertexArrays, GLsizei, GLuint *); // glGenVertexArrays
DEFINE_GL(void, glBindVertexArray, GLuint);            // glBindVertexArray
DEFINE_GL(void, glDeleteVertexArrays, GLsizei,
          GLuint *); // glDeleteVertexArrays
DEFINE_GL(void, glVertexAttribPointer, GLuint, GLint, GLuint, GLint, GLint,
          const void *);
DEFINE_GL(void, glEnableVertexAttribArray, GLuint);
DEFINE_GL(void, glDisableVertexAttribArray, GLuint);

// Buffers
DEFINE_GL(void, glGenBuffers, GLsizei, GLuint *);    // glGenBuffers
DEFINE_GL(void, glBindBuffer, GLenum, GLuint);       // glBindBuffer
DEFINE_GL(void, glDeleteBuffers, GLsizei, GLuint *); // glDeleteBuffers
DEFINE_GL(void, glBufferData, GLenum, GLsizei, const void *,
          GLenum); // glBufferData(GLenum target, GLsizeptr size, const void*
                   // data, GLenum usage)
DEFINE_GL(void, glBufferSubData, GLenum, GLsizei, GLsizei,
          const void *); // glBufferSubData(GLenum target, GLintptr offset,
                         // GLsizeiptr, size, const void* data)

// DEFINE_GL(void, glDrawArrays, GLenum, GLint, GLsizei); // glDrawArrays

// SHADERS
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31

#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82

DEFINE_GL(GLuint, glCreateShader, GLenum); // glCreateShader
DEFINE_GL(void, glShaderSource, GLuint, GLsizei, const char **,
          const GLint *);                 // glShaderSource
DEFINE_GL(void, glCompileShader, GLuint); // glCompileShader

#define GL_LINK_STATUS 0x8B82
DEFINE_GL(void, glGetShaderiv, GLuint, GLenum, GLint *); // glGetShaderiv
const int t = GL_LINK_STATUS;
DEFINE_GL(void, glGetShaderInfoLog, GLuint, GLsizei, GLsizei *,
          char *);                                        // glGetShaderInfoLog
DEFINE_GL(void, glDeleteShader, GLuint);                  // glDeleteShader
DEFINE_GL(GLuint, glCreateProgram, void);                 // glCreateProgram
DEFINE_GL(void, glAttachShader, GLuint, GLuint);          // glAttachShader
DEFINE_GL(void, glLinkProgram, GLuint);                   // glLinkProgram
DEFINE_GL(void, glGetProgramiv, GLuint, GLenum, GLint *); // glGetProgramiv
DEFINE_GL(void, glGetProgramInfoLog, GLuint, GLsizei, GLsizei *,
          char *);                        // glGetProgramInfoLog
DEFINE_GL(void, glUseProgram, GLuint);    // glUseProgram
DEFINE_GL(void, glDeleteProgram, GLuint); // glUseProgram

DEFINE_GL(void, glGetActiveUniform, GLuint, GLuint, GLint, GLint *, GLint *,
          GLint *, char *);
DEFINE_GL(GLint, glGetUniformLocation, GLuint, const char *);

#define DEFINE_GL_UNIFORM(X, T)                                                \
  DEFINE_GL(void, glUniform1##X, GLuint, T);                                   \
  DEFINE_GL(void, glUniform2##X, GLint, T, T);                                 \
  DEFINE_GL(void, glUniform3##X, GLint, T, T, T);                              \
  DEFINE_GL(void, glUniform4##X, GLint, T, T, T, T);                           \
  DEFINE_GL(void, glUniform1##X##v, GLint, GLint, const T *);                  \
  DEFINE_GL(void, glUniform2##X##v, GLint, GLint, const T *);                  \
  DEFINE_GL(void, glUniform3##X##v, GLint, GLint, const T *);                  \
  DEFINE_GL(void, glUniform4##X##v, GLint, GLint, const T *)

// DEFINE_GL_UNIFORM(f, float);
// DEFINE_GL_UNIFORM(i, GLint);

DEFINE_GL(void, glUniformMatrix2fv, GLint, GLint, GLint, const float *);
DEFINE_GL(void, glUniformMatrix3fv, GLint, GLint, GLint, const float *);
DEFINE_GL(void, glUniformMatrix4fv, GLint, GLint, GLint, const float *);

DEFINE_GL(GLint, glGetAttribLocation, GLuint, const char *);
DEFINE_GL(void, glGetActiveAttribe, GLuint, GLuint, GLint, GLint *, GLint *,
          GLuint *, char *);
DEFINE_GL(void, glBindAttribLocation, GLuint, GLuint, const char *);

#if defined(_WIN32)
static HMODULE _gl_sym;
#elif defined(__EMSCRIPTEN__)
#else
static void *_gl_sym;
#ifndef RTLD_LAZY
#define RTLD_LAZY 0x00001
#endif
#ifndef RTLD_GLOBAL
#define RTLD_GLOBAL 0x00100
#endif
#endif

static void *_get_proc(const char *name);
BITE_RESULT _load_gl(void);
void _setup_gl(void);
void _close_gl(void);
#endif

BITE_RESULT _init_context(be_Context *ctx, const be_Config *conf);
BITE_RESULT _init_window(be_Window *window, const be_Config *conf);
BITE_RESULT _init_render(be_Render *render, const be_Window *window,
                         const be_Config *conf);
void _poll_events(be_Context *ctx);

#if defined(_WIN32)
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  be_Context *ctx = (be_Context *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
  be_Event e;
  e.type = 0;
  be_EventCallback fn = NULL;
  switch (msg) {
    case WM_CREATE: return 0;
    case WM_CLOSE: {
      e.type = BITE_WINDOW_CLOSE;
    } break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    case WM_SYSKEYDOWN:
    case WM_KEYDOWN: {
      e.type = BITE_KEY_PRESSED;
      e.key.keycode = keys[(int)wParam];
    } break;
    case WM_SYSKEYUP:
    case WM_KEYUP: {
      e.type = BITE_KEY_RELEASED;
      e.key.keycode = keys[(int)wParam];
      printf("KeyPressed: %d\n", e.key.keycode);
    } break;
    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }

  if (ctx)
    fn = ctx->callbacks[e.type];
  if (fn)
    fn(ctx, &e);
  printf("Teste\n");
  return 0;
}
#endif

void bite_simple_triangle(be_Context *ctx) {
  glClearColor(0.3f, 0.4f, 0.4f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);
  // fprintf(stdout, "VAO: %d\n", ctx->render.vao);
#if !defined(__EMSCRIPTEN__)
  glBindVertexArray(ctx->render.vao);
#else
  glBindBuffer(GL_ARRAY_BUFFER, ctx->render.vbo);
#endif
  glDrawArrays(GL_TRIANGLES, 0, 3);
#if !defined(__EMSCRIPTEN__)
  glBindVertexArray(0);
#else
  glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
  // glBegin(GL_TRIANGLES);
  // glColor3f(1.f, 0.f, 0.f);
  // glVertex2f(0.f, 0.5f);
  // glColor3f(0.f, 1.f, 0.f);
  // glVertex2f(-0.5f, -0.5f);
  // glColor3f(0.f, 0.f, 1.f);
  // glVertex2f(0.5f, -0.5f);
  // glEnd();
}

be_Config bite_init_config(const char *title, int w, int h) {
  be_Config c = {0};
  title = title != NULL ? title : "bitEngine " BITE_VERSION;
  size_t len = strlen(title);
  memcpy(c.window.title, title, len);
  c.window.width = w;
  c.window.height = h;
  return c;
}

be_Context *bite_create(const be_Config *conf) {
  conf = conf ? conf : &(_conf);
  be_Context *ctx = malloc(sizeof(*ctx));
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
  const char *ver = (const char*)glGetString(GL_VERSION);
  printf("OpenGL loaded: %s\n", ver);
  return ctx;
}

void bite_destroy(be_Context *ctx) {
  if (!ctx)
    return;
  be_Window *window = &(ctx->window);
  be_Render *render = &(ctx->render);
#if defined(_WIN32)
  wglMakeCurrent(NULL, NULL);
  wglDeleteContext(render->gl_context);
  ReleaseDC(window->handle, window->dev_context);
  DestroyWindow(window->handle);
#elif defined(__EMSCRIPTEN__)
  emscripten_webgl_destroy_context(render->gl_context);
#else
  glXDestroyContext(window->display, render->gl_context);
  XDestroyWindow(window->display, window->handle);
  XCloseDisplay(window->display);
#endif
  free(ctx);
}

void bite_register_callback(be_Context *ctx, int type,
                            be_EventCallback callback) {
  if (!ctx)
    return;
  if (type < BITE_QUIT || type >= BITE_EVENTS)
    return;
  ctx->callbacks[type] = callback;
}

int bite_should_close(be_Context *ctx) {
  if (!ctx)
    return 1;
  return !(ctx->running);
}

void bite_set_should_close(be_Context *ctx, int should_close) {
  if (!ctx)
    return;
  ctx->running = !should_close;
}

void bite_poll_events(be_Context *ctx) {
  if (!ctx)
    return;
  _poll_events(ctx);
}

void bite_swap(be_Context *ctx) {
  if (!ctx)
    return;
  be_Window *window = &(ctx->window);
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
void bite_set_window_width(be_Context *ctx, int width) {}

int bite_get_window_width(be_Context *ctx) {
  if (!ctx)
    return -1;
  return ctx->window.width;
}

/*********************
 * Render
 *********************/

static GLuint _compile_shader(GLenum type, const char *src) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);

  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char info_log[512];
    glGetShaderInfoLog(shader, 512, NULL, info_log);
    fprintf(stderr, "Failed to compile shader: %s\n", info_log);
    glDeleteShader(shader);
    return 0;
  }

  return shader;
}

static GLuint _create_program(GLuint vert, GLuint frag) {
#if 1
  GLuint program = glCreateProgram();
  glAttachShader(program, vert);
  glAttachShader(program, frag);
  glLinkProgram(program);

  int success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    char info_log[512];
    glGetProgramInfoLog(program, 512, NULL, info_log);
    fprintf(stderr, "Failed to link program: %s\n", info_log);
    glDeleteProgram(program);
    return 0;
  }

  return program;
#endif
  return 0;
}

be_Texture *bite_create_texture(int width, int height, int format, void *data) {
  be_Texture *texture = (be_Texture *)malloc(sizeof(*texture));
  if (!texture) {
    fprintf(stderr, "Failed to alloc memory for texture\n");
    return NULL;
  }
  texture->filter[0] = GL_NEAREST;
  texture->filter[1] = GL_NEAREST;
  texture->wrap[0] = GL_CLAMP_TO_EDGE;
  texture->wrap[1] = GL_CLAMP_TO_EDGE;
  texture->width = width;
  texture->height = height;

  glGenTextures(1, &(texture->handle));
  glBindTexture(GL_TEXTURE_2D, texture->handle);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->filter[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->filter[1]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture->wrap[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture->wrap[1]);
  glBindTexture(GL_TEXTURE_2D, 0);
  return texture;
}

be_Shader *bite_create_shader(const char *vert_src, const char *frag_src) {
  be_Shader *shader = NULL;
  GLuint vert, frag;
  vert = _compile_shader(GL_VERTEX_SHADER, vert_src);
  if (vert <= 0)
    return shader;
  frag = _compile_shader(GL_FRAGMENT_SHADER, frag_src);
  if (frag <= 0)
    return shader;

  GLuint program = _create_program(vert, frag);
  if (program <= 0)
    return shader;
  shader = malloc(sizeof(*shader));
  shader->handle = program;

  return shader;
}

void bite_render_clear_color(float r, float g, float b, float a) {
  glClearColor(r, g, b, a);
}

void bite_render_clear(void) { glClear(GL_COLOR_BUFFER_BIT); }

void bite_bind_framebuffer(be_Framebuffer *fbo) {
#if 0
    if (!fbo) glBindFramebuffer(GL_FRAMEBUFFER, 0);
    else glBindFramebuffer(GL_FRAMEBUFFER, fbo->handle);
#endif
}

void bite_bind_texture(be_Texture *tex) {
  if (!tex)
    glBindTexture(GL_TEXTURE_2D, 0);
  else
    glBindTexture(GL_TEXTURE_2D, tex->handle);
}

void bite_use_shader(be_Shader *shader) {
  if (!shader)
    glUseProgram(0);
  else
    glUseProgram(shader->handle);
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
  WNDCLASSA window_class = {.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
                            .lpfnWndProc = DefWindowProcA,
                            .hInstance = GetModuleHandle(0),
                            .lpszClassName = "Dummy_WGL_window"};

  if (!RegisterClassA(&window_class)) {
    fprintf(stderr, "Failed to register dummy OpenGL window\n");
    exit(EXIT_FAILURE);
  }

  HWND dummy_window =
      CreateWindowExA(0, window_class.lpszClassName, "Dummy OpenGL Window", 0,
                      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                      CW_USEDEFAULT, 0, 0, window_class.hInstance, 0);

  if (!dummy_window) {
    fprintf(stderr, "Failed to create dummy OpenGL window\n");
    exit(EXIT_FAILURE);
  }

  HDC dummy_dc = GetDC(dummy_window);

  PIXELFORMATDESCRIPTOR pfd = {.nSize = sizeof(pfd),
                               .nVersion = 1,
                               .iPixelType = PFD_TYPE_RGBA,
                               .dwFlags = PFD_DRAW_TO_WINDOW |
                                          PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
                               .cColorBits = 32,
                               .cAlphaBits = 8,
                               .iLayerType = PFD_MAIN_PLANE,
                               .cDepthBits = 24,
                               .cStencilBits = 8};

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
  wglCreateContextAttribsARB =
      (wglCreateContextAttribsARBProc *)wglGetProcAddress(
          "wglCreateContextAttribsARB");
  wglChoosePixelFormatARB = (wglChoosePixelFormatARBProc *)wglGetProcAddress(
      "wglChoosePixelFormatARB");

  printf("functions: %p %p\n", wglCreateContextAttribsARB,
         wglChoosePixelFormatARB);

  wglMakeCurrent(dummy_dc, 0);
  wglDeleteContext(dummy_context);
  ReleaseDC(dummy_window, dummy_dc);
  DestroyWindow(dummy_window);
}
#else
static BITE_BOOL is_extension_supported(const char *extList,
                                        const char *extension) {
  const char *start;
  const char *where, *terminator;

  /* Extension names should not have spaces. */
  where = strchr(extension, ' ');
  if (where || *extension == '\0')
    return BITE_FALSE;

  /* It takes a bit of care to be fool-proof about parsing the
     OpenGL extensions string. Don't be fooled by sub-strings,
     etc. */
  for (start = extList;;) {
    where = strstr(start, extension);

    if (!where)
      break;

    terminator = where + strlen(extension);

    if (where == start || *(where - 1) == ' ')
      if (*terminator == ' ' || *terminator == '\0')
        return BITE_TRUE;

    start = terminator;
  }
  return BITE_FALSE;
}
#endif

BITE_RESULT _init_context(be_Context *ctx, const be_Config *conf) {
  if (_init_window(&(ctx->window), conf) != BITE_OK)
    return BITE_ERROR;
#if defined(_WIN32)
  SetWindowLongPtr(ctx->window.handle, GWLP_USERDATA, (LONG_PTR)ctx);
#endif
  if (_init_render(&(ctx->render), &(ctx->window), conf) != BITE_OK)
    return BITE_ERROR;
  be_Render *render = &(ctx->render);

  float vertices[] = {0.f, 0.5f, 1.f, 0.f,  0.f,   1.f, -0.5f, -0.5f, 0.f,
                      1.f, 0.f,  1.f, 0.5f, -0.5f, 0.f, 0.f,   1.f,   1.f};

#if !defined(__EMSCRIPTEN__)
  glGenVertexArrays(1, &(render->vao));
  glBindVertexArray(render->vao);
#endif
  glGenBuffers(1, &(render->vbo));
  glBindBuffer(GL_ARRAY_BUFFER, render->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  // glBindBuffer(GL_ARRAY_BUFFER, 0);
#if !defined(__EMSCRIPTEN__)
  glBindVertexArray(0);
#endif
  return BITE_OK;
}

BITE_RESULT _init_window(be_Window *window, const be_Config *conf) {
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
    MessageBox(NULL, _T("Call to RegisterClassEx failed"), _T("bitEngine"),
               NULL);
    return BITE_ERROR;
  }

  handle = CreateWindow(windowClass, conf->window.title, WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT, conf->window.width,
                        conf->window.height, NULL, NULL, hInstance, NULL);
  if (!handle) {
    MessageBox(NULL, _T("Class to CreateWindow failed"), _T("bitEngine"), NULL);
    return BITE_ERROR;
  }
  window->handle = handle;
  window->dev_context = GetDC(handle);

  ShowWindow(handle, SW_SHOWDEFAULT);
  UpdateWindow(handle);
#elif (__EMSCRIPTEN__)
#else
  Display *dpy;
  Window handle;
  dpy = XOpenDisplay(NULL);
  if (dpy == NULL) {
    fprintf(stderr, "Could not open X11 display\n");
    return BITE_ERROR;
  }
  int scrId = DefaultScreen(dpy);

  static int visual_attribs[] = {
      GLX_X_RENDERABLE, True, GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
      GLX_RENDER_TYPE, GLX_RGBA_BIT, GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
      GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8, GLX_BLUE_SIZE, 8, GLX_ALPHA_SIZE, 8,
      GLX_DEPTH_SIZE, 24, GLX_STENCIL_SIZE, 8, GLX_DOUBLEBUFFER, True,
      // GLX_SAMPLE_BUFFERS  , 1,
      // GLX_SAMPLES         , 4,
      None};

  GLint major, minor;
  glXQueryVersion(dpy, &major, &minor);
  if ((major == 1 && minor < 3) || (major < 1)) {
    fprintf(stderr, "Invalid GLX version\n");
    XCloseDisplay(dpy);
    return BITE_ERROR;
  }

  int fbcount;
  GLXFBConfig *fbc = glXChooseFBConfig(dpy, scrId, visual_attribs, &fbcount);
  if (!fbc) {
    fprintf(stderr, "Failed to retrieve a framebuffer config\n");
    return BITE_ERROR;
  }

  int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

  int i;
  for (i = 0; i < fbcount; ++i) {
    XVisualInfo *vi = glXGetVisualFromFBConfig(dpy, fbc[i]);
    if (vi) {
      int samp_buf, samples;
      glXGetFBConfigAttrib(dpy, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
      glXGetFBConfigAttrib(dpy, fbc[i], GLX_SAMPLES, &samples);

      // printf( "  Matching fbconfig %d, visual ID 0x%2x: SAMPLE_BUFFERS = %d,"
      //         " SAMPLES = %d\n",
      //         i, vi -> visualid, samp_buf, samples );

      if (best_fbc < 0 || samp_buf && samples > best_num_samp)
        best_fbc = i, best_num_samp = samples;
      if (worst_fbc < 0 || !samp_buf || samples < worst_num_samp)
        worst_fbc = i, worst_num_samp = samples;
    }
    XFree(vi);
  }
  GLXFBConfig bestFbc = fbc[best_fbc];
  XFree(fbc);

  XVisualInfo *vi = glXGetVisualFromFBConfig(dpy, bestFbc);
  XSetWindowAttributes swa;
  Colormap map;
  swa.colormap =
      XCreateColormap(dpy, RootWindow(dpy, vi->screen), vi->visual, AllocNone);
  swa.background_pixmap = None;
  swa.border_pixel = 0;
  swa.event_mask = ExposureMask | StructureNotifyMask;

  handle =
      XCreateWindow(dpy, RootWindow(dpy, vi->screen), 0, 0, conf->window.width,
                    conf->window.height, 0, vi->depth, InputOutput, vi->visual,
                    CWBorderPixel | CWColormap | CWEventMask, &swa);

  if (!handle) {
    fprintf(stderr, "Failed to create X11 window\n");
    return BITE_ERROR;
  }

  XFree(vi);
  XSelectInput(dpy, handle, ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask);
  XClearWindow(dpy, handle);
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

BITE_RESULT _init_render(be_Render *render, const be_Window *window,
                         const be_Config *conf) {
#if defined(_WIN32)
  init_opengl_extensions();
  HDC hdc = window->dev_context;
  int pixel_format_attribs[] = {WGL_DRAW_TO_WINDOW_ARB,
                                GL_TRUE,
                                WGL_SUPPORT_OPENGL_ARB,
                                GL_TRUE,
                                WGL_DOUBLE_BUFFER_ARB,
                                GL_TRUE,
                                WGL_ACCELERATION_ARB,
                                WGL_FULL_ACCELERATION_ARB,
                                WGL_PIXEL_TYPE_ARB,
                                WGL_TYPE_RGBA_ARB,
                                WGL_COLOR_BITS_ARB,
                                32,
                                WGL_DEPTH_BITS_ARB,
                                24,
                                WGL_STENCIL_BITS_ARB,
                                8,
                                0,
                                0};

  int pixel_format;
  UINT num_formats;
  wglChoosePixelFormatARB(hdc, pixel_format_attribs, 0, 1, &pixel_format,
                          &num_formats);
  if (!num_formats) {
    MessageBox(NULL, _T("Failed to set Modern OpenGL pixel format"),
               _T("bitEngine"), NULL);
    DestroyWindow(window->handle);
    return BITE_ERROR;
  }

  PIXELFORMATDESCRIPTOR pfd;
  DescribePixelFormat(hdc, pixel_format, sizeof(pfd), &pfd);
  if (!SetPixelFormat(hdc, pixel_format, &pfd)) {
    MessageBox(NULL, _T("Failed to set Modern OpenGL pixel format\n"),
               _T("bitEngine"), NULL);
    DestroyWindow(window->handle);
    return BITE_ERROR;
  }

  int gl_attribs[] = {WGL_CONTEXT_MAJOR_VERSION_ARB,
                      3,
                      WGL_CONTEXT_MINOR_VERSION_ARB,
                      0,
                      WGL_CONTEXT_PROFILE_MASK_ARB,
                      WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
                      0,
                      0};

  HGLRC hglrc = wglCreateContextAttribsARB(hdc, 0, gl_attribs);
  if (!hglrc) {
    MessageBox(NULL, _T("Failed to create Modern OpenGL context"),
               _T("bitEngine"), NULL);
    DestroyWindow(window->handle);
    return BITE_ERROR;
  }
  wglMakeCurrent(hdc, hglrc);

  if (_load_gl() != BITE_OK) {
    MessageBox(NULL, _T("Failed to init OpenGL loader"), _T("bitEngine"), NULL);
    DestroyWindow(window->handle);
    return BITE_ERROR;
  }
  _setup_gl();
  _close_gl();
#elif defined(__EMSCRIPTEN__)
  EmscriptenWebGLContextAttributes attr;
  emscripten_webgl_init_context_attributes(&attr);
  attr.alpha = 0;
  render->gl_context = emscripten_webgl_create_context("#canvas", &attr);
  emscripten_webgl_make_context_current(render->gl_context);
#else
  Display *dpy = window->display;
  int scrId = DefaultScreen(dpy);
  const char *glxExts = glXQueryExtensionsString(dpy, scrId);
  glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
  glXCreateContextAttribsARB =
      (glXCreateContextAttribsARBProc)glXGetProcAddressARB(
          (const GLubyte *)"glXCreateContextAttribsARB");

  GLXContext ctx = 0;

  if (!glXCreateContextAttribsARB) {
    fprintf(stderr, "Failed to load glXCreateContextAttribsARB(), loading "
                    "legacy OpenGL context\n");
    ctx = glXCreateNewContext(dpy, window->fbconf, GLX_RGBA_TYPE, 0, True);
  } else {
    int context_attribs[] = {GLX_CONTEXT_MAJOR_VERSION_ARB,
                             2,
                             GLX_CONTEXT_MINOR_VERSION_ARB,
                             1,
                             GLX_CONTEXT_PROFILE_MASK_ARB,
                             GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
                             None};

    ctx = glXCreateContextAttribsARB(dpy, window->fbconf, 0, True,
                                     context_attribs);
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
void _poll_events(be_Context *ctx) {
#if defined(_WIN32)
  MSG message;
  if (GetMessage(&message, NULL, 0, 0)) {
    TranslateMessage(&message);
    DispatchMessage(&message);
  }
#elif defined(__EMSCRIPTEN__)
#else
  XEvent ev;
  be_Window *window = &(ctx->window);
  while (XPending(window->display)) {
    XNextEvent(window->display, &ev);
    be_Event e = {0};
    be_EventCallback fn = NULL;
    switch (ev.type) {
      case ClientMessage: {
        if (ev.xclient.data.l[0] ==
            XInternAtom(window->display, "WM_DELETE_WINDOW", 0)) {
          e.type = BITE_WINDOW_CLOSE;
        }
      } break;
      case DestroyNotify: {
        e.type = BITE_QUIT;
      } break;
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
      } break;
      case KeyPress: {
        e.type = BITE_KEY_PRESSED;
        int code = ev.xkey.keycode;
        int keysym = XkbKeycodeToKeysym(
          ctx->window.display,
          ev.xkey.keycode,
          0,
          ev.xkey.state & ShiftMask ? 1 : 0
        );
        e.key.keycode = keys[keysym];
        printf("KeyPress: %d %d %d\n", keysym, ev.xkey.keycode, e.key.keycode);
      } break;
      case KeyRelease: {
        e.type = BITE_KEY_RELEASED;
        int keysym = XkbKeycodeToKeysym(
          ctx->window.display,
          ev.xkey.keycode,
          0,
          ev.xkey.state & ShiftMask ? 1 : 0
        );
        e.key.keycode = keys[keysym];
      } break;
    }
    fn = ctx->callbacks[e.type];
    if (fn)
      fn(ctx, &e);
  }
#endif
}

// OpenGL loader

#if !defined(__APPLE__) && !defined(__HAIKU__)
void *(*_proc_address)(const char *);
#endif

#define GET_GL_PROC(name) name = (name##Proc *)_get_proc(#name)

void *_get_proc(const char *);

BITE_RESULT _load_gl(void) {
#if defined(_WIN32)
  _gl_sym = LoadLibrary("opengl32.dll");
  if (_gl_sym == NULL) {
    fprintf(stderr, "Failed to load OpenGL32.dll\n");
    return BITE_ERROR;
  }
  _proc_address =
      (void *(*)(const char *))GetProcAddress(_gl_sym, "wglGetProcAddress");
#elif defined(__EMSCRIPTEN__)
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
      _proc_address =
          (void *(*)(const char *))dlsym(_gl_sym, "glXGetProcAddress");
      return _proc_address != NULL ? BITE_OK : BITE_ERROR;
#endif
      break;
    }
  }
#endif
  return BITE_OK;
}

#if !defined(__EMSCRIPTEN__)
void _close_gl(void) {
  if (_gl_sym != NULL) {
#if defined(_WIN32)
    FreeLibrary(_gl_sym);
#else
    dlclose(_gl_sym);
#endif
    _gl_sym = NULL;
  }
}

void _setup_gl(void) {
  const char *version = (const char*)glGetString(GL_VERSION);
  const char *glsl = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
  if (!version) {
    fprintf(stderr, "Failed to get OpenGL version\n");
  }
  const char *prefixes[] = {"OpenGL ES-CM ", "OpenGL ES-CL ", "OpenGL ES ",
                            NULL};

  if (version) {
    const char *ver = version;
    for (int i = 0; prefixes[i] != NULL; i++) {
      if (strncmp(ver, prefixes[i], strlen(prefixes[i])) == 0) {
        ver += strlen(prefixes[i]);
        fprintf(stdout, "Using OpenGL ES\n");
      }
    }
  }

  fprintf(stderr, "OpenGL: %s\n", version);

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
  GET_GL_PROC(glDeleteShader);
  GET_GL_PROC(glCreateProgram);
  GET_GL_PROC(glAttachShader);
  GET_GL_PROC(glLinkProgram);
  GET_GL_PROC(glGetProgramiv);
  GET_GL_PROC(glGetProgramInfoLog);
  GET_GL_PROC(glUseProgram);
  GET_GL_PROC(glDeleteProgram);
}

static inline void *_get_proc(const char *name) {
  void *sym = NULL;
  if (_gl_sym == NULL)
    return sym;
#if !defined(__APPLE__) && !defined(__HAIKU__)
  if (_proc_address != NULL)
    sym = _proc_address(name);
#endif
  if (sym == NULL) {
#if defined(_WIN32) || defined(__CYGWWIN__)
    sym = (void *)GetProcAddress(_gl_sym, name);
#else
    sym = (void *)dlsym(_gl_sym, name);
#endif
  }
  fprintf(stderr, "%s: %p\n", name, sym);
  return sym;
}
#endif
