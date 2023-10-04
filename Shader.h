#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL/glew.h>

class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    void Use();
    GLuint GetProgramID() const;

private:
    GLuint programID;
    std::string ReadFile(const std::string& filepath);
    void CompileShader(const std::string& vertexSource, const std::string& fragmentSource);
    void CheckCompileErrors(GLuint shader, const std::string& type);
};

#endif