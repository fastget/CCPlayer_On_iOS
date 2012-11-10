#ifndef GLWRAPPER_H
#define GLWRAPPER_H

#include "Common.h"

namespace CCPlayer
{

class IPlayerDelegate;
class VideoFrame;

class CCGLWrapper
{
public:
    CCGLWrapper(IPlayerDelegate* pIPlayerDelegate);
    virtual ~CCGLWrapper();

public:
    int ClearGLRenderView();
    int DrawFrame(VideoFrame* pVideoFrame, int width, int height);

private:
    void InitGL();
    void CompileShaders();
    GLuint compileShader(GLenum shaderType, const char** pShaderContext, int contextLength);
    void setupVBOs();
    GLuint setupTexture();

private:
    void GLCheckError();
    
private:
    bool m_bFirstTime;
    
private:
    IPlayerDelegate* m_pIPlayerDelegate;
    
//    GLuint m_glTexture;
    GLint m_positionSlot;
    GLint m_colorSlot;
//    GLint m_projectionUniform;
//    GLint m_modelViewUniform;
    GLint m_texCoordSlot;
    GLint m_textureUniform;
    GLuint m_vertexBuffer;
    GLuint m_indexBuffer;
    GLuint m_colorBuffer;
    GLuint m_glTexture;
    std::vector<float> vertexColorTexture;
    std::vector<GLubyte> indices;
};

}
#endif // GLWRAPPER_H
