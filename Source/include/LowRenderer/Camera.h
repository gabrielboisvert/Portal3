#pragma once
#include <LibMaths/Mat4/Mat4.h>

namespace LowRenderer
{
	enum class Movement
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UP,
		DOWN
	};
	class Camera
	{
		private:
			struct Config
			{
			public:
				lm::vec3 position;
				float yaw = -90;
				float pitch = 0;

				lm::vec3 forward = lm::vec3::forward;
				lm::vec3 right = lm::vec3::right;
				lm::vec3 up = lm::vec3::up;
			};

			Config editor;
			Config inGame;

			lm::vec3* currentPosition = &inGame.position;
			float* yaw = &inGame.yaw;
			float* pitch = &inGame.pitch;
			lm::vec3* forward = &inGame.forward;
			lm::vec3* right = &inGame.right;
			lm::vec3* up = &inGame.up;
			
			lm::mat4 view;
			
			lm::mat4 projection;
			lm::mat4 vp;
			
			float speed = 30;
			bool firstMouse = true;

			float lastX = 0;
			float lastY = 0;

			float mouseX = 0;
			float mouseY = 0;

			bool isRun = false;
			bool isFocus = true;

		public:
			Camera() {};
			Camera(const float width, const float height, const lm::mat4& projection, const lm::vec3& position);
			Camera(const float width, const float height, const lm::mat4& projection, const Camera& oldCam);
			lm::vec3& getPosition();
			lm::vec3 calculateFront();
			lm::mat4& getView();
			lm::mat4& getVP();
			lm::mat4& getProjection();
			float& getSpeed();
			bool& isRunning();

			void LowRenderer::Camera::updateCameraVectors();
			void Camera::processKeyboard(LowRenderer::Movement direction, float deltaTime);
			void setMousePosition(float mouseX, float mouseY);

			void setShift(bool isShift);

			float getLastX();
			float getLastY();
			float& getYaw();
			float& getPitch();
			bool hasFocus();
			void setFocus(bool isFocus);
			void switchCam();
			void changeViewPortSize(const lm::mat4& projection);
			lm::vec3& getInGamePosition();
			lm::vec3& getforward();
			lm::vec3& getInGameForward();
			lm::vec3& getInGameRight();
			float& getInGameRotationPitch();
			float& getInGameRotationYaw();
			void updatePos();

	
	};
}