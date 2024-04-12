#ifndef CANVASH_DEFINES_H
#define CANVASH_DEFINES_H

#define CANVASH_KEY_UNKNOWN          GLFW_KEY_UNKNOWN
#define CANVASH_KEY_SPACE            GLFW_KEY_SPACE
#define CANVASH_KEY_MINUS            GLFW_KEY_MINUS
#define CANVASH_KEY_PERIOD           GLFW_KEY_PERIOD
#define CANVASH_KEY_SLASH            GLFW_KEY_SLASH
#define CANVASH_KEY_0                GLFW_KEY_0
#define CANVASH_KEY_1                GLFW_KEY_1
#define CANVASH_KEY_2                GLFW_KEY_2
#define CANVASH_KEY_3                GLFW_KEY_3
#define CANVASH_KEY_4                GLFW_KEY_4
#define CANVASH_KEY_5                GLFW_KEY_5
#define CANVASH_KEY_6                GLFW_KEY_6
#define CANVASH_KEY_7                GLFW_KEY_7
#define CANVASH_KEY_8                GLFW_KEY_8
#define CANVASH_KEY_9                GLFW_KEY_9
#define CANVASH_KEY_SEMICOLON        GLFW_KEY_SEMICOLON
#define CANVASH_KEY_EQUAL            GLFW_KEY_EQUAL
#define CANVASH_KEY_A                GLFW_KEY_A
#define CANVASH_KEY_B                GLFW_KEY_B
#define CANVASH_KEY_C                GLFW_KEY_C
#define CANVASH_KEY_D                GLFW_KEY_D
#define CANVASH_KEY_E                GLFW_KEY_E
#define CANVASH_KEY_F                GLFW_KEY_F
#define CANVASH_KEY_G                GLFW_KEY_G
#define CANVASH_KEY_H                GLFW_KEY_H
#define CANVASH_KEY_I                GLFW_KEY_I
#define CANVASH_KEY_J                GLFW_KEY_J
#define CANVASH_KEY_K                GLFW_KEY_K
#define CANVASH_KEY_L                GLFW_KEY_L
#define CANVASH_KEY_M                GLFW_KEY_M
#define CANVASH_KEY_N                GLFW_KEY_N
#define CANVASH_KEY_O                GLFW_KEY_O
#define CANVASH_KEY_P                GLFW_KEY_P
#define CANVASH_KEY_Q                GLFW_KEY_Q
#define CANVASH_KEY_R                GLFW_KEY_R
#define CANVASH_KEY_S                GLFW_KEY_S
#define CANVASH_KEY_T                GLFW_KEY_T
#define CANVASH_KEY_U                GLFW_KEY_U
#define CANVASH_KEY_V                GLFW_KEY_V
#define CANVASH_KEY_W                GLFW_KEY_W
#define CANVASH_KEY_X                GLFW_KEY_X
#define CANVASH_KEY_Y                GLFW_KEY_Y
#define CANVASH_KEY_Z                GLFW_KEY_Z
#define CANVASH_KEY_F1               GLFW_KEY_F1
#define CANVASH_KEY_F2               GLFW_KEY_F2
#define CANVASH_KEY_F3               GLFW_KEY_F3
#define CANVASH_KEY_F4               GLFW_KEY_F4
#define CANVASH_KEY_F5               GLFW_KEY_F5
#define CANVASH_KEY_F6               GLFW_KEY_F6
#define CANVASH_KEY_F7               GLFW_KEY_F7
#define CANVASH_KEY_F8               GLFW_KEY_F8
#define CANVASH_KEY_F9               GLFW_KEY_F9
#define CANVASH_KEY_F10              GLFW_KEY_F10
#define CANVASH_KEY_F11              GLFW_KEY_F11
#define CANVASH_KEY_F12              GLFW_KEY_F12
#define CANVASH_KEY_ESCAPE           GLFW_KEY_ESCAPE
#define CANVASH_KEY_RIGHT_SHIFT      GLFW_KEY_RIGHT_SHIFT
#define CANVASH_KEY_RIGHT_CONTROL    GLFW_KEY_RIGHT_CONTROL
#define CANVASH_KEY_RIGHT_ALT        GLFW_KEY_RIGHT_ALT
#define CANVASH_KEY_RIGHT_SUPER      GLFW_KEY_RIGHT_SUPER
#define CANVASH_KEY_MENU             GLFW_KEY_MENU
#define CANVASH_KEY_TAB              GLFW_KEY_TAB
#define CANVASH_KEY_ENTER            GLFW_KEY_ENTER
#define CANVASH_KEY_RIGHT            GLFW_KEY_RIGHT
#define CANVASH_KEY_LEFT             GLFW_KEY_LEFT
#define CANVASH_KEY_DOWN             GLFW_KEY_DOWN
#define CANVASH_KEY_UP               GLFW_KEY_UP
#define CANVASH_KEY_IS_PRESSED(x, y, z) x == z && y == GLFW_PRESS
#define CANVASH_KEY_IS_RELEASED(x, y, z) x == z && y == GLFW_RELEASE

#define CANVASH_MOUSE_BUTTON_LEFT      GLFW_MOUSE_BUTTON_LEFT
#define CANVASH_MOUSE_BUTTON_RIGHT     GLFW_MOUSE_BUTTON_RIGHT
#define CANVASH_MOUSE_BUTTON_MIDDLE    GLFW_MOUSE_BUTTON_MIDDLE
#define CANVASH_BUTTON_IS_PRESSED(x, y, z) x == z && y == GLFW_PRESS
#define CANVASH_BUTTON_IS_RELEASED(x, y, z) x == z && y == GLFW_RELEASE

#define CANVASH_RANDOM_COLOR (vec4){(float)(rand() % 255) / 255.0f, (float)(rand() % 255) / 255.0f, (float)(rand() % 255) / 255.0f, 1.0f}
#define CANVASH_MAX_OBJECTS 1000
#endif //CANVASH_DEFINES_H