#include "bite.h"

be_Window* wnd;

static int running = 1;

void quit_callback(be_Event* ev) {
    running = 0;
}

void render() {
    printf("Entering render function\n");
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
    bite_window_swap(wnd);
}

int main(int argc, char** argv) {
    bite_init(0);
    printf("Testando essa porra\n");
    wnd = bite_create_window("Teste", 640, 380, 0);
#if 0
    bite_register_callback(BITE_QUIT, quit_callback);
    while (running) {
        bite_begin();
        bite_end();
    }
#endif
    printf("Serase 1\n");
#if defined(__EMSCRIPTEN__)
    emscripten_set_main_loop(render, 0, 1);
#else
    render();
#endif
    bite_timer_sleep(3000);
    printf("Serase 2\n");
    bite_destroy_window(wnd);
    bite_quit();
    return 0;
}