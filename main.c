#include "bite.h"

#if defined(__EMSCRIPTEN__)
    #include <emscripten.h>
#endif

// #include <GL/gl.h>

void key_pressed(be_Context* ctx, be_Event* ev) {
    printf("Pressed: %x\n", ev->key.keycode);
    if (ev->key.keycode == BITEK_ESCAPE) bite_set_should_close(ctx, 1);
}

void quit_callback(be_Context* ctx, be_Event* ev) {
    bite_set_should_close(ctx, 1);
}

void main_loop(void* data) {
    // printf("Entering render function\n");
    be_Context* ctx = (be_Context*)data;
    bite_poll_events(ctx);
    // glClearColor(0.3f, 0.4f, 0.4f, 1.f);
    // glClear(GL_COLOR_BUFFER_BIT);
    // glBegin(GL_TRIANGLES);
    // glColor3f(1.f, 0.f, 0.f);
    // glVertex2f(0.f, 0.5f);
    // glColor3f(0.f, 1.f, 0.f);
    // glVertex2f(-0.5f, -0.5f);
    // glColor3f(0.f, 0.f, 1.f);
    // glVertex2f(0.5f, -0.5f);
    // glEnd();
    bite_simple_triangle();
    bite_swap(ctx);
}

int main(int argc, char** argv) {
    be_Config c = bite_init_config("bitEngine", 640, 380);
    be_Context* ctx = bite_create(&c);
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