//
//  PlayerAdapter.h
//  CCPlayer
//
//  Created by maclion on 11/2/12.
//  Copyright (c) 2012 cc. All rights reserved.
//

#ifndef CCPlayer_PlayerAdapter_h
#define CCPlayer_PlayerAdapter_h

#import "PlayerAdapterProtocol.h"
#include "IGLView.h"
#include "ICommandResponse.h"

namespace CCPlayer
{
    class CCPlayer;
}

class CCPlayerViewAdapter: public CCPlayer::IGLView, public CCPlayer::ICommandResponse
{
public:
    CCPlayerViewAdapter(NSObject<PlayerDelegate>* delegate);
    ~CCPlayerViewAdapter();
    
    //below functions are just for command
public:
    int Open(NSString* mediaPath);
    int Pause();
    int Stop();
    int Seek(int64_t posInSecond);
    int Continue();
    
public:
    void SetGLRenderView();
    
public:
    virtual void OnCommandOpen(int ErrCode);
    virtual void OnCommandStop(int ErrCode);
    
public:
    virtual int GetRenderViewRect(int* x, int* y, int* width, int* height);
    virtual int PreDrawFrame();
    virtual int SwapBuffers();
    
private:
    NSObject<PlayerDelegate>* delegate;
    
private:
    CCPlayer::CCPlayer* m_pPlayerInstance;
};

#endif
