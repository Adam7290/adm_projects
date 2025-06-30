#include "input.h"
#include "app.h"

#include <adm_utils/panic.h>
#include <adm_utils/arena.h>

#include <GLFW/glfw3.h>

PRIVATE int _input_key_to_glfw(input_key_t key) {
    switch (key) {
        case INPUT_KEY_SPACE:
            return GLFW_KEY_SPACE;
        case INPUT_KEY_APOSTROPHE:
            return GLFW_KEY_APOSTROPHE;
        case INPUT_KEY_COMMA:
            return GLFW_KEY_COMMA;
        case INPUT_KEY_MINUS:
            return GLFW_KEY_MINUS;
        case INPUT_KEY_PERIOD:
            return GLFW_KEY_PERIOD;
        case INPUT_KEY_SLASH:
            return GLFW_KEY_SLASH;
        case INPUT_KEY_0:
            return GLFW_KEY_0;
        case INPUT_KEY_1:
            return GLFW_KEY_1;
        case INPUT_KEY_2:
            return GLFW_KEY_2;
        case INPUT_KEY_3:
            return GLFW_KEY_3;
        case INPUT_KEY_4:
            return GLFW_KEY_4;
        case INPUT_KEY_5:
            return GLFW_KEY_5;
        case INPUT_KEY_6:
            return GLFW_KEY_6;
        case INPUT_KEY_7:
            return GLFW_KEY_7;
        case INPUT_KEY_8:
            return GLFW_KEY_8;
        case INPUT_KEY_9:
            return GLFW_KEY_9;
        case INPUT_KEY_SEMICOLON:
            return GLFW_KEY_SEMICOLON;
        case INPUT_KEY_EQUAL:
            return GLFW_KEY_EQUAL;
        case INPUT_KEY_A:
            return GLFW_KEY_A;
        case INPUT_KEY_B:
            return GLFW_KEY_B;
        case INPUT_KEY_C:
            return GLFW_KEY_C;
        case INPUT_KEY_D:
            return GLFW_KEY_D;
        case INPUT_KEY_E:
            return GLFW_KEY_E;
        case INPUT_KEY_F:
            return GLFW_KEY_F;
        case INPUT_KEY_G:
            return GLFW_KEY_G;
        case INPUT_KEY_H:
            return GLFW_KEY_H;
        case INPUT_KEY_I:
            return GLFW_KEY_I;
        case INPUT_KEY_J:
            return GLFW_KEY_J;
        case INPUT_KEY_K:
            return GLFW_KEY_K;
        case INPUT_KEY_L:
            return GLFW_KEY_L;
        case INPUT_KEY_M:
            return GLFW_KEY_M;
        case INPUT_KEY_N:
            return GLFW_KEY_N;
        case INPUT_KEY_O:
            return GLFW_KEY_O;
        case INPUT_KEY_P:
            return GLFW_KEY_P;
        case INPUT_KEY_Q:
            return GLFW_KEY_Q;
        case INPUT_KEY_R:
            return GLFW_KEY_R;
        case INPUT_KEY_S:
            return GLFW_KEY_S;
        case INPUT_KEY_T:
            return GLFW_KEY_T;
        case INPUT_KEY_U:
            return GLFW_KEY_U;
        case INPUT_KEY_V:
            return GLFW_KEY_V;
        case INPUT_KEY_W:
            return GLFW_KEY_W;
        case INPUT_KEY_X:
            return GLFW_KEY_X;
        case INPUT_KEY_Y:
            return GLFW_KEY_Y;
        case INPUT_KEY_Z:
            return GLFW_KEY_Z;
        case INPUT_KEY_LEFT_BRACKET:
            return GLFW_KEY_LEFT_BRACKET;
        case INPUT_KEY_BACKSLASH:
            return GLFW_KEY_BACKSLASH;
        case INPUT_KEY_RIGHT_BRACKET:
            return GLFW_KEY_RIGHT_BRACKET;
        case INPUT_KEY_GRAVE_ACCENT:
            return GLFW_KEY_GRAVE_ACCENT;
        case INPUT_KEY_WORLD_1:
            return GLFW_KEY_WORLD_1;
        case INPUT_KEY_WORLD_2:
            return GLFW_KEY_WORLD_2;
        case INPUT_KEY_ESCAPE:
            return GLFW_KEY_ESCAPE;
        case INPUT_KEY_ENTER:
            return GLFW_KEY_ENTER;
        case INPUT_KEY_TAB:
            return GLFW_KEY_TAB;
        case INPUT_KEY_BACKSPACE:
            return GLFW_KEY_BACKSPACE;
        case INPUT_KEY_INSERT:
            return GLFW_KEY_INSERT;
        case INPUT_KEY_DELETE:
            return GLFW_KEY_DELETE;
        case INPUT_KEY_RIGHT:
            return GLFW_KEY_RIGHT;
        case INPUT_KEY_LEFT:
            return GLFW_KEY_LEFT;
        case INPUT_KEY_DOWN:
            return GLFW_KEY_DOWN;
        case INPUT_KEY_UP:
            return GLFW_KEY_UP;
        case INPUT_KEY_PAGE_UP:
            return GLFW_KEY_PAGE_UP;
        case INPUT_KEY_PAGE_DOWN:
            return GLFW_KEY_PAGE_DOWN;
        case INPUT_KEY_HOME:
            return GLFW_KEY_HOME;
        case INPUT_KEY_END:
            return GLFW_KEY_END;
        case INPUT_KEY_CAPS_LOCK:
            return GLFW_KEY_CAPS_LOCK;
        case INPUT_KEY_SCROLL_LOCK:
            return GLFW_KEY_SCROLL_LOCK;
        case INPUT_KEY_NUM_LOCK:
            return GLFW_KEY_NUM_LOCK;
        case INPUT_KEY_PRINT_SCREEN:
            return GLFW_KEY_PRINT_SCREEN;
        case INPUT_KEY_PAUSE:
            return GLFW_KEY_PAUSE;
        case INPUT_KEY_F1:
            return GLFW_KEY_F1;
        case INPUT_KEY_F2:
            return GLFW_KEY_F2;
        case INPUT_KEY_F3:
            return GLFW_KEY_F3;
        case INPUT_KEY_F4:
            return GLFW_KEY_F4;
        case INPUT_KEY_F5:
            return GLFW_KEY_F5;
        case INPUT_KEY_F6:
            return GLFW_KEY_F6;
        case INPUT_KEY_F7:
            return GLFW_KEY_F7;
        case INPUT_KEY_F8:
            return GLFW_KEY_F8;
        case INPUT_KEY_F9:
            return GLFW_KEY_F9;
        case INPUT_KEY_F10:
            return GLFW_KEY_F10;
        case INPUT_KEY_F11:
            return GLFW_KEY_F11;
        case INPUT_KEY_F12:
            return GLFW_KEY_F12;
        case INPUT_KEY_F13:
            return GLFW_KEY_F13;
        case INPUT_KEY_F14:
            return GLFW_KEY_F14;
        case INPUT_KEY_F15:
            return GLFW_KEY_F15;
        case INPUT_KEY_F16:
            return GLFW_KEY_F16;
        case INPUT_KEY_F17:
            return GLFW_KEY_F17;
        case INPUT_KEY_F18:
            return GLFW_KEY_F18;
        case INPUT_KEY_F19:
            return GLFW_KEY_F19;
        case INPUT_KEY_F20:
            return GLFW_KEY_F20;
        case INPUT_KEY_F21:
            return GLFW_KEY_F21;
        case INPUT_KEY_F22:
            return GLFW_KEY_F22;
        case INPUT_KEY_F23:
            return GLFW_KEY_F23;
        case INPUT_KEY_F24:
            return GLFW_KEY_F24;
        case INPUT_KEY_F25:
            return GLFW_KEY_F25;
        case INPUT_KEY_KP_0:
            return GLFW_KEY_KP_0;
        case INPUT_KEY_KP_1:
            return GLFW_KEY_KP_1;
        case INPUT_KEY_KP_2:
            return GLFW_KEY_KP_2;
        case INPUT_KEY_KP_3:
            return GLFW_KEY_KP_3;
        case INPUT_KEY_KP_4:
            return GLFW_KEY_KP_4;
        case INPUT_KEY_KP_5:
            return GLFW_KEY_KP_5;
        case INPUT_KEY_KP_6:
            return GLFW_KEY_KP_6;
        case INPUT_KEY_KP_7:
            return GLFW_KEY_KP_7;
        case INPUT_KEY_KP_8:
            return GLFW_KEY_KP_8;
        case INPUT_KEY_KP_9:
            return GLFW_KEY_KP_9;
        case INPUT_KEY_KP_DECIMAL:
            return GLFW_KEY_KP_DECIMAL;
        case INPUT_KEY_KP_DIVIDE:
            return GLFW_KEY_KP_DIVIDE;
        case INPUT_KEY_KP_MULTIPLY:
            return GLFW_KEY_KP_MULTIPLY;
        case INPUT_KEY_KP_SUBTRACT:
            return GLFW_KEY_KP_SUBTRACT;
        case INPUT_KEY_KP_ADD:
            return GLFW_KEY_KP_ADD;
        case INPUT_KEY_KP_ENTER:
            return GLFW_KEY_KP_ENTER;
        case INPUT_KEY_KP_EQUAL:
            return GLFW_KEY_KP_EQUAL;
        case INPUT_KEY_LEFT_SHIFT:
            return GLFW_KEY_LEFT_SHIFT;
        case INPUT_KEY_LEFT_CONTROL:
            return GLFW_KEY_LEFT_CONTROL;
        case INPUT_KEY_LEFT_ALT:
            return GLFW_KEY_LEFT_ALT;
        case INPUT_KEY_LEFT_SUPER:
            return GLFW_KEY_LEFT_SUPER;
        case INPUT_KEY_RIGHT_SHIFT:
            return GLFW_KEY_RIGHT_SHIFT;
        case INPUT_KEY_RIGHT_CONTROL:
            return GLFW_KEY_RIGHT_CONTROL;
        case INPUT_KEY_RIGHT_ALT:
            return GLFW_KEY_RIGHT_ALT;
        case INPUT_KEY_RIGHT_SUPER:
            return GLFW_KEY_RIGHT_SUPER;
        case INPUT_KEY_MENU:
            return GLFW_KEY_MENU;
        default:
            return GLFW_KEY_UNKNOWN;
    }
}

