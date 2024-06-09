#include "webgl.h"
#include <stdio.h>
#include <emscripten.h>
#include <string.h>
#include <emscripten/html5.h>
#include <math.h>

#include <remote/remote.hpp>
#include <bridge/bridge.hpp>
#include <test/fakemesh.hpp>
#include "js_transport.hpp"
#include "webgl_adapter.hpp"

static inline const char *emscripten_event_type_to_string(int eventType) {
  const char *events[] = { "(invalid)", "(none)", "keypress", "keydown", "keyup", "click", "mousedown", "mouseup", "dblclick", "mousemove", "wheel", "resize",
    "scroll", "blur", "focus", "focusin", "focusout", "deviceorientation", "devicemotion", "orientationchange", "fullscreenchange", "pointerlockchange",
    "visibilitychange", "touchstart", "touchend", "touchmove", "touchcancel", "gamepadconnected", "gamepaddisconnected", "beforeunload",
    "batterychargingchange", "batterylevelchange", "webglcontextlost", "webglcontextrestored", "mouseenter", "mouseleave", "mouseover", "mouseout", "(invalid)" };
  ++eventType;
  if (eventType < 0) eventType = 0;
  if (eventType >= sizeof(events)/sizeof(events[0])) eventType = sizeof(events)/sizeof(events[0])-1;
  return events[eventType];
}

// Per-frame animation tick.
void draw_frame(double t, double dt)
{
  clear_screen(0.1f, 0.2f, 0.3f, 1.f);

  // snow background
#define NUM_FLAKES 100
#define FLAKE_SIZE 10
#define FLAKE_SPEED 0.05f
#define SNOWINESS 0.998
  static struct { float x, y; } flakes[NUM_FLAKES] = {};

#define SIM do { \
      flakes[i].y -= dt*(FLAKE_SPEED+i*0.05f/NUM_FLAKES); \
      flakes[i].x += dt*(fmodf(i*345362.f, 0.02f) - 0.01f); \
      float c = 0.5f + i*0.5/NUM_FLAKES; \
      if (flakes[i].y > -FLAKE_SIZE) fill_solid_rectangle(flakes[i].x, flakes[i].y, flakes[i].x+FLAKE_SIZE, flakes[i].y+FLAKE_SIZE, c, c, c, 1.f); \
      else if (rand01() > SNOWINESS) flakes[i].y = HEIGHT, flakes[i].x = WIDTH*rand01(); \
    } while(0)
  for(int i = 0; i < NUM_FLAKES/2; ++i) SIM;

  // flagpole x,y,width,height
#define FPX 50.f
#define FPW 25.f
#define FPH (HEIGHT-75.f)
  fill_solid_rectangle(FPX, 0.f, FPX+FPW, FPH, 0, 0, 0, 1.f);
  // flag x,y,width,height
#define FX 80
#define FH 200
#define FY (FPH-FH)
#define FW (18.f/11.f*FH)
  // flag grid
#define GX 18
#define GY 11
  // block size
#define BX ((double)FW/GX)
#define BY ((double)FH/GY)
  // color
#define COLOR(gx, gy) (((gx >= 5 && gx <= 7) || (gy >= 4 && gy <= 6)) ? 1 : 0)
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
  for(int y = 0; y < GY; ++y)
    for(int x = 0; x < GX; ++x)
    {
      int c = COLOR(x, y);
      float wy = sinf(0.3f*(x+t*0.01f))*GY*0.5f*(MIN((float)x*3.f/GX,1.f));
      fill_solid_rectangle(FX+x*BX, FY+y*BY+wy, FX+(x+1)*BX, FY+(y+1)*BY+wy, c?0.f:1.f, c?47/255.f:1.f, c?108/255.f:1.f, 1.f);
    }

  // ground
  fill_solid_rectangle(0.f, 0.f, WIDTH, 20.f, 0.8f, 0.8f, 0.8f, 1.f);

  // tree & reindeer
  fill_image(WIDTH-320.f, 0.f, 2.f, 1.f, 1.f, 1.f, 1.f, "tree.png");
  fill_image(250.f, 10.f, 1.f, 1.f, 1.f, 1.f, 1.f, "reindeer.png");

  // snow foreground
  for(int i = NUM_FLAKES/2; i < NUM_FLAKES; ++i) SIM;

  // text
  fill_text(190.f, HEIGHT*2/5+80.f, 1.f, 0.f, 0.f, 1.f, "MERRY XMAS", 64.f, 64, true);
}
EM_BOOL webglcontext_callback(int eventType, const void *reserved, void *userData) {
  printf("%s.\n", emscripten_event_type_to_string(eventType));

  return 0;
}

void reboot_unexpected() {
  emscripten_throw_string("Unexpected reboot");
}

RemoteDevice* remote;
BridgeDevice* bridge;
JSBridgeTransport* presenter;

void loop(double t, double dt) {
  // draw_frame(t, dt);
  remote->Loop();
  bridge->Loop();
}

#define KEYDOWN_EVENT_ID  2
#define KEYUP_EVENT_ID 3
#define NUMBER_1_KEYCODE 49
#define NUMBER_2_KEYCODE 50
#define NUMBER_3_KEYCODE 51
#define NUMBER_4_KEYCODE 52
#define NUMBER_5_KEYCODE 53
#define NUMBER_6_KEYCODE 54
#define NUMBER_7_KEYCODE 55
#define NUMBER_8_KEYCODE 56

