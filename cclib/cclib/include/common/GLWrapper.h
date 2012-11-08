#ifndef GLWRAPPER_H
#define GLWRAPPER_H

#include "Common.h"

namespace CCPlayer
{

class IGLView;
class VideoFrame;

class CCGLWrapper
{
public:
    CCGLWrapper();
    virtual ~CCGLWrapper();

public:
    void SetIGLRenderView(IGLView* pGLRenderView);
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
    IGLView* m_pIGLRenderView;
    
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