PRIVATE input_key_t _glfw_to_input_key(int key) {
    switch (key) {
        case GLFW_KEY_SPACE:
            return INPUT_KEY_SPACE;
        case GLFW_KEY_APOSTROPHE:
            return INPUT_KEY_APOSTROPHE;
        case GLFW_KEY_COMMA:
            return INPUT_KEY_COMMA;
        case GLFW_KEY_MINUS:
            return INPUT_KEY_MINUS;
        case GLFW_KEY_PERIOD:
            return INPUT_KEY_PERIOD;
        case GLFW_KEY_SLASH:
            return INPUT_KEY_SLASH;
        case GLFW_KEY_0:
            return INPUT_KEY_0;
        case GLFW_KEY_1:
            return INPUT_KEY_1;
        case GLFW_KEY_2:
            return INPUT_KEY_2;
        case GLFW_KEY_3:
            return INPUT_KEY_3;
        case GLFW_KEY_4:
            return INPUT_KEY_4;
        case GLFW_KEY_5:
            return INPUT_KEY_5;
        case GLFW_KEY_6:
            return INPUT_KEY_6;
        case GLFW_KEY_7:
            return INPUT_KEY_7;
        case GLFW_KEY_8:
            return INPUT_KEY_8;
        case GLFW_KEY_9:
            return INPUT_KEY_9;
        case GLFW_KEY_SEMICOLON:
            return INPUT_KEY_SEMICOLON;
        case GLFW_KEY_EQUAL:
            return INPUT_KEY_EQUAL;
        case GLFW_KEY_A:
            return INPUT_KEY_A;
        case GLFW_KEY_B:
            return INPUT_KEY_B;
        case GLFW_KEY_C:
            return INPUT_KEY_C;
        case GLFW_KEY_D:
            return INPUT_KEY_D;
        case GLFW_KEY_E:
            return INPUT_KEY_E;
        case GLFW_KEY_F:
            return INPUT_KEY_F;
        case GLFW_KEY_G:
            return INPUT_KEY_G;
        case GLFW_KEY_H:
            return INPUT_KEY_H;
        case GLFW_KEY_I:
            return INPUT_KEY_I;
        case GLFW_KEY_J:
            return INPUT_KEY_J;
        case GLFW_KEY_K:
            return INPUT_KEY_K;
        case GLFW_KEY_L:
            return INPUT_KEY_L;
        case GLFW_KEY_M:
            return INPUT_KEY_M;
        case GLFW_KEY_N:
            return INPUT_KEY_N;
        case GLFW_KEY_O:
            return INPUT_KEY_O;
        case GLFW_KEY_P:
            return INPUT_KEY_P;
        case GLFW_KEY_Q:
            return INPUT_KEY_Q;
        case GLFW_KEY_R:
            return INPUT_KEY_R;
        case GLFW_KEY_S:
            return INPUT_KEY_S;
        case GLFW_KEY_T:
            return INPUT_KEY_T;
        case GLFW_KEY_U:
            return INPUT_KEY_U;
        case GLFW_KEY_V:
            return INPUT_KEY_V;
        case GLFW_KEY_W:
            return INPUT_KEY_W;
        case GLFW_KEY_X:
            return INPUT_KEY_X;
        case GLFW_KEY_Y:
            return INPUT_KEY_Y;
        case GLFW_KEY_Z:
            return INPUT_KEY_Z;
        case GLFW_KEY_LEFT_BRACKET:
            return INPUT_KEY_LEFT_BRACKET;
        case GLFW_KEY_BACKSLASH:
            return INPUT_KEY_BACKSLASH;
        case GLFW_KEY_RIGHT_BRACKET:
            return INPUT_KEY_RIGHT_BRACKET;
        case GLFW_KEY_GRAVE_ACCENT:
            return INPUT_KEY_GRAVE_ACCENT;
        case GLFW_KEY_WORLD_1:
            return INPUT_KEY_WORLD_1;
        case GLFW_KEY_WORLD_2:
            return INPUT_KEY_WORLD_2;
        case GLFW_KEY_ESCAPE:
            return INPUT_KEY_ESCAPE;
        case GLFW_KEY_ENTER:
            return INPUT_KEY_ENTER;
        case GLFW_KEY_TAB:
            return INPUT_KEY_TAB;
        case GLFW_KEY_BACKSPACE:
            return INPUT_KEY_BACKSPACE;
        case GLFW_KEY_INSERT:
            return INPUT_KEY_INSERT;
        case GLFW_KEY_DELETE:
            return INPUT_KEY_DELETE;
        case GLFW_KEY_RIGHT:
            return INPUT_KEY_RIGHT;
        case GLFW_KEY_LEFT:
            return INPUT_KEY_LEFT;
        case GLFW_KEY_DOWN:
            return INPUT_KEY_DOWN;
        case GLFW_KEY_UP:
            return INPUT_KEY_UP;
        case GLFW_KEY_PAGE_UP:
            return INPUT_KEY_PAGE_UP;
        case GLFW_KEY_PAGE_DOWN:
            return INPUT_KEY_PAGE_DOWN;
        case GLFW_KEY_HOME:
            return INPUT_KEY_HOME;
        case GLFW_KEY_END:
            return INPUT_KEY_END;
        case GLFW_KEY_CAPS_LOCK:
            return INPUT_KEY_CAPS_LOCK;
        case GLFW_KEY_SCROLL_LOCK:
            return INPUT_KEY_SCROLL_LOCK;
        case GLFW_KEY_NUM_LOCK:
            return INPUT_KEY_NUM_LOCK;
        case GLFW_KEY_PRINT_SCREEN:
            return INPUT_KEY_PRINT_SCREEN;
        case GLFW_KEY_PAUSE:
            return INPUT_KEY_PAUSE;
        case GLFW_KEY_F1:
            return INPUT_KEY_F1;
        case GLFW_KEY_F2:
            return INPUT_KEY_F2;
        case GLFW_KEY_F3:
            return INPUT_KEY_F3;
        case GLFW_KEY_F4:
            return INPUT_KEY_F4;
        case GLFW_KEY_F5:
            return INPUT_KEY_F5;
        case GLFW_KEY_F6:
            return INPUT_KEY_F6;
        case GLFW_KEY_F7:
            return INPUT_KEY_F7;
        case GLFW_KEY_F8:
            return INPUT_KEY_F8;
        case GLFW_KEY_F9:
            return INPUT_KEY_F9;
        case GLFW_KEY_F10:
            return INPUT_KEY_F10;
        case GLFW_KEY_F11:
            return INPUT_KEY_F11;
        case GLFW_KEY_F12:
            return INPUT_KEY_F12;
        case GLFW_KEY_F13:
            return INPUT_KEY_F13;
        case GLFW_KEY_F14:
            return INPUT_KEY_F14;
        case GLFW_KEY_F15:
            return INPUT_KEY_F15;
        case GLFW_KEY_F16:
            return INPUT_KEY_F16;
        case GLFW_KEY_F17:
            return INPUT_KEY_F17;
        case GLFW_KEY_F18:
            return INPUT_KEY_F18;
        case GLFW_KEY_F19:
            return INPUT_KEY_F19;
        case GLFW_KEY_F20:
            return INPUT_KEY_F20;
        case GLFW_KEY_F21:
            return INPUT_KEY_F21;
        case GLFW_KEY_F22:
            return INPUT_KEY_F22;
        case GLFW_KEY_F23:
            return INPUT_KEY_F23;
        case GLFW_KEY_F24:
            return INPUT_KEY_F24;
        case GLFW_KEY_F25:
            return INPUT_KEY_F25;
        case GLFW_KEY_KP_0:
            return INPUT_KEY_KP_0;
        case GLFW_KEY_KP_1:
            return INPUT_KEY_KP_1;
        case GLFW_KEY_KP_2:
            return INPUT_KEY_KP_2;
        case GLFW_KEY_KP_3:
            return INPUT_KEY_KP_3;
        case GLFW_KEY_KP_4:
            return INPUT_KEY_KP_4;
        case GLFW_KEY_KP_5:
            return INPUT_KEY_KP_5;
        case GLFW_KEY_KP_6:
            return INPUT_KEY_KP_6;
        case GLFW_KEY_KP_7:
            return INPUT_KEY_KP_7;
        case GLFW_KEY_KP_8:
            return INPUT_KEY_KP_8;
        case GLFW_KEY_KP_9:
            return INPUT_KEY_KP_9;
        case GLFW_KEY_KP_DECIMAL:
            return INPUT_KEY_KP_DECIMAL;
        case GLFW_KEY_KP_DIVIDE:
            return INPUT_KEY_KP_DIVIDE;
        case GLFW_KEY_KP_MULTIPLY:
            return INPUT_KEY_KP_MULTIPLY;
        case GLFW_KEY_KP_SUBTRACT:
            return INPUT_KEY_KP_SUBTRACT;
        case GLFW_KEY_KP_ADD:
            return INPUT_KEY_KP_ADD;
        case GLFW_KEY_KP_ENTER:
            return INPUT_KEY_KP_ENTER;
        case GLFW_KEY_KP_EQUAL:
            return INPUT_KEY_KP_EQUAL;
        case GLFW_KEY_LEFT_SHIFT:
            return INPUT_KEY_LEFT_SHIFT;
        case GLFW_KEY_LEFT_CONTROL:
            return INPUT_KEY_LEFT_CONTROL;
        case GLFW_KEY_LEFT_ALT:
            return INPUT_KEY_LEFT_ALT;
        case GLFW_KEY_LEFT_SUPER:
            return INPUT_KEY_LEFT_SUPER;
        case GLFW_KEY_RIGHT_SHIFT:
            return INPUT_KEY_RIGHT_SHIFT;
        case GLFW_KEY_RIGHT_CONTROL:
            return INPUT_KEY_RIGHT_CONTROL;
        case GLFW_KEY_RIGHT_ALT:
            return INPUT_KEY_RIGHT_ALT;
        case GLFW_KEY_RIGHT_SUPER:
            return INPUT_KEY_RIGHT_SUPER;
        case GLFW_KEY_MENU:
            return INPUT_KEY_MENU;
        default:
            return INPUT_KEY_UNKNOWN;
    }
}

void _input_glfw_key_callback(GLFWwindow* glfw_window, int glfw_key, int glfw_scancode, int glfw_action, int glfw_mods) {
    // We don't process repeat actions
    if (glfw_action == GLFW_REPEAT) {
        return;
    }

    app_t* app = glfwGetWindowUserPointer(glfw_window);

    input_key_t key = _glfw_to_input_key(glfw_key);
    PANIC_ASSERT(key >= 0 && key < INPUT_KEY_COUNT, "Invalid input_key");

    app->_input->key_states[key] = glfw_action == GLFW_PRESS ? INPUT_BUTTON_STATE_JUST_PRESSED : INPUT_BUTTON_STATE_JUST_RELEASED;
}

void _input_init(app_t* app) {
    // adm_arena will guarentee allocated memory is zeroed out so our button buffers should all be INPUT_BUTTON_STATE_RELEASED aka 0
    app->_input = arena_alloc(app->_arena, input_t);

    glfwSetKeyCallback(app->_window, _input_glfw_key_callback);
}

// Expected to be called before glfwPollEvents
void _input_frame(app_t* app) {
    for (input_button_state_t key = 0; key < INPUT_KEY_COUNT; key++) {
        if (app->_input->key_states[key] == INPUT_BUTTON_STATE_JUST_PRESSED) {
            app->_input->key_states[key] = INPUT_BUTTON_STATE_PRESSED;
        } else if (app->_input->key_states[key] == INPUT_BUTTON_STATE_JUST_RELEASED) {
            app->_input->key_states[key] = INPUT_BUTTON_STATE_RELEASED;
        }
    }
}

input_button_state_t input_key_state(app_t* app, input_key_t key) {
    PANIC_ASSERT(key >= 0 && key < INPUT_KEY_COUNT, "Invalid input_key");
    return app->_input->key_states[key];
}

bool input_key_down(app_t* app, input_key_t key) {
    input_button_state_t state = input_key_state(app, key);
    return state == INPUT_BUTTON_STATE_JUST_PRESSED || INPUT_BUTTON_STATE_PRESSED;
}

bool input_key_up(app_t* app, input_key_t key) {
    return !input_key_down(app, key);
}

bool input_key_just_pressed(app_t* app, input_key_t key) {
    return input_key_state(app, key) == INPUT_BUTTON_STATE_JUST_PRESSED;
}

bool input_key_just_released(app_t* app, input_key_t key) {
    return input_key_state(app, key) == INPUT_BUTTON_STATE_JUST_RELEASED;
}