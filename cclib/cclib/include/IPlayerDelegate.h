//
//  IPlayerDelegate.h
//  cclib
//
//  Created by maclion on 11/10/12.
//  Copyright (c) 2012 cc. All rights reserved.
//

#ifndef IPLAYER_DELEGATE_H
#define IPLAYER_DELEGATE_H

namespace CCPlayer
{
    
class IPlayerDelegate
{
public:
    virtual void OnCommandOpen(int ErrCode) = 0;
    virtual void OnCommandStop(int ErrCode) = 0;
        
public:
    virtual int GetRenderViewRect(int* x, int* y, int* width, int* height) = 0;
    virtual int PreDrawFrame() = 0;
    virtual int SwapBuffers() = 0;
};
    
}

#endif
