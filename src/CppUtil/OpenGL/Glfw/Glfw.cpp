#include <GLFW/Glfw.h>

#include <CppUtil/Basic/EventManager.h>
#include <CppUtil/Basic/GStorage.h>
#include <CppUtil/Basic/Operation.h>

#include <iostream>

using namespace CppUtil::OpenGL;
using namespace CppUtil::Basic;
using namespace std;

Glfw::Glfw():window(NULL){ }

//------------

void Glfw::Init(size_t width, size_t height, const string & title){
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//mac os
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//------------
	GenWindow(width, height, title);
	LoadGL();
	glViewport(0, 0, width, height);
	//------------
	glfwSetKeyCallback(window, [](GLFWwindow * window, int key, int scanCode, int state, int mods) {
		size_t kbState =
			(state == GLFW_PRESS ? EventManager::KEYBOARD_PRESS
				: (state == GLFW_REPEAT ? EventManager::KEYBOARD_REPEAT
					: (state == GLFW_RELEASE ? EventManager::KEYBOARD_RELEASE
						: 0)));
		if(kbState != 0)
			EventManager::GetInstance()->Response(key| kbState);
		EventManager::GetInstance()->Response(key | Basic::EventManager::KEYBOARD);
	});
	//------------
	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xPos, double yPos) {
		static float lastX = xPos, lastY = yPos;
		static float mousePos_XOffset, mousePos_YOffset;
		GStorage<float *>::GetInstance()->Register("mousePos_XOffset", &mousePos_XOffset);
		GStorage<float *>::GetInstance()->Register("mousePos_YOffset", &mousePos_YOffset);
		//------------
		mousePos_XOffset = xPos - lastX;
		mousePos_YOffset = lastY - yPos;
		lastX = xPos;
		lastY = yPos;
		EventManager::GetInstance()->Response(EventManager::MOUSE_MOUVE);
	});
	//------------
	glfwSetScrollCallback(window, [](GLFWwindow* window, double xOffset, double yOffset) {
		static float mouseScroll_YOffset;
		GStorage<float *>::GetInstance()->Register("mouseScroll_YOffset", &mouseScroll_YOffset);
		//------------
		mouseScroll_YOffset = yOffset;
		EventManager::GetInstance()->Response(EventManager::MOUSE_SCROLL);
	});
}

void Glfw::LockCursor() {
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Glfw::Terminate() { glfwTerminate(); }

GLFWwindow * Glfw::GetWindow() { return window; }

void Glfw::Run(Ptr<Operation> op) {
	if (window == NULL)
		Init();
	//------------
	bool loop = op != NULL;
	while (!glfwWindowShouldClose(window)) {
		if (loop) {
			op->Run();
			if (!op->IsHold())
				loop = false;
		}
	}
}

int Glfw::GetKey(int key) { return glfwGetKey(window, key); }

void Glfw::CloseWindow() { glfwSetWindowShouldClose(window, true); }

//------------

Glfw* Glfw::instance = new Glfw();

Glfw* Glfw::GetInstance(){ return instance; }

//------------

void Glfw::GenWindow(size_t width, size_t height, const string & title) {
	window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		Terminate();
		exit(1);
	}
	//------------
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);
	});
}

void Glfw::LoadGL() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit(1);
	}
}