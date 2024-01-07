#ifndef CANVASH_DEV_SHADER_H
#define CANVASH_DEV_SHADER_H

#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <cglm/call.h>
#include <memory.h>
#include <string.h>

#define NO_SHADER 0
typedef GLuint Shader;


Shader create_shader(const char *vertexFile, const char *fragmentFile);

void activate_shader(Shader ID);

void deactivate_shader();

void shader_upload_v4(Shader ID, const char *varName, const vec4 v);

void shader_upload_v3(Shader ID, const char *varName, const vec3 v);

void shader_upload_v2(Shader ID, const char *varName, const vec2 v);

void shader_upload_f(Shader ID, const char *varName, float f);

void shader_upload_iv4(Shader ID, const char *varName, const ivec4 v);

void shader_upload_iv3(Shader ID, const char *varName, const ivec3 v);

void shader_upload_iv2(Shader ID, const char *varName, const ivec2 v);

void shader_upload_i(Shader ID, const char *varName, int v);

void shader_upload_ui(Shader ID, const char *varName, unsigned int v);

void shader_upload_iarr(Shader ID, const char *varName, const int *arr, int size);

void shader_upload_b(Shader ID, const char *varName, bool v);

void shader_upload_m4(Shader ID, const char *varName, const mat4 m);

void shader_upload_m3(Shader ID, const char *varName, const mat3 m);

void delete_shader(Shader ID);

void shader_compile_errors(GLuint shader, const char *type, const char *file);

#endif //CANVASH_DEV_SHADER_H
