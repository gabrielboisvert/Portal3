#pragma once
#include "LinearMath/btIDebugDraw.h"
#include "../../Resources/Shader.h"
#include <GLFW/glfw3.h>

namespace Core
{
	class PhysicDebugDrawer : public btIDebugDraw
	{
	public:
		PhysicDebugDrawer(GLFWwindow* window);
		~PhysicDebugDrawer();

		void flushLines() override;

		void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
		void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;
		void reportErrorWarning(const char* warningString) override;

		void draw3dText(const btVector3& location, const char* textString) override;

		void setDebugMode(int debugMode) override;

		int getDebugMode() const override;

	private:
		struct DebugVertex
		{
			DebugVertex() {};
			DebugVertex(float x, float y, float z, float r, float g, float b)
			{
				position[0] = x;
				position[1] = y;
				position[2] = z;

				color[0] = r;
				color[1] = g;
				color[2] = b;
			}

			float position[3];
			float color[3];
		};

		int debugMode = 1;

		unsigned int VBO;
		unsigned int VAO;
		Resources::Shader* shader;
		GLFWwindow* window = nullptr;

		std::vector<DebugVertex> lineVertice;
	};
}