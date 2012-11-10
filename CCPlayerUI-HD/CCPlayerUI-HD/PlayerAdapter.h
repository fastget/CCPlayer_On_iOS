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
#include "IPlayerDelegate.h"

namespace CCPlayer
{
    class CCPlayer;
}

class CCPlayerViewAdapter: public CCPlayer::IPlayerDelegate
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
    void SetVolume(float volume);
    int64_t GetTotalDuration();
    int64_t GetCurrentPosition();
    
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
