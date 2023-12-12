#include "bite.h"

#if defined(__EMSCRIPTEN__)
    #include <emscripten.h>
#endif

const char* vert =
#if defined(__EMSCRIPTEN__)
"#version 100\n"
"attribute vec2 a_Position;\n"
"attribute vec4 a_Color;\n"
"varying vec4 v_Color;\n"
#else
"#version 140\n"
"in vec2 a_Position;\n"
"in vec4 a_Color;\n"
"out vec4 v_Color;\n"
#endif
"void main() {\n"
"   gl_Position = vec4(a_Position.x, a_Position.y, 0, 1.0);\n"
"   v_Color = a_Color;\n"
"}";

const char* frag =
#if defined(__EMSCRIPTEN__)
"#version 100\n"
"precision mediump float;\n"
"varying vec4 v_Color;\n"
"#define o_FragColor gl_FragColor\n"
#else
"#version 140\n"
"in vec4 v_Color;\n"
"out vec4 o_FragColor;\n"
#endif
"void main() {"
"   o_FragColor = v_Color;\n"
"}";

// #include <GL/gl.h>
be_Texture* tex;
be_Shader* shader;

void key_pressed(be_Context* ctx, be_Event* ev) {
    printf("Pressed: %d\n", ev->key.keycode);
    if (ev->key.keycode == BITEK_ESCAPE) bite_set_should_close(ctx, 1);
}

void quit_callback(be_Context* ctx, be_Event* ev) {
    bite_set_should_close(ctx, 1);
}

void main_loop(void* data) {
    // printf("Entering render function\n");
    be_Context* ctx = (be_Context*)data;
    bite_poll_events(ctx);
    bite_use_shader(shader);
    bite_simple_triangle(ctx);
    bite_use_shader(NULL);
    bite_swap(ctx);
}

int main(int argc, char** argv) {
    be_Config c = bite_init_config("bitEngine", 640, 380);
    be_Context* ctx = bite_create(&c);
    be_u8 pixels[] = {255, 255, 255, 255};
    tex = bite_create_texture(1, 1, 0, pixels);
    shader = bite_create_shader(vert, frag);
#if defined(__EMSCRIPTEN__)
    emscripten_set_main_loop_arg(main_loop, ctx, 0, 1);
#else
    bite_register_callback(ctx, BITE_WINDOW_CLOSE, quit_callback);
    bite_register_callback(ctx, BITE_KEY_PRESSED, key_pressed);
    while (!bite_should_close(ctx)) main_loop(ctx);
#endif
    bite_destroy(ctx);
    return 0;
}
