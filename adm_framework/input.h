#pragma once
#include <stdbool.h>
#include <adm_utils/util.h>
#include "gmath.h"

typedef enum input_key_t {
    INPUT_KEY_UNKNOWN = -1,
    INPUT_KEY_SPACE,
    INPUT_KEY_APOSTROPHE,
    INPUT_KEY_COMMA,
    INPUT_KEY_MINUS,
    INPUT_KEY_PERIOD,
    INPUT_KEY_SLASH,
    INPUT_KEY_0,
    INPUT_KEY_1,
    INPUT_KEY_2,
    INPUT_KEY_3,
    INPUT_KEY_4,
    INPUT_KEY_5,
    INPUT_KEY_6,
    INPUT_KEY_7,
    INPUT_KEY_8,
    INPUT_KEY_9,
    INPUT_KEY_SEMICOLON,
    INPUT_KEY_EQUAL,
    INPUT_KEY_A,
    INPUT_KEY_B,
    INPUT_KEY_C,
    INPUT_KEY_D,
    INPUT_KEY_E,
    INPUT_KEY_F,
    INPUT_KEY_G,
    INPUT_KEY_H,
    INPUT_KEY_I,
    INPUT_KEY_J,
    INPUT_KEY_K,
    INPUT_KEY_L,
    INPUT_KEY_M,
    INPUT_KEY_N,
    INPUT_KEY_O,
    INPUT_KEY_P,
    INPUT_KEY_Q,
    INPUT_KEY_R,
    INPUT_KEY_S,
    INPUT_KEY_T,
    INPUT_KEY_U,
    INPUT_KEY_V,
    INPUT_KEY_W,
    INPUT_KEY_X,
    INPUT_KEY_Y,
    INPUT_KEY_Z,
    INPUT_KEY_LEFT_BRACKET,
    INPUT_KEY_BACKSLASH,
    INPUT_KEY_RIGHT_BRACKET,
    INPUT_KEY_GRAVE_ACCENT,
    INPUT_KEY_WORLD_1,
    INPUT_KEY_WORLD_2,
    INPUT_KEY_ESCAPE,
    INPUT_KEY_ENTER,
    INPUT_KEY_TAB,
    INPUT_KEY_BACKSPACE,
    INPUT_KEY_INSERT,
    INPUT_KEY_DELETE,
    INPUT_KEY_RIGHT,
    INPUT_KEY_LEFT,
    INPUT_KEY_DOWN,
    INPUT_KEY_UP,
    INPUT_KEY_PAGE_UP,
    INPUT_KEY_PAGE_DOWN,
    INPUT_KEY_HOME,
    INPUT_KEY_END,
    INPUT_KEY_CAPS_LOCK,
    INPUT_KEY_SCROLL_LOCK,
    INPUT_KEY_NUM_LOCK,
    INPUT_KEY_PRINT_SCREEN,
    INPUT_KEY_PAUSE,
    INPUT_KEY_F1,
    INPUT_KEY_F2,
    INPUT_KEY_F3,
    INPUT_KEY_F4,
    INPUT_KEY_F5,
    INPUT_KEY_F6,
    INPUT_KEY_F7,
    INPUT_KEY_F8,
    INPUT_KEY_F9,
    INPUT_KEY_F10,
    INPUT_KEY_F11,
    INPUT_KEY_F12,
    INPUT_KEY_F13,
    INPUT_KEY_F14,
    INPUT_KEY_F15,
    INPUT_KEY_F16,
    INPUT_KEY_F17,
    INPUT_KEY_F18,
    INPUT_KEY_F19,
    INPUT_KEY_F20,
    INPUT_KEY_F21,
    INPUT_KEY_F22,
    INPUT_KEY_F23,
    INPUT_KEY_F24,
    INPUT_KEY_F25,
    INPUT_KEY_KP_0,
    INPUT_KEY_KP_1,
    INPUT_KEY_KP_2,
    INPUT_KEY_KP_3,
    INPUT_KEY_KP_4,
    INPUT_KEY_KP_5,
    INPUT_KEY_KP_6,
    INPUT_KEY_KP_7,
    INPUT_KEY_KP_8,
    INPUT_KEY_KP_9,
    INPUT_KEY_KP_DECIMAL,
    INPUT_KEY_KP_DIVIDE,
    INPUT_KEY_KP_MULTIPLY,
    INPUT_KEY_KP_SUBTRACT,
    INPUT_KEY_KP_ADD,
    INPUT_KEY_KP_ENTER,
    INPUT_KEY_KP_EQUAL,
    INPUT_KEY_LEFT_SHIFT,
    INPUT_KEY_LEFT_CONTROL,
    INPUT_KEY_LEFT_ALT,
    INPUT_KEY_LEFT_SUPER,
    INPUT_KEY_RIGHT_SHIFT,
    INPUT_KEY_RIGHT_CONTROL,
    INPUT_KEY_RIGHT_ALT,
    INPUT_KEY_RIGHT_SUPER,
    INPUT_KEY_MENU,
    _input_key_count,
} input_key_t;

typedef enum input_mouse_button_t {
    INPUT_MOUSE_BUTTON_UNKNOWN = -1,
    INPUT_MOUSE_BUTTON_LEFT,
    INPUT_MOUSE_BUTTON_MIDDLE,
    INPUT_MOUSE_BUTTON_RIGHT,
    _input_mouse_button_count,
} input_mouse_button_t;

typedef enum input_button_state_t {
    INPUT_BUTTON_STATE_RELEASED,
    INPUT_BUTTON_STATE_JUST_RELEASED,
    INPUT_BUTTON_STATE_JUST_PRESSED,
    INPUT_BUTTON_STATE_PRESSED,
	INPUT_BUTTON_STATE_REPEAT,
} input_button_state_t;

typedef struct app_t app_t;

typedef struct input_t {
	app_t* app;
    u8 key_states[_input_key_count];
    u8 mouse_button_states[_input_mouse_button_count];
	uint codepoint;
	vec2f_t scroll;
} input_t;

typedef struct app_t app_t;

void _input_init(app_t* app);
void _input_frame(input_t* input);
input_button_state_t input_key_state(app_t* app, input_key_t key);
bool input_key_down(app_t* app, input_key_t key);
bool input_key_up(app_t* app, input_key_t key);
bool input_key_just_pressed(app_t* app, input_key_t key);
bool input_key_just_released(app_t* app, input_key_t key);
input_button_state_t input_mouse_button_state(app_t* app, input_mouse_button_t mouse_button);
bool input_mouse_button_down(app_t* app, input_mouse_button_t mouse_button);
bool input_mouse_button_up(app_t* app, input_mouse_button_t mouse_button);
bool input_mouse_button_pressed(app_t* app, input_mouse_button_t mouse_button);
bool input_mouse_button_released(app_t* app, input_mouse_button_t mouse_button);
vec2i_t input_mouse_pos(app_t* app);
void input_set_mouse_pos(app_t* app, const vec2i_t* pos);
vec2f_t input_mouse_scroll(app_t* app);
NULLABLE const char* input_clipboard_string(app_t* app);
void input_set_clipboard_string(app_t* app, const char* str);
// If a char is typed *this frame* this function will return it
uint input_codepoint(app_t* app);
