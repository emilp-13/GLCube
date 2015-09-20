#include "cube.h"

#include <QSurfaceFormat>
#include <QSurface>
#include <QVector3D>
#include <QVector4D>
#include <QDebug>

using namespace std;

struct Vertex
{
    QVector3D position;
    QVector4D color;
};

Cube::Cube()
    : QOpenGLWindow()
{
    this->setSurfaceType(QSurface::OpenGLSurface);

    this->setWidth(640);
    this->setHeight(480);

    QSurfaceFormat surfaceFormat;
    surfaceFormat.setVersion(4, 4);
    surfaceFormat.setProfile(QSurfaceFormat::CompatibilityProfile);
    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);

    this->m_pContext = new QOpenGLContext(this);
    this->m_pContext->setFormat(surfaceFormat);
    this->m_pContext->create();

    this->m_pContext->makeCurrent(this);

    this->m_mtxView.setToIdentity();
    this->m_mtxViewProj.setToIdentity();
    this->m_mtxWorldViewProj.setToIdentity();
}

Cube::~Cube()
{

}

void Cube::updateGLScene()
{

}

void Cube::initializeGL()
{
    this->initializeOpenGLFunctions();

    this->glViewport(0, 0, this->width(), this->height());

    this->buildBuffers();

    this->manageShaders();

    this->m_mtxView.lookAt(QVector3D(0.0f, 0.0f, 2.0f), QVector3D(0.0f, 0.0f, 0.0f),
                           QVector3D(0.0f, 1.0f, 0.0f));

    QMatrix4x4 mtxProj;
    mtxProj.setToIdentity();
    float aspectRatio = (float)this->width() / (float)this->height();
    mtxProj.perspective(45.0f, aspectRatio,
                        0.01f, 100.0f);
    this->m_mtxViewProj = mtxProj * this->m_mtxView;

    this->glEnable(GL_DEPTH_TEST);
    this->glDepthFunc(GL_LESS);

    this->glEnable(GL_CULL_FACE);
}

void Cube::paintGL()
{
    this->glClearColor(0.55f, 0.55f, 0.55f, 1.0f);
    this->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 mtxWorld;
    mtxWorld.setToIdentity();
    mtxWorld.translate(320.0f, 240.0f, 0.0f);

    this->m_mtxWorldViewProj = this->m_mtxViewProj * mtxWorld;

    this->m_pColorSP->bind();

    this->m_CubeVAO.bind();

    this->glUniformMatrix4fv(0, 1, GL_FALSE, this->m_mtxWorldViewProj.data());
    this->glDrawElements(GL_TRIANGLES, 36, GL_SHORT, 0);

    this->m_CubeVAO.release();

    this->m_pColorSP->release();
}

void Cube::resizeGL(int width, int height)
{
    this->glViewport(0, 0, width, height);

    QMatrix4x4 mtxProj;
    mtxProj.setToIdentity();
    float aspectRatio = (float)width / (float)height;
    mtxProj.perspective(45.0f, aspectRatio,
                        0.01f, 100.0f);
    this->m_mtxViewProj = mtxProj * this->m_mtxView;
}

void Cube::buildBuffers()
{
    Vertex cubeVertexData[] =
    {
        { QVector3D(+10.0f, +10.0f, +10.0f), QVector4D(1.0f, 0.0f, 0.0f, 1.0f) },
        { QVector3D(-10.0f, +10.0f, +10.0f), QVector4D(0.0f, 0.0f, 0.0f, 1.0f) },
        { QVector3D(-10.0f, -10.0f, +10.0f), QVector4D(1.0f, 1.0f, 1.0f, 1.0f) },
        { QVector3D(+10.0f, -10.0f, +10.0f), QVector4D(0.0f, 1.0f, 0.0f, 1.0f) },

        { QVector3D(+10.0f, -10.0f, -10.0f), QVector4D(1.0f, 0.0f, 1.0f, 1.0f) },
        { QVector3D(-10.0f, -10.0f, -10.0f), QVector4D(0.0f, 0.0f, 1.0f, 1.0f) },
        { QVector3D(-10.0f, +10.0f, -10.0f), QVector4D(1.0f, 1.0f, 0.0f, 1.0f) },
        { QVector3D(+10.0f, +10.0f, -10.0f), QVector4D(0.0f, 1.0f, 1.0f, 1.0f) },
    };

    short cubeIndices[] =
    {
        // front plane
        0, 1, 2,
        2, 3, 0,
        // back plane
        7, 4, 5,
        5, 6, 7,
        // left plane
        6, 5, 2,
        2, 1, 6,
        // right plane
        7, 0, 3,
        3, 4, 7,
        // top plane
        7, 6, 1,
        1, 0, 7,
        // bottom plane
        3, 2, 5,
        5, 4, 3,
    };

    this->m_CubeVAO.create();
    this->m_CubeVAO.bind();

    this->m_pCubeVB = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    this->m_pCubeVB->create();
    this->m_pCubeVB->setUsagePattern(QOpenGLBuffer::StaticDraw);
    this->m_pCubeVB->bind();
    this->m_pCubeVB->allocate(cubeVertexData, 8 * sizeof(Vertex));

    this->glEnableVertexAttribArray(0);
    this->glEnableVertexAttribArray(1);

    this->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                reinterpret_cast<void*>(offsetof(Vertex, position)));
    this->glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                reinterpret_cast<void*>(offsetof(Vertex, color)));

    this->m_pCubeVB->release();

    this->m_pCubeIB = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    this->m_pCubeIB->create();
    this->m_pCubeIB->setUsagePattern(QOpenGLBuffer::StaticDraw);
    this->m_pCubeIB->bind();
    this->m_pCubeIB->allocate(cubeIndices, 36 * sizeof(short));

    this->m_pCubeIB->release();

    this->m_CubeVAO.release();
}

void Cube::manageShaders()
{
    this->m_pColorVS = new QOpenGLShader(QOpenGLShader::Vertex, this);
    this->m_pColorVS->compileSourceFile(":/shaders/Color.vsh");

    this->m_pColorFS = new QOpenGLShader(QOpenGLShader::Fragment, this);
    this->m_pColorFS->compileSourceFile(":/shaders/Color.fsh");

    this->m_pColorSP = new QOpenGLShaderProgram(this);
    this->m_pColorSP->create();
    this->m_pColorSP->addShader(this->m_pColorVS);
    this->m_pColorSP->addShader(this->m_pColorFS);
    this->m_pColorSP->link();
}
