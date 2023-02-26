#include "../../../include/Core/Debug/PhysicDebugDrawer.h"
#include <glad/glad.h>
#include "../../../include/Application.h"

Core::PhysicDebugDrawer::PhysicDebugDrawer(GLFWwindow* window)
{
	this->shader = new Resources::Shader("debugShader", "Source/shaders/Vertex/PDebugVertex.glsl", "Source/shaders/Fragment/PDebugFragment.glsl");
	this->window = window;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), 0);
	glEnableVertexAttribArray(0);
																		//offsetof()
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Core::PhysicDebugDrawer::~PhysicDebugDrawer()
{
	delete this->shader;
}

void Core::PhysicDebugDrawer::flushLines()
{
	if (this->lineVertice.size() == 0)
		return;

	Application* app = (Application*)glfwGetWindowUserPointer(this->window);
	this->shader->activate(true);
	this->shader->activate(true);
	this->shader->setUniformMat4("vp", &(app->getCam().getVP()[0][0]));

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(DebugVertex) * this->lineVertice.size(), &lineVertice.front(), GL_STATIC_DRAW);
	glDrawArrays(GL_LINES, 0, this->lineVertice.size());
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	this->lineVertice.resize(0);
}


void Core::PhysicDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	this->lineVertice.emplace_back(DebugVertex(from.x(), from.y(), from.z(), color.x(), color.y(), color.z()));
	this->lineVertice.emplace_back(DebugVertex(to.x(), to.y(), to.z(), color.x(), color.y(), color.z()));
}

void Core::PhysicDebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
}

void Core::PhysicDebugDrawer::reportErrorWarning(const char* warningString)
{
}

void Core::PhysicDebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
}

void Core::PhysicDebugDrawer::setDebugMode(int debugMode)
{
	this->debugMode = debugMode;
}

int Core::PhysicDebugDrawer::getDebugMode() const
{
	return this->debugMode;
}
