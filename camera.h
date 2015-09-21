#ifndef CAMERA_H
#define CAMERA_H

#include <QVector3D>
#include <QMatrix4x4>

class Camera
{
public:
    Camera();

    void setPosition(const QVector3D position);
    void setViewDirection(const QVector3D viewDirection);
    void setUpDirection(const QVector3D upDirection);

    const QMatrix4x4 getViewMatrix() const;

private:
    QVector3D   m_Position;
    QVector3D   m_ViewDirection;
    QVector3D   m_UpDirection;
};

#endif // CAMERA_H
