#include "input.h"
#include "app.h"

#include <GLFW/glfw3.h>

PRIVATE int _input_key_to_glfw(input_key key) {
    // Keys are directly mapped to glfw keys
    return key;
}

bool input_key_down(app_t* app, input_key key) {
    return glfwGetKey(app->_window, _input_key_to_glfw(key)) == GLFW_PRESS;
}

bool input_key_up(app_t* app, input_key key) {
    return !input_key_down(app, key);
}