# bitE (bit Engine)

Projetinho com o objetivo de não ter nenhuma dependência externa além das do próprio sistema. Então a ideia é ter o básico pra criar um joguinho (janela, input, áudio e gráficos).

Pros gráficos pretendo usar `OpenGL`, que também está presente em praticamente todas as plataformas.

Inicialmente quero focar no desktop pra aprender como funciona, e também pra web utilizando Emscripten, já que pelo menos a parte de render dá pra aproveitar utilizando `OpenGLES2`.

- Windows
- Linux
- MacOSX
- Emscripten

Formatos suportados obviamente vão ser os mais simples de se ler: `.bmp` para imagens e `.wav` para áudios. Alguns que também são relativamente fáceis de implementar são `.json` e `.tar`, mas fica pra depois.

A ideia é ser uma parada mais simples mesmo, só com o básico, se quer algo mais completo e que também seja multiplataforma tem outras que tão no mercado aí pra isso ([SDL2](https://libsdl.org) e [GLFW](https://glfw.org) por exemplo).

Esse artigo aqui dá uma boa pincelada no assunto pra quem tiver interesse: https://zserge.com/posts/fenster/

[fenster](https://github.com/zserge/fenster)