#include <GLES2/gl2.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <string.h>
#include <stdio.h>

void upload_unicode_char_to_texture(int unicodeChar, int charSize, int applyShadow);
void load_texture_from_url(GLuint texture, const char *url, int *outWidth, int *outHeight);
void request_animation_frame_loop(EM_BOOL (*cb)(double time, void *userData), void *userData);
float find_character_pair_kerning(unsigned int ch1, unsigned int ch2, int charSize);

static EMSCRIPTEN_WEBGL_CONTEXT_HANDLE glContext;
static GLuint quad, colorPos, matPos, solidColor;
static float pixelWidth, pixelHeight;

static GLuint compile_shader(GLenum shaderType, const char *src)
{
   GLuint shader = glCreateShader(shaderType);
   glShaderSource(shader, 1, &src, NULL);
   glCompileShader(shader);
   return shader;
}

static GLuint create_program(GLuint vertexShader, GLuint fragmentShader)
{
   GLuint program = glCreateProgram();
   glAttachShader(program, vertexShader);
   glAttachShader(program, fragmentShader);
   glBindAttribLocation(program, 0, "pos");
   glLinkProgram(program);
   glUseProgram(program);
   return program;
}

static GLuint create_texture()
{
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  return texture;
}

void EMSCRIPTEN_KEEPALIVE init_webgl(int width, int height)
{
  double dpr = emscripten_get_device_pixel_ratio();
  emscripten_set_element_css_size("#remote-canvas", width / dpr, height / dpr);
  emscripten_set_canvas_element_size("#remote-canvas", width, height);

  EmscriptenWebGLContextAttributes attrs;
  emscripten_webgl_init_context_attributes(&attrs);
  attrs.alpha = 0;
  attrs.majorVersion = 2;
  glContext = emscripten_webgl_create_context("#remote-canvas", &attrs);
  emscripten_webgl_make_context_current(glContext);

  pixelWidth = 2.f / width;
  pixelHeight = 2.f / height;

  static const char vertex_shader[] =
    "attribute vec4 pos;"
    "varying vec2 uv;"
    "uniform mat4 mat;"
    "void main(){"
      "uv=pos.xy;"
      "gl_Position=mat*pos;"
    "}";
  GLuint vs = compile_shader(GL_VERTEX_SHADER, vertex_shader);

  static const char fragment_shader[] =
    "precision lowp float;"
    "uniform sampler2D tex;"
    "varying vec2 uv;"
    "uniform vec4 color;"
    "void main(){"
      "gl_FragColor=color*texture2D(tex,uv);"
    "}";
  GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fragment_shader);

  GLuint program = create_program(vs, fs);
  colorPos = glGetUniformLocation(program, "color");
  matPos = glGetUniformLocation(program, "mat");
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glGenBuffers(1, &quad);
  glBindBuffer(GL_ARRAY_BUFFER, quad);
  const float pos[] = { 0, 0, 1, 0, 0, 1, 1, 1 };
  glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  solidColor = create_texture();
  unsigned int whitePixel = 0xFFFFFFFFu;
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &whitePixel);
}

double EMSCRIPTEN_KEEPALIVE rand01()
{
  return emscripten_random();
}

typedef void (*tick_func)(double t, double dt);

static EM_BOOL tick(double time, void *userData)
{
  static double t0;
  double dt = time - t0;
  t0 = time;
  tick_func f = (tick_func)(userData);
  f(time, dt);
  return EM_TRUE;
}

void EMSCRIPTEN_KEEPALIVE set_animation_frame_callback(void (*func)(double t, double dt))
{
  emscripten_request_animation_frame_loop(tick, func);
}

void EMSCRIPTEN_KEEPALIVE clear_screen(float r, float g, float b, float a)
{
  glClearColor(r, g, b, a);
  glClear(GL_COLOR_BUFFER_BIT);
}

static void fill_textured_rectangle(float x0, float y0, float x1, float y1, float r, float g, float b, float a, GLuint texture)
{
  printf("[gfx] Drawing at %f, %f to %f, %f\n", x0, y0, x1, y1);
  float mat[16] = { (x1-x0)*pixelWidth, 0, 0, 0, 0, (y1-y0)*pixelHeight, 0, 0, 0, 0, 1, 0, (x0*pixelWidth)-1.f, (y0*pixelHeight)-1.f, 0, 1};
  glUniformMatrix4fv(matPos, 1, 0, mat);
  glUniform4f(colorPos, r, g, b, a);
  glBindTexture(GL_TEXTURE_2D, texture);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void EMSCRIPTEN_KEEPALIVE fill_solid_rectangle(float x0, float y0, float x1, float y1, float r, float g, float b, float a)
{
  fill_textured_rectangle(x0, y0, x1, y1, r, g, b, a, solidColor);
}

typedef struct Texture
{
  // Image
  char *url;
  int w, h;

  GLuint texture;
} Texture;

#define MAX_TEXTURES 256
static Texture textures[MAX_TEXTURES] = {};

static Texture *find_or_cache_url(const char *url)
{
  for(int i = 0; i < MAX_TEXTURES; ++i) // Naive O(n) lookup for tiny code size
    if (!strcmp(textures[i].url, url))
      return textures+i;
    else if (!textures[i].url)
    {
      textures[i].url = strdup(url);
      textures[i].texture = create_texture();
      load_texture_from_url(textures[i].texture, url, &textures[i].w, &textures[i].h);
      return textures+i;
    }
  return 0; // fail
}

static void load_texture_from_buffer(Texture* texture, const void* img_data, int w, int h) {
  glPixelStorei(0x9240/*GLctx.UNPACK_FLIP_Y_WEBGL*/, 1);
  uint32_t image_size = w * h * 2;
  uint32_t rgba_image_size = w * h * 4;
  uint32_t* rgba_data = (uint32_t*)malloc(rgba_image_size);
  bzero(rgba_data, rgba_image_size);
  for (int i =0; i < (w*h); i++) {
    uint16_t color = ((uint16_t*)img_data)[i];
    if (color == 0x0120) {
      continue;
    }
    uint8_t red = (color >> 11 & 0x1F) << 3; // red = 5 bits
    uint8_t green = ((color >> 5) & 0x3F) << 2; // green = 6 bits
    uint8_t blue = (color & 0x1F) << 3; // blue = 5 bits
    rgba_data[i] = (0xFF << 24) | (blue << 16) | (green << 8) | (red);
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba_data);
  // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, img_data);
  free(rgba_data);
  glPixelStorei(0x9240/*GLctx.UNPACK_FLIP_Y_WEBGL*/, 0);
  texture->w = w;
  texture->h = h;
}

static Texture *find_or_cache_tag(const char *tag, const void* img_data, int w, int h) {
  for(int i = 0; i < MAX_TEXTURES; ++i) // Naive O(n) lookup for tiny code size
    if (!strcmp(textures[i].url, tag))
      return textures+i;
    else if (!textures[i].url)
    {
      textures[i].url = strdup(tag);
      textures[i].texture = create_texture();
      load_texture_from_buffer(&textures[i], img_data, w, h);
      return textures+i;
    }
  return 0; // fail
}

void EMSCRIPTEN_KEEPALIVE load_image_by_tag(const char *tag, const void* data, int w, int h) {
  find_or_cache_tag(tag, data, w, h);
}

void EMSCRIPTEN_KEEPALIVE fill_image(float x0, float y0, float scale, float r, float g, float b, float a, const char *url)
{
  Texture *t = find_or_cache_url(url);
  fill_textured_rectangle(x0, y0, x0 + t->w * scale, y0 + t->h * scale, r, g, b, a, t->texture);
}

void EMSCRIPTEN_KEEPALIVE fill_image_tag(float x0, float y0, const char *tag)
{
  Texture *t = find_or_cache_url(tag);
  if (t == NULL) return; // TODO: handle this better
  float x1 = x0 + ((float)t->w);
  float y1 = y0 + ((float)t->h);
  fill_textured_rectangle(x0, y0, x1, y1, 1.0, 1.0, 1.0, 1.0f, t->texture);
}

typedef struct Glyph
{
  // Font
  unsigned int ch;
  int charSize;
  int shadow;

  GLuint texture;
} Glyph;

#define MAX_GLYPHS 256
static Glyph glyphs[MAX_GLYPHS] = {};
static Glyph *find_or_cache_character(unsigned int ch, int charSize, int shadow)
{
  for(int i = 0; i < MAX_TEXTURES; ++i) // Naive O(n) lookup for tiny code size
    if (glyphs[i].ch == ch && glyphs[i].charSize == charSize && glyphs[i].shadow == shadow)
      return glyphs+i;
    else if (!glyphs[i].ch)
    {
      glyphs[i].ch = ch;
      glyphs[i].charSize = charSize;
      glyphs[i].shadow = shadow;
      glyphs[i].texture = create_texture();
      upload_unicode_char_to_texture(ch, charSize, shadow);
      return glyphs+i;
    }
  return 0; // fail
}

typedef struct KerningPair
{
  // Font
  unsigned int ch1, ch2;
  int charSize;
  float kerning;
} KerningPair;

#define MAX_KERNING_PAIRS 4096
static KerningPair kerningPairs[MAX_KERNING_PAIRS] = {};

static float get_character_pair_kerning(unsigned int ch1, unsigned int ch2, int charSize)
{
  size_t i = 0;
  for(; i < MAX_KERNING_PAIRS; ++i)
  {
    if (!kerningPairs[i].ch1)
      break;
    if (kerningPairs[i].ch1 == ch1 && kerningPairs[i].ch2 == ch2 && kerningPairs[i].charSize == charSize)
      return kerningPairs[i].kerning;
  }
  float kerning = find_character_pair_kerning(ch1, ch2, charSize);
  kerningPairs[i].ch1 = ch1;
  kerningPairs[i].ch2 = ch2;
  kerningPairs[i].charSize = charSize;
  kerningPairs[i].kerning = kerning;
  return kerning;
}

static void fill_char(float x0, float y0, float r, float g, float b, float a, unsigned int ch, int charSize, int shadow)
{
  fill_textured_rectangle(x0, y0, x0+charSize, y0+charSize, r, g, b, a, find_or_cache_character(ch, charSize, shadow)->texture);
}

void EMSCRIPTEN_KEEPALIVE fill_text(float x0, float y0, float r, float g, float b, float a, const char *str, float spacing, int charSize, int shadow)
{
  while(*str)
  {
    char ch = *str++;
    fill_char(x0, y0, r, g, b, a, ch, charSize, shadow);
    char next = *str;
    x0 += get_character_pair_kerning(ch, next, charSize);
  }
}