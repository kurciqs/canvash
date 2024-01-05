#include "shader.h"

const char *load_file(const char *path)
{
    FILE *file = fopen(path, "rb");

    if (file == NULL)
    {
        fprintf(stderr, "[ERROR] failed to open shader %s.\n", path);
        return 0;
    }

    fseek(file, 0, SEEK_END);
    unsigned long length = ftell(file);
    char *data = (char *)malloc(sizeof(char) * (length + 1));
    memset(data, 0, length + 1);
    fseek(file, 0, SEEK_SET);
    fread(data, 1, length, file);
    fclose(file);

    return data;
}

// error checking
void shader_compile_errors(unsigned int shader, const char* type, const char* file){
    // boolean to make sure to only check if it didn't compile
    GLint hasCompiled;
    // space for the log
    char infoLog[1024];
    // if was program we want to check for linking errors
    if (strcmp(type, "PROGRAM") != 0) {
        // get if even has compiled
        glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE) {
            // get da info
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            fprintf(stderr, "[ERROR] shader compilation error for: %s\n", type);
            fprintf(stderr, "[ERROR] shader program (%s) failed to compile:\n%s\n", file, infoLog);
        }
    }
    else {
        // this time it's for the program
        glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE) {
            // put it into infoLog
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            fprintf(stderr, "[ERROR] shader linking error for: %s\n", type);
            fprintf(stderr, "[ERROR] shader linking failed:\n%s\n", infoLog);
        }
    }
}

// Constructor that build the Shader Program from 2 different shaders
Shader create_shader(const char* vertexFile, const char* fragmentFile)
{
    // Read vertexFile and fragmentFile and store the strings
    const char* vertexCode = load_file(vertexFile);
    const char* fragmentCode = load_file(fragmentFile);

    // Convert the shader source strings into character arrays
    const char* vertexSource = vertexCode;
    const char* fragmentSource = fragmentCode;

    // Create Vertex Shader Object and get its reference (OpenGL remembers it)
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // Attach Vertex Shader source to the Vertex Shader Object (1 for string length)
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    // Compile the Vertex Shader into machine code
    glCompileShader(vertexShader);
    shader_compile_errors(vertexShader, "VERTEX", vertexFile);

    // Create Fragment Shader Object and get its reference
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // Attach Fragment Shader source to the Fragment Shader Object (1 for string length)
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    // Compile the Vertex Shader into machine code
    glCompileShader(fragmentShader);
    shader_compile_errors(fragmentShader, "FRAGMENT", fragmentFile);

    // Create Shader Program Object and get its reference
    Shader ID = glCreateProgram();
    // Attach the Vertex and Fragment Shaders to the Shader Program
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    // Wrap-up/Link all the shaders together into the Shader Program
    glLinkProgram(ID);
    shader_compile_errors(ID, "PROGRAM", "mfw");

    // Delete the now useless Vertex and Fragment Shader objects  (they are now connected to the ShaderProgram)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return ID;
}


void activate_shader(Shader ID){
    //just use it
    glUseProgram(ID);
}

void deactivate_shader(){
    //just don't use it
    activate_shader(NO_SHADER);
}

void delete_shader(Shader ID){
    //del
    glDeleteProgram(ID);
}

void shader_upload_v4(Shader ID, const char *varName, const float *v) {
    glUniform4f(glGetUniformLocation(ID,  varName), v[0], v[1], v[2], v[3]);
}

void shader_upload_v3(Shader ID, const char *varName, const float *v) {
    glUniform3f(glGetUniformLocation(ID,  varName), v[0], v[1], v[2]);
}

void shader_upload_v2(Shader ID, const char *varName, const float *v) {
    glUniform2f(glGetUniformLocation(ID,  varName), v[0], v[1]);
}

void shader_upload_f(Shader ID, const char *varName, const float f) {
    glUniform1f(glGetUniformLocation(ID,  varName), f);
}

void shader_upload_iv4(Shader ID, const char *varName, const int *v) {
    glUniform4i(glGetUniformLocation(ID,  varName), v[0], v[1], v[2], v[3]);
}

void shader_upload_iv3(Shader ID, const char *varName, const int *v) {
    glUniform3i(glGetUniformLocation(ID,  varName), v[0], v[1], v[2]);
}

void shader_upload_iv2(Shader ID, const char *varName, const int *v) {
    glUniform2i(glGetUniformLocation(ID,  varName), v[0], v[1]);
}

void shader_upload_i(Shader ID, const char *varName, const int v) {
    glUniform1i(glGetUniformLocation(ID,  varName), v);
}

void shader_upload_ui(Shader ID, const char *varName, const unsigned int v) {
    glUniform1ui(glGetUniformLocation(ID,  varName), v);
}

void shader_upload_iarr(Shader ID, const char *varName, const int *arr, int size) {
    glUniform1iv(glGetUniformLocation(ID,  varName), size, arr);
}

void shader_upload_b(Shader ID, const char *varName, const bool v) {
    glUniform1i(glGetUniformLocation(ID,  varName), v ? 1 : 0);
}

void shader_upload_m4(Shader ID, const char *varName, vec4 const *m) {
    glUniformMatrix4fv(glGetUniformLocation(ID,  varName), 1, GL_FALSE, &m[0][0]);
}

void shader_upload_m3(Shader ID, const char *varName, vec3 const *m) {
    glUniformMatrix3fv(glGetUniformLocation(ID,  varName), 1, GL_FALSE, &m[0][0]);
}