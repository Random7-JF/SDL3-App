#pragma once
#include <string>
#include <unordered_map>

struct ShaderProgramSource {
  std::string VertexSource;
  std::string FragmentSource;
};

ShaderProgramSource ParseShader(const std::string &filepath);
unsigned int CompileShader(unsigned int type, const std::string &source);
unsigned int CreateShader(const std::string &vertexShader,
                          const std::string &fragmentShader);

class Shader {
  private:
    unsigned int m_rendererID;
    std::string m_filePath;
    std::unordered_map<std::string, int> m_uniformLocationCache;

  public:
    Shader(const std::string& filepath);
    ~Shader();

    void Bind() const;
    void Unbind() const;
    //set uniforms
    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
    void SetUniform1f(const std::string& name, float value);
    void SetUniform1i(const std::string& name, int value);

  private:
    int GetUnifromLocation(const std::string& name);
    unsigned int CompileShader(unsigned int type, const std::string &source);
    ShaderProgramSource ParseShader(const std::string &filepath);
    unsigned int CreateShader(const std::string &vertexShader, const std::string &fragmentShader);
};