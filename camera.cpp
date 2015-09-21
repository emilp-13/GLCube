#include "camera.h"

Camera::Camera()
{
    this->m_Position = QVector3D(0.0f, 0.0f, 0.0f);
    this->m_ViewDirection = QVector3D(0.0f, 0.0f, -1.0f);
    this->m_UpDirection = QVector3D(0.0f, 1.0f, 0.0f);
}

void Camera::setPosition(const QVector3D position)
{
    this->m_Position = position;
}

void Camera::setViewDirection(const QVector3D viewDirection)
{
    this->m_ViewDirection = viewDirection;
}

void Camera::setUpDirection(const QVector3D upDirection)
{
    this->m_UpDirection = upDirection;
}

const QMatrix4x4 Camera::getViewMatrix() const
{
    QMatrix4x4 mtxView;
    mtxView.setToIdentity();
    mtxView.lookAt(this->m_Position, this->m_Position + this->m_ViewDirection,
                   this->m_UpDirection);

    return mtxView;
}

