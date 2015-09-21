#include "cube.h"

#include <QSurfaceFormat>
#include <QSurface>
#include <QVector3D>
#include <QVector4D>
#include <QMessageBox>
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
    surfaceFormat.setSamples(4);
    surfaceFormat.setSwapInterval(1);

    this->m_pContext = new QOpenGLContext(this);
    this->m_pContext->setFormat(surfaceFormat);
    this->m_pContext->create();

    this->m_pContext->makeCurrent(this);

    this->m_mtxRotation.setToIdentity();
    this->m_mtxViewProj.setToIdentity();
    this->m_mtxWorldViewProj.setToIdentity();

    connect(this, SIGNAL(frameSwapped()), this, SLOT(updateGLScene()));
}

Cube::~Cube()
{
    this->makeCurrent();

    this->m_pCubeIB->destroy();
    this->m_pCubeVB->destroy();

    delete this->m_pCubeIB;
    delete this->m_pCubeVB;
    delete this->m_pColorVS;
    delete this->m_pColorFS;
    delete this->m_pColorSP;

    delete this->m_pContext;
}

void Cube::updateGLScene()
{
    float rotationAngle = 2.0f;
    this->m_mtxRotation.rotate(rotationAngle,
                               QVector3D(1.0f, 1.0f, 0.0f));

    this->update();
}

void Cube::initializeGL()
{
    this->initializeOpenGLFunctions();

    this->glViewport(0, 0, this->width(), this->height());

    this->buildBuffers();

    if(this->manageShaders() == false)
    {
        QMessageBox::information(nullptr, "Error", "An error occured while "
                                                   "loading resources!");

        this->close();
    }

    this->glEnable(GL_DEPTH_TEST);
    this->glDepthFunc(GL_LESS);

    this->glEnable(GL_CULL_FACE);
}

void Cube::paintGL()
{
    this->m_Timer.start();
    this->glClearColor(0.55f, 0.55f, 0.55f, 1.0f);
    this->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 mtxTranslate;
    mtxTranslate.translate(0.0f, 0.0f, -50.0f);
    QMatrix4x4 mtxWorld;
    mtxWorld.setToIdentity();
    mtxWorld = mtxTranslate * this->m_mtxRotation;

    this->m_mtxWorldViewProj = this->m_mtxViewProj * mtxWorld;

    this->m_pColorSP->bind();

    this->m_CubeVAO.bind();

    this->glUniformMatrix4fv(0, 1, GL_FALSE, this->m_mtxWorldViewProj.data());
    this->glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

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
                        0.01f, 1000.0f);

    this->m_mtxViewProj = mtxProj * this->m_Camera.getViewMatrix();
}

void Cube::buildBuffers()
{
    Vertex cubeVertexData[] =
    {
        { QVector3D(-10.0f, -10.0f, +10.0f), QVector4D(1.0f, 0.0f, 0.0f, 1.0f) },
        { QVector3D(-10.0f, +10.0f, +10.0f), QVector4D(0.0f, 0.0f, 0.0f, 1.0f) },
        { QVector3D(+10.0f, +10.0f, +10.0f), QVector4D(1.0f, 1.0f, 1.0f, 1.0f) },
        { QVector3D(+10.0f, -10.0f, +10.0f), QVector4D(0.0f, 1.0f, 0.0f, 1.0f) },

        { QVector3D(-10.0f, -10.0f, -10.0f), QVector4D(1.0f, 0.0f, 1.0f, 1.0f) },
        { QVector3D(-10.0f, +10.0f, -10.0f), QVector4D(0.0f, 0.0f, 1.0f, 1.0f) },
        { QVector3D(+10.0f, +10.0f, -10.0f), QVector4D(1.0f, 1.0f, 0.0f, 1.0f) },
        { QVector3D(+10.0f, -10.0f, -10.0f), QVector4D(0.0f, 1.0f, 1.0f, 1.0f) },
    };

    unsigned short cubeIndices[] =
    {
        // front plane
        0, 2, 1,
        0, 3, 2,
        // back plane
        7, 5, 6,
        7, 4, 5,
        // left plane
        4, 1, 5,
        4, 0, 1,
        // right plane
        3, 6, 2,
        3, 7, 6,
        // top plane
        1, 6, 5,
        1, 2, 6,
        // bottom plane
        4, 3, 0,
        4, 7, 3,
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

    this->m_pCubeIB = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    this->m_pCubeIB->create();
    this->m_pCubeIB->setUsagePattern(QOpenGLBuffer::StaticDraw);
    this->m_pCubeIB->bind();
    this->m_pCubeIB->allocate(cubeIndices, 36 * sizeof(unsigned short));

    this->m_CubeVAO.release();
}

bool Cube::manageShaders()
{
    this->m_pColorVS = new QOpenGLShader(QOpenGLShader::Vertex, this);
    if(this->m_pColorVS->compileSourceFile(":/shaders/Color.vsh") == false)
    {
        qDebug() << "Error compiling Vertex shader!";
        return false;
    }

    this->m_pColorFS = new QOpenGLShader(QOpenGLShader::Fragment, this);
    if(this->m_pColorFS->compileSourceFile(":/shaders/Color.fsh") == false)
    {
        qDebug() << "Error compiling Fragment shader!";
        return false;
    }

    this->m_pColorSP = new QOpenGLShaderProgram(this);
    this->m_pColorSP->create();
    this->m_pColorSP->addShader(this->m_pColorVS);
    this->m_pColorSP->addShader(this->m_pColorFS);
    if(this->m_pColorSP->link() == false)
    {
        qDebug() << "Cannot link the shader program!";
        return false;
    }

    return true;
}
