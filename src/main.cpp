#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <stdexcept>
#include <string>
#include <sstream>

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}


static bool GLLogCall(const char* func, const char* file, int line)
{
	while (auto error = glGetError()) {
		std::cout << file << ":" << func << ":" << line << " GL error " << error << std::endl;
		return false;
	}

	return true;
}

struct ShaderCode
{
	std::string vs;
	std::string fs;
};

static ShaderCode ParseShader(const std::string& shaderFile)
{
	std::ifstream stream(shaderFile);
	if (!stream.is_open()) {
		std::cout << "Shader is not available" << std::endl;
		throw std::runtime_error("Shader is not available");
	}

	enum class ShaderType
	{
		None = -1,
		Vertex,
		Fragment
	};

	std::string line;
	std::stringstream vs, fs;
	auto mode = ShaderType::None;
	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos) {
				mode = ShaderType::Vertex;
			}
			else if (line.find("fragment") != std::string::npos) {
				mode = ShaderType::Fragment;
			}
		}
		else {
			if (mode == ShaderType::Vertex) {
				vs << line << std::endl;
			}
			else if (mode == ShaderType::Fragment) {
				fs << line << std::endl;
			}
		}
	}

	std::cout << "Vertex Shader code: " << std::endl << vs.str() << std::endl;
	std::cout << "Fragment Shader code: " << std::endl << fs.str() << std::endl;
	return { vs.str(), fs.str() };
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		std::cout << "Escape pressed" << std::endl;
		glfwSetWindowShouldClose(window, true);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	std::cout << "Width " << width << " Height " << height << std::endl;
	//glViewport(0, 0, width, height);
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	auto id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = new char[length];
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << type << "Compile failed " << message << std::endl;
		delete []message;

		return 0;
	}

	return id;
}

static int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	auto program = glCreateProgram();
	auto vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	auto fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow* window = glfwCreateWindow(800, 800, "Opengl", NULL, NULL);
	if (window == NULL) {
		std::cout << "Window is NULL " << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	float vertices[] = {
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		 0.5f,  0.5f,
		 -0.5f,  0.5f,
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};
	unsigned int VAO; glGenVertexArrays(1, &VAO); glBindVertexArray(VAO);

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	GLuint index_buffer;
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	auto shaderCode = ParseShader("resources/shader/basic.shader");
	auto shader = CreateShader(shaderCode.vs, shaderCode.fs);
	glUseProgram(shader);

	int location = glGetUniformLocation(shader, "u_color");
	ASSERT(location != -1);
	glUniform4f(location, 0.7f, 0.5f, 0.7f, 1.0f);

	float r = 0.0f;
	float increment = 0.05f;
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);

		processInput(window);

		glUniform4f(location, r, 0.3f, 0.4f, 1.0f);
		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
		
		glfwSwapInterval(0.5);
		if (r > 1.0f) {
			r = 0.0f;
		}
		else {
			r += increment;
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteProgram(shader);

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
