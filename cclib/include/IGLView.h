#ifndef UIGLVIEW_H
#define UIGLVIEW_H

namespace CCPlayer
{

class IGLView
{
public:
    virtual int GetRenderViewRect(int* x, int* y, int* width, int* height) = 0;
    virtual int PreDrawFrame() = 0;
    virtual int SwapBuffers() = 0;
};

}

#endif // UIGLVIEW_H
