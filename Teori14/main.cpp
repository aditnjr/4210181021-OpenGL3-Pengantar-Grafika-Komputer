#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

//glm
#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//soil2
#include <SOIL2/SOIL2.h>

struct Vertex
{
  glm::vec3 position;
  glm::vec3 color;
  glm::vec2 texcoord;
};

Vertex vertices[] =
{
  //position                      //color                       //tex coords
  glm::vec3(-0.5f, 0.5f, 0.f),    glm::vec3(1.f, 0.f, 0.f),   glm::vec2(0.f, 1.f),
  glm::vec3(-0.5f, -0.5f, 0.f),   glm::vec3(0.f, 1.f, 0.f),   glm::vec2(0.f, 0.f),
  glm::vec3(0.5f, -0.5f, 0.f),    glm::vec3(0.f, 0.f, 1.f),   glm::vec2(1.f, 0.f),
  glm::vec3(0.5f, 0.5f, 0.f),     glm::vec3(1.f, 1.f, 0.f),   glm::vec2(1.f, 1.f)
};
unsigned nrOfVertices = sizeof(vertices) / sizeof(Vertex);

GLuint indices[] =
{
  0, 1, 2, //triangle 1
  0, 2, 3  //triangle 2
};
unsigned nrOfIndices = sizeof(indices) / sizeof(GLuint);

void updateInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
bool loadShaders(GLuint &program);

// settings
const unsigned int SCR_WIDTH = 640;
const unsigned int SCR_HEIGHT = 480;

int main()
{
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // glfw window creation
  // --------------------
  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL)
  {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  //Init glew (needs window and opengl context)
  glewExperimental = GL_TRUE;

  //Error
  if(glewInit() != GLEW_OK)
  {
    std::cout << "ERROR::MAIN.CPP::GLEW_INIT_FAILED" << "\n";
    glfwTerminate();
  }

  //OpenGL options
  glEnable(GL_DEPTH_TEST);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  //Init Shader
  GLuint core_program;
  if(!loadShaders(core_program))
    glfwTerminate();

  //Model

  //VAO, VBO, EBO --
  //Gen VAO and bind
  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  //Gen VBO and bind and send data
  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  //Gen EBO and bind and send data
  GLuint EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  //Set vertexAttribPointer and enable
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texcoord));
  glEnableVertexAttribArray(2);

  //Bind VAO 0
  glBindVertexArray(0);

  //Init Texture
  //Texture 0
  int image_width = 0;
  int image_height = 0;
  unsigned char* image = SOIL_load_image("Images/pusheen.png", &image_width, &image_height, NULL, SOIL_LOAD_RGBA);

  GLuint texture0;
  glGenTextures(1, &texture0);
  glBindTexture(GL_TEXTURE_2D, texture0);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  if(image)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
  {
    std::cout << "Error - Texture loading failed" << "\n";
  }

  glActiveTexture(0);
  glBindTexture(GL_TEXTURE_2D, 0);
  SOIL_free_image_data(image);

  //Texture 1

  int image_width1 = 0;
  int image_height1 = 0;
  unsigned char* image1 = SOIL_load_image("Images/album-thumbnail.png", &image_width1, &image_height1, NULL, SOIL_LOAD_RGBA);

  GLuint texture1;
  glGenTextures(1, &texture1);
  glBindTexture(GL_TEXTURE_2D, texture1);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  if(image1)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width1, image_height1, 0, GL_RGBA, GL_UNSIGNED_BYTE, image1);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
  {
    std::cout << "Error - Texture loading failed" << "\n";
  }

  glActiveTexture(0);
  glBindTexture(GL_TEXTURE_2D, 0);
  SOIL_free_image_data(image1);

  //Main loop
  while(!glfwWindowShouldClose(window))
  {
    //UPDATE --
    glfwPollEvents();

    //UPDATE --
    updateInput(window);

    //DRAW --
    //Clear
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    //Use a Program
    glUseProgram(core_program);

    //Update uniforms
    glUniform1i(glGetUniformLocation(core_program, "texture0"), 0);
    glUniform1i(glGetUniformLocation(core_program, "texture1"), 1);

    //Activate texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    //Bind vertex array object
    glBindVertexArray(VAO);

    //Draw
    //glDrawArrays(GL_TRIANGLES, 0, nrOfVertices);
    glDrawElements(GL_TRIANGLES, nrOfIndices, GL_UNSIGNED_INT, 0);

    //End Draw
    glfwSwapBuffers(window);
    glFlush();

    glBindVertexArray(0);
    glUseProgram(0);
    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  //End of program
  glfwDestroyWindow(window);
  glfwTerminate();

  //Delete Program
  glDeleteProgram(core_program);

  return 0;
}

void updateInput(GLFWwindow* window)
{
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

bool loadShaders(GLuint &program)
{
  bool loadSuccess = true;
  char infoLog[512];
  GLint success;

  std::string temp = "";
  std::string src = "";

  std::ifstream in_file;

  //Vertex
  in_file.open("vertex_core.glsl");

  if(in_file.is_open())
  {
    while(std::getline(in_file, temp))
      src += temp + "\n";
  }
  else
  {
    std::cout << "ERROR - loadShaders could not open vertex file" << "\n";
    loadSuccess = false;
  }

  in_file.close();

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  const GLchar* vertSrc = src.c_str();
  glShaderSource(vertexShader, 1, &vertSrc, NULL);
  glCompileShader(vertexShader);

  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if(!success)
  {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "Error - loadShaders could not compile vertex shader" << "\n";
    std::cout << infoLog << "\n";
  }

  temp = "";
  src = "";

  //Fragment
  in_file.open("fragment_core.glsl");

  if(in_file.is_open())
  {
    while(std::getline(in_file, temp))
      src += temp + "\n";
  }
  else
  {
    std::cout << "ERROR - loadShaders could not open fragment file" << "\n";
    loadSuccess = false;
  }

  in_file.close();

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  const GLchar* fragSrc = src.c_str();
  glShaderSource(fragmentShader, 1, &fragSrc, NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if(!success)
  {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "Error - loadShaders could not compile fragment shader" << "\n";
    std::cout << infoLog << "\n";
    loadSuccess = false;
  }

  //Program
  program = glCreateProgram();

  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);

  glLinkProgram(program);

  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if(!success)
  {
    glGetProgramInfoLog(program, 512, NULL, infoLog);
    std::cout << "Error - loadShaders could not link program" << "\n";
    std::cout << infoLog << "\n";
    loadSuccess = false;
  }

  //End
  glUseProgram(0);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return loadSuccess;
}
