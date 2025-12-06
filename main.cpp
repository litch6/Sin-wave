#define GL_VERSION_4_5
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <vector>
#include <iostream>

unsigned int WIDTH = 1280, HEIGHT = 720;

const char* VertexShadersource = "#version 450\n"
"layout (location = 0) in vec3 pos;\n"
"uniform mat4 model;\n"
"void main() {\n"
"gl_Position = model * vec4(pos, 1.0);\n"
"}";

const char* FragmentShaderSource = "#version 450\n"
"out vec4 Fragcolor;\n"
"uniform vec3 color;\n"
"void main(){\n"
"Fragcolor = vec4(color, 1.0);\n"
"}";

static void framebuffer_size_callback(GLFWwindow* traget, int width, int height) {
	glViewport(0, 0, width, height);
	WIDTH = static_cast<float>(width);
	HEIGHT = static_cast<float>(height);
}

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "SIN WAVE", NULL, NULL);
	if (!window) {
		std::cout << "WINDOW::ERROR";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "GLAD ERROR";
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, WIDTH, HEIGHT);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int success;
	char infolog[512];

	unsigned int VertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShader, 1, &VertexShadersource, NULL);
	glCompileShader(VertexShader);

	unsigned int FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShader, 1, &FragmentShaderSource, NULL);
	glCompileShader(FragmentShader);

	unsigned int ShaderID = glCreateProgram();
	glAttachShader(ShaderID, VertexShader);
	glAttachShader(ShaderID, FragmentShader);
	glLinkProgram(ShaderID);

	const unsigned int Gridsize = 20;
	std::vector<glm::vec3> GridVertices;

	float paperLeft = -0.7f;
	float paperRight = 0.7f;
	float paperBottom = -0.7f;
	float paperTop = 0.7f;
	const glm::vec3 Axis[2] = { glm::vec3(paperLeft,0.0,0.0),glm::vec3(paperRight,0.0,0.0) };
	float cellWidth = (paperRight - paperLeft) / Gridsize;
	float cellHeight = (paperTop - paperBottom) / Gridsize;
	for (int i = 0; i <= Gridsize; i++) {
		float y = paperBottom + (i / (float)Gridsize) * (paperTop - paperBottom);
		GridVertices.push_back(glm::vec3(paperLeft, y, 0.0f));
		GridVertices.push_back(glm::vec3(paperRight, y, 0.0f));
	}

	for (int i = 0; i <= Gridsize; i++) {
		float x = paperLeft + (i / (float)Gridsize) * (paperRight - paperLeft);
		GridVertices.push_back(glm::vec3(x, paperBottom, 0.0f));
		GridVertices.push_back(glm::vec3(x, paperTop, 0.0f));
	}

	//Lines Buffer
	unsigned int LVAO;
	unsigned int LVBO;

	glGenVertexArrays(1, &LVAO);
	glBindVertexArray(LVAO);

	glGenBuffers(1, &LVBO);
	glBindBuffer(GL_ARRAY_BUFFER, LVBO);
	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec3), Axis, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	const glm::mat4 Xaxis = glm::mat4(1.0);
	glm::mat4 Yaxis = glm::translate(Xaxis, glm::vec3(-0.5f, 0.0, 0.0));
	Yaxis = glm::rotate(Yaxis, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));

	// Grid Buffer
	unsigned int GridVAO;
	unsigned int GridVBO;

	glGenVertexArrays(1, &GridVAO);
	glBindVertexArray(GridVAO);

	glGenBuffers(1, &GridVBO);
	glBindBuffer(GL_ARRAY_BUFFER, GridVBO);
	glBufferData(GL_ARRAY_BUFFER, GridVertices.size() * sizeof(glm::vec3), GridVertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// sin wave
	unsigned int smooth = 100;
	std::vector<glm::vec3> SinWave;
	const float a = 10.0f;
	const float f = 4.5f;
	const float phase = 0.0f;

	float startX = -0.5f;
	float endX = paperRight;

	/*for (int i = 0; i < smooth; i++) {
		float x = startX + (i / (float)(smooth - 1)) * (endX - startX);
		float y = (a * cellHeight) * sin(f * x + phase);
		SinWave.push_back(glm::vec3(x, y, 0.0f));
	}*/

	for (int i = 0; i < smooth; i++) {
		float x = startX + (i / float(smooth - 1)) * (endX - startX);
		float height = (a * cellHeight) * sin(f * x + phase);      // ارتفاع الموجة
		float slope = (a * cellHeight) * f * cos(f * x + phase);   // مشتقة الموجة
		glm::vec2 tangent = glm::normalize(glm::vec2(1.0f, slope));
		glm::vec2 pos(x, height);
		SinWave.push_back(glm::vec3(pos + tangent * 0.07f, 0.0f));
		SinWave.push_back(glm::vec3(pos - tangent * 0.07f, 0.0f));  // Changed 0.f to 0.07f
	}


	unsigned int SinVAO, SinVBO;
	glGenVertexArrays(1, &SinVAO);
	glBindVertexArray(SinVAO);

	glGenBuffers(1, &SinVBO);
	glBindBuffer(GL_ARRAY_BUFFER, SinVBO);
	glBufferData(GL_ARRAY_BUFFER, SinWave.size() * sizeof(glm::vec3), SinWave.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
	glfwSwapInterval(1);
	while (!glfwWindowShouldClose(window)) {
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)glfwSetWindowShouldClose(window, true);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(ShaderID);
		unsigned int colorloc = glGetUniformLocation(ShaderID, "color");
		unsigned int modelloc = glGetUniformLocation(ShaderID, "model");

		glLineWidth(1.0f);
		glUniform3fv(colorloc, 1, glm::value_ptr(glm::vec3(0.2f, 0.25f, 0.3f)));
		glUniformMatrix4fv(modelloc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
		glBindVertexArray(GridVAO);
		glDrawArrays(GL_LINES, 0, GridVertices.size());
		glBindVertexArray(0);

		glLineWidth(2.5f);
		glUniform3fv(colorloc, 1, glm::value_ptr(glm::vec3(0.9f, 0.3f, 0.4f)));
		glUniformMatrix4fv(modelloc, 1, GL_FALSE, glm::value_ptr(Xaxis));
		glBindVertexArray(LVAO);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);

		glUniform3fv(colorloc, 1, glm::value_ptr(glm::vec3(0.3f, 0.6f, 0.9f)));
		glUniformMatrix4fv(modelloc, 1, GL_FALSE, glm::value_ptr(Yaxis));
		glBindVertexArray(LVAO);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);

		glLineWidth(2.0f);
		glUniform3fv(colorloc, 1, glm::value_ptr(glm::vec3(0.2f, 0.9f, 0.5f)));
		glUniformMatrix4fv(modelloc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
		glBindVertexArray(SinVAO);
		glDrawArrays(GL_LINE_STRIP, 0, SinWave.size());
		glBindVertexArray(0);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &GridVAO);
	glDeleteBuffers(1, &GridVBO);
	glDeleteVertexArrays(1, &LVAO);
	glDeleteBuffers(1, &LVBO);
	glDeleteProgram(ShaderID);
	glDeleteVertexArrays(1, &SinVAO);
	glDeleteBuffers(1, &SinVBO);

	glfwTerminate();

	return 0;
}