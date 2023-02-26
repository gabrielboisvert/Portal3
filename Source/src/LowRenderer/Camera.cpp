#include "../../include/LowRenderer/Camera.h"
#include "../../Source/include/Application.h"

LowRenderer::Camera::Camera(const float width, const float height, const lm::mat4& projection, const lm::vec3& position) : projection(projection)
{
    this->inGame.position = this->editor.position = position;

    this->lastX = width * 0.5f;
    this->lastY = height * 0.5f;
}

LowRenderer::Camera::Camera(const float width, const float height, const lm::mat4& projection, const Camera& oldCam) : projection(projection)
{
    this->lastX = width * 0.5f;
    this->lastY = height * 0.5f;

    this->inGame.position = oldCam.inGame.position;
    this->inGame.yaw = oldCam.inGame.yaw;
    this->inGame.pitch = oldCam.inGame.pitch;


    this->editor.position = oldCam.editor.position;
    this->editor.yaw = oldCam.editor.yaw;
    this->editor.pitch = oldCam.editor.pitch;
    
    this->currentPosition = (Application::state == Application::E_PLAY_MODE? &this->inGame.position : &this->editor.position);
    this->yaw = (Application::state == Application::E_PLAY_MODE? &this->inGame.yaw : &this->editor.yaw);
    this->pitch = (Application::state == Application::E_PLAY_MODE? &this->inGame.pitch : &this->editor.pitch);

    this->forward = oldCam.forward;
    this->right = oldCam.right;
    this->up = oldCam.up;

    this->isFocus = oldCam.isFocus;
}

lm::vec3& LowRenderer::Camera::getPosition()
{
	return *this->currentPosition;
}

lm::vec3 LowRenderer::Camera::calculateFront()
{
    lm::vec3 front;
    front.X() = cos(*this->yaw * (M_PI / HALF_CIRCLE)) * (Application::state == Application::E_EDITOR_MODE ? cos(*this->pitch * (M_PI / HALF_CIRCLE)) : 1);
    front.Y() = (Application::state == Application::E_EDITOR_MODE ? sin(*this->pitch * (M_PI / HALF_CIRCLE)) : 0);
    front.Z() = sin(*this->yaw * (M_PI / HALF_CIRCLE)) * (Application::state == Application::E_EDITOR_MODE ? cos(*this->pitch * (M_PI / HALF_CIRCLE)) : 1);
    front.normalize();
    return front;
}

lm::mat4& LowRenderer::Camera::getView()
{
	return this->view;
}

lm::mat4& LowRenderer::Camera::getVP()
{
	return this->vp;
}

lm::mat4& LowRenderer::Camera::getProjection()
{
    return this->projection;
}

float& LowRenderer::Camera::getSpeed()
{
    return this->speed;
}

bool& LowRenderer::Camera::isRunning()
{
    return this->isRun;
}

void LowRenderer::Camera::setMousePosition(float mouseX, float mouseY)
{
    if (!this->isFocus)
        return;

    if (this->firstMouse)
    {
        lastX = mouseX;
        lastY = mouseY;
        this->firstMouse = false;
    }

    float xoffset = mouseX - lastX;
    float yoffset = lastY - mouseY;
    lastX = mouseX;
    lastY = mouseY;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    *yaw += xoffset;
    *pitch += yoffset;

    if (*pitch > 90.0f)
        *pitch = 90.0f;
    if (*pitch < -90.0f)
        *pitch = -90.0f;
    
    this->updateCameraVectors();
}

void LowRenderer::Camera::setShift(bool isShift)
{
    this->isRun = isShift;
}

float LowRenderer::Camera::getLastX()
{
    return this->lastX;
}

float LowRenderer::Camera::getLastY()
{
    return this->lastY;
}

float& LowRenderer::Camera::getYaw()
{
    return *this->yaw;
}

float& LowRenderer::Camera::getPitch()
{
    return *this->pitch;
}

bool LowRenderer::Camera::hasFocus()
{
    return this->isFocus;
}

void LowRenderer::Camera::setFocus(bool isFocus)
{
    this->isFocus = isFocus;
}

void LowRenderer::Camera::switchCam()
{
    this->currentPosition = (Application::state == Application::E_PLAY_MODE ? &this->inGame.position : &this->editor.position);
    this->yaw = (Application::state == Application::E_PLAY_MODE ? &this->inGame.yaw : &this->editor.yaw);
    this->pitch = (Application::state == Application::E_PLAY_MODE ? &this->inGame.pitch : &this->editor.pitch);
    this->forward = (Application::state == Application::E_PLAY_MODE ? &this->inGame.forward : &this->editor.forward);
    this->right = (Application::state == Application::E_PLAY_MODE ? &this->inGame.right : &this->editor.right);
    this->up = (Application::state == Application::E_PLAY_MODE ? &this->inGame.up : &this->editor.up);
    this->updateCameraVectors();
}

void LowRenderer::Camera::changeViewPortSize(const lm::mat4& projection)
{
    this->projection = projection;
}

lm::vec3& LowRenderer::Camera::getInGamePosition()
{
    return this->inGame.position;
}

lm::vec3& LowRenderer::Camera::getforward()
{
    return *forward;
}

lm::vec3& LowRenderer::Camera::getInGameForward()
{
    return this->inGame.forward;
}

lm::vec3& LowRenderer::Camera::getInGameRight()
{
    return this->inGame.right;
}

float& LowRenderer::Camera::getInGameRotationPitch()
{
    return this->inGame.pitch;
}

float& LowRenderer::Camera::getInGameRotationYaw()
{
    return this->inGame.yaw;
}

void LowRenderer::Camera::updateCameraVectors()
{
    lm::vec3 front;
    front.X() = cos(*this->yaw * (M_PI / HALF_CIRCLE)) * cos(*this->pitch * (M_PI / HALF_CIRCLE));
    front.Y() = sin(*this->pitch * (M_PI / HALF_CIRCLE));
    front.Z() = sin(*this->yaw * (M_PI / HALF_CIRCLE)) * cos(*this->pitch * (M_PI / HALF_CIRCLE));
    *this->forward = front.normalized();

    *this->right = this->forward->crossProduct(lm::vec3::up).normalized();
    *this->up = this->right->crossProduct(*this->forward).normalized();
}

void LowRenderer::Camera::processKeyboard(LowRenderer::Movement direction, float deltaTime)
{
    float velocity = this->speed * (this->isRun? 3: 1) * deltaTime;
    
    lm::vec3 front = this->calculateFront();

    if (direction == LowRenderer::Movement::FORWARD)
        *this->currentPosition += front * velocity;
    if (direction == LowRenderer::Movement::BACKWARD)
        *this->currentPosition -= front * velocity;
    if (direction == LowRenderer::Movement::LEFT)
        *this->currentPosition -= *this->right * velocity;
    if (direction == LowRenderer::Movement::RIGHT)
        *this->currentPosition += *this->right * velocity;
}

void LowRenderer::Camera::updatePos()
{
	this->view = lm::mat4::lookAt(*this->currentPosition, *this->currentPosition + *this->forward, *this->up);
	this->vp = this->projection * this->view;
}