EM_BOOL key_callback(int eventType, const EmscriptenKeyboardEvent *e, void *userData) {
    // printf("%x%s, key: \"%s\", code: \"%s\", location: %u,%s%s%s%s repeat: %d, locale: \"%s\", char: \"%s\", charCode: %u, keyCode: %u, which: %u, timestamp: %lf\n", eventType,
    //   emscripten_event_type_to_string(eventType), e->key, e->code, e->location,
    //   e->ctrlKey ? " CTRL" : "", e->shiftKey ? " SHIFT" : "", e->altKey ? " ALT" : "", e->metaKey ? " META" : "",
    //   e->repeat, e->locale, e->charValue, e->charCode, e->keyCode, e->which,
    //   e->timestamp);
  if (e->keyCode >= NUMBER_1_KEYCODE && e->keyCode <= NUMBER_4_KEYCODE) {
    uint8_t index = e->keyCode - NUMBER_1_KEYCODE;
    if (eventType == KEYDOWN_EVENT_ID) {
      remote->ButtonPressed(index);
    }
    if (eventType == KEYUP_EVENT_ID) {
      remote->ButtonReleased(index);
    }
  }
  if (e->keyCode >= NUMBER_5_KEYCODE && e->keyCode <= NUMBER_8_KEYCODE) {
    uint8_t index = e->keyCode - NUMBER_5_KEYCODE;
    if (eventType == KEYDOWN_EVENT_ID) {
      bridge->ButtonPressed(index);
    }
    if (eventType == KEYUP_EVENT_ID) {
      bridge->ButtonReleased(index);
    }
  }
  return 0;
}

EM_BOOL mouse_callback(int eventType, const EmscriptenMouseEvent *e, void *userData) {
  printf("%s, screen: (%d,%d), client: (%d,%d),%s%s%s%s button: %hu, buttons: %hu, movement: (%d,%d), canvas: (%d,%d), timestamp: %lf\n",
    emscripten_event_type_to_string(eventType), e->screenX, e->screenY, e->clientX, e->clientY,
    e->ctrlKey ? " CTRL" : "", e->shiftKey ? " SHIFT" : "", e->altKey ? " ALT" : "", e->metaKey ? " META" : "",
    e->button, e->buttons, e->movementX, e->movementY, e->canvasX, e->canvasY,
    e->timestamp);

  return 0;
}

EM_BOOL touch_callback(int eventType, const EmscriptenTouchEvent *e, void *userData) {
  printf("%s, numTouches: %d timestamp: %lf %s%s%s%s\n",
    emscripten_event_type_to_string(eventType), e->numTouches, e->timestamp,
    e->ctrlKey ? " CTRL" : "", e->shiftKey ? " SHIFT" : "", e->altKey ? " ALT" : "", e->metaKey ? " META" : "");
  for (int i = 0; i < e->numTouches; ++i) {
    const EmscriptenTouchPoint *t = &e->touches[i];
    printf("  %d: screen: (%d,%d), client: (%d,%d), page: (%d,%d), isChanged: %d, onTarget: %d, canvas: (%d, %d)\n",
      t->identifier, t->screenX, t->screenY, t->clientX, t->clientY, t->pageX, t->pageY, t->isChanged, t->onTarget, t->canvasX, t->canvasY);
  }

  return 0;
}

extern "C" void send_presenter_msg(const char* msg) {
  uint32_t msg_size = strlen(msg);
  printf("Got presenter message %s\n", msg);
  presenter->SendTextMessageToBridge(msg, msg_size);
}
uint8_t remote_mac[6] = {0};
char remote_mac_str[20] = {0};

extern "C" const char* get_remote_mac() {
  presenter->MacToString(remote_mac, remote_mac_str, sizeof(remote_mac_str));
  return (const char*)remote_mac_str;
}

extern "C" const char* fake_presenter_state_dark() {

  PRESENTER_PRESENTERSETSTATE(msg, "");
  const uint32_t text_size = 255;
  char* text = (char*)malloc(text_size);
  bzero(text,255);
  strncpy(msg.state_name, "dark", sizeof(msg.state_name));
  presenter->ConvertMessageToString(PRESENTER_MAC, remote_mac, (const uint8_t *)&msg, sizeof(msg), text, text_size);
  printf("Crafting fake message of %s\n", text);
  return text;
}

int main()
{
  init_webgl(WIDTH, HEIGHT);
  // EMSCRIPTEN_RESULT ret = emscripten_set_keypress_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, 1, key_callback);
  emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, 1, key_callback);
  emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, 1, key_callback);
  //emscripten_set_click_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, 1, mouse_callback);

  // Fake presenter
  presenter = new JSBridgeTransport();

  FakeMesh* mesh = new FakeMesh();
  // Remote
  NamespacedPrinter* remotePrint = new NamespacedPrinter("remote");
  for (int i=0; i < 6; i++) remote_mac[i] = (int8_t)(255 * emscripten_random());
  remote = new RemoteDevice(remotePrint, reboot_unexpected, remote_mac, new RemoteGraphicsWebgl());
  FakeMeshCommunicator* remote_comm = new FakeMeshCommunicator(mesh);
  remote_comm->registerDevice(remote);
  // Bridge
  NamespacedPrinter* bridgePrint = new NamespacedPrinter("bridge");
  uint8_t bridge_mac[] = {0x01, 0x12, 0x23, 0x34, 0x45, 0x56};
  bridge_mac[0] = (int8_t)(255 * emscripten_random());
  bridge = new BridgeDevice(bridgePrint, reboot_unexpected, presenter, bridge_mac);
  FakeMeshCommunicator* bridge_comm = new FakeMeshCommunicator(mesh);
  bridge_comm->registerDevice(bridge);
  // Setup
  bridge->PreSetup();
  remote->PreSetup();
  bridge->Setup();
  remote->Setup();
  
  set_animation_frame_callback(&loop);
}