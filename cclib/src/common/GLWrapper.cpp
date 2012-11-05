#include "GLWrapper.h"
#include "IGLView.h"
#include "VideoDef.h"

namespace CCPlayer
{
    
#define STRINGIZE(x) #x
#define STRINGIZE2(x) STRINGIZE(x)
#define SHADER_STRING(text) STRINGIZE2(text)

const std::string vertexShaderString =  SHADER_STRING
(
//attribute vec4 position;
//attribute vec3 color;
//varying lowp vec3 colorVarying;
// 
//attribute vec2 TexCoordIn;
//varying vec2 TexCoordOut;
//
//void main()
//{
//    colorVarying = color;
//    gl_Position = position;
//    TexCoordOut = TexCoordIn;
//}
 
 attribute vec4 Position;
 attribute vec4 SourceColor;
 
 varying vec4 DestinationColor;
 
 attribute vec2 TexCoordIn; // New
 varying vec2 TexCoordOut; // New
 
 void main(void) {
     DestinationColor = SourceColor;
     gl_Position = Position;
     TexCoordOut = TexCoordIn; // New
 }
);
    
const std::string fragmentShaderString = SHADER_STRING
(
//varying lowp vec3 colorVarying;
//varying lowp vec2 TexCoordOut;
// 
//void main()
//{
//    //gl_FragColor = vec4(colorVarying, 1.0);
//     gl_FragColor = colorVarying * texture2D(TexCoordOut);
//}
 
 varying lowp vec4 DestinationColor;
 
 varying lowp vec2 TexCoordOut; // New
 uniform sampler2D Texture; // New
 
 void main(void) {
     gl_FragColor = DestinationColor * texture2D(Texture, TexCoordOut); // New
 }
 
);

CCGLWrapper::CCGLWrapper()
:m_pIGLRenderView(NULL)
{
}

CCGLWrapper::~CCGLWrapper()
{
    glGetError();
}

void CCGLWrapper::SetIGLRenderView(IGLView* pGLRenderView)
{
    m_pIGLRenderView = pGLRenderView;
}

void CCGLWrapper::InitGL()
{
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    m_pIGLRenderView->GetRenderViewRect(&x, &y, &width, &height);
    
    glViewport(0, 0, width, height);
    GLCheckError();
}

GLuint CCGLWrapper::compileShader(GLenum shaderType, const char** pShaderContext, int contextLength)
{
    GLuint shaderHandle = glCreateShader(shaderType);
    
    glShaderSource(shaderHandle, 1, pShaderContext, NULL);
    
    glCompileShader(shaderHandle);
    
    GLint compileSuccess;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileSuccess);
    if (compileSuccess == GL_FALSE) {
        GLchar messages[256];
        glGetShaderInfoLog(shaderHandle, sizeof(messages), 0, &messages[0]);
        
        printf("Compile share error : %s\n", messages);
        
        return 0;
    }
    return shaderHandle;
}

void CCGLWrapper::CompileShaders()
{
    const char* szVertexShaderString = vertexShaderString.c_str();
    const char* szFragmentShaderString = fragmentShaderString.c_str();
    
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER,
                                        (const char**)&szVertexShaderString,
                                        strlen(szVertexShaderString));
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER,
                                          (const char**)&szFragmentShaderString,
                                          strlen(szFragmentShaderString));
    
    GLuint programHandle = glCreateProgram();
    glAttachShader(programHandle, vertexShader);
    glAttachShader(programHandle, fragmentShader);
    glLinkProgram(programHandle);
    
    GLint linkSuccess;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess == GL_FALSE) {
        GLchar messages[256];
        glGetProgramInfoLog(programHandle, sizeof(messages), 0, &messages[0]);
        printf("Linke share error : %s\n", messages);
    }
    
    glUseProgram(programHandle);

    // 5
    m_positionSlot = glGetAttribLocation(programHandle, "Position");
    m_colorSlot = glGetAttribLocation(programHandle, "SourceColor");
    m_texCoordSlot = glGetAttribLocation(programHandle, "TexCoordIn");
    m_textureUniform = glGetUniformLocation(programHandle, "Texture");
    
    glEnableVertexAttribArray(m_positionSlot);
    glEnableVertexAttribArray(m_colorSlot);
    glEnableVertexAttribArray(m_texCoordSlot);
}
    
void CCGLWrapper::setupVBOs()
{
    //    Front
    //    position color texture
    
    //position
    vertexColorTexture.push_back(-1.0f);
    vertexColorTexture.push_back(-1.0f);
    vertexColorTexture.push_back(0.0f);
    //color
    vertexColorTexture.push_back(1.0f);
    vertexColorTexture.push_back(1.0f);
    vertexColorTexture.push_back(1.0f);
    vertexColorTexture.push_back(1.0f);
    //texture
    vertexColorTexture.push_back(0.0f);
    vertexColorTexture.push_back(1.0f);

    //position
    vertexColorTexture.push_back(-1.0f);
    vertexColorTexture.push_back(1.0f);
    vertexColorTexture.push_back(0.0f);
    //color
    vertexColorTexture.push_back(1.0f);
    vertexColorTexture.push_back(1.0f);
    vertexColorTexture.push_back(1.0f);
    vertexColorTexture.push_back(1.0f);
    //texture
    vertexColorTexture.push_back(0.0f);
    vertexColorTexture.push_back(0.0f);
    
    //position
    vertexColorTexture.push_back(1.0f);
    vertexColorTexture.push_back(1.0f);
    vertexColorTexture.push_back(0.0f);
    //color
    vertexColorTexture.push_back(1.0f);
    vertexColorTexture.push_back(1.0f);
    vertexColorTexture.push_back(1.0f);
    vertexColorTexture.push_back(1.0f);
    //texture
    vertexColorTexture.push_back(1.0f);
    vertexColorTexture.push_back(0.0f);
    
    //position
    vertexColorTexture.push_back(1.0f);
    vertexColorTexture.push_back(-1.0f);
    vertexColorTexture.push_back(0.0f);
    //color
    vertexColorTexture.push_back(1.0f);
    vertexColorTexture.push_back(1.0f);
    vertexColorTexture.push_back(1.0f);
    vertexColorTexture.push_back(1.0f);
    //texture
    vertexColorTexture.push_back(1.0f);
    vertexColorTexture.push_back(1.0f);

    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertexColorTexture.size(), &vertexColorTexture[0], GL_STATIC_DRAW);

    //    Front
    //    0, 1, 2,
    //    2, 3, 0,
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(2);
    indices.push_back(3);
    indices.push_back(0);

    glGenBuffers(1, &m_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte)*indices.size(), &indices[0], GL_STATIC_DRAW);
}

GLuint CCGLWrapper::setupTexture()
{
    GLuint texName;
    glGenTextures(1, &texName);
    glBindTexture(GL_TEXTURE_2D, texName);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    return texName;
    
}

int CCGLWrapper::DrawFrame(VideoFrame* pVideoFrame, int width, int height)
{
    m_pIGLRenderView->PreDrawFrame();
    
    static bool first = true;
    if (first)
    {
        glGenTextures(1, &m_glTexture);
        glBindTexture(GL_TEXTURE_2D, m_glTexture);
        glTexParameterf(GL_TEXTURE_2D,
                        GL_TEXTURE_MIN_FILTER,
                        GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D,
                        GL_TEXTURE_MAG_FILTER,
                        GL_NEAREST);
        
        //Init the render rect
        InitGL();
        
        setupVBOs();
        
        CompileShaders();
        
        first = false;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    
    glVertexAttribPointer(m_positionSlot, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, 0);
    glVertexAttribPointer(m_colorSlot, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (GLvoid*) (sizeof(float) * 3));
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_glTexture);
    glVertexAttribPointer(m_texCoordSlot, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (GLvoid*) (sizeof(float) * 7));
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, pVideoFrame->GetFrameData());
    glUniform1i(m_textureUniform, 0);
    
    glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(indices[0]), GL_UNSIGNED_BYTE, 0);
    
    m_pIGLRenderView->SwapBuffers();
    
    return 0;
}

void CCGLWrapper::GLCheckError()
{
    GLenum error = glGetError();
    
    switch (error) {
        case GL_INVALID_ENUM:
            printf("GL Error: Enum argument is out of range\n");
            break;
        case GL_INVALID_VALUE:
            printf("GL Error: Numeric value is out of range\n");
            break;
        case GL_INVALID_OPERATION:
            printf("GL Error: Operation illegal in current state\n");
            break;
        case GL_OUT_OF_MEMORY:
            printf("GL Error: Not enough memory to execute command\n");
            break;
        case GL_NO_ERROR:
            printf("GL Error: No Error\n");
            break;
        default:
            printf("Unknown GL Error");
            break;
    }
}

}
