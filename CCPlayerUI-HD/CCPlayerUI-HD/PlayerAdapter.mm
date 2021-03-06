//
//  PlayerAdapter.mm
//  CCPlayer
//
//  Created by maclion on 11/2/12.
//  Copyright (c) 2012 cc. All rights reserved.
//

#include "PlayerAdapter.h"
#include "Player.h"

CCPlayerViewAdapter::CCPlayerViewAdapter(NSObject<PlayerDelegate>* delegate)
:m_pPlayerInstance(NULL)
{
    this->delegate = delegate;
    
    m_pPlayerInstance = new CCPlayer::CCPlayer(this);
}

CCPlayerViewAdapter::~CCPlayerViewAdapter()
{
    this->delegate = nil;
    
    if (m_pPlayerInstance != NULL)
    {
        delete m_pPlayerInstance;
        m_pPlayerInstance = NULL;
    }
}

#pragma --mark "this will called by player view controller"

int CCPlayerViewAdapter::Open(NSString* mediaPath)
{
    const char* szMediaPath = [mediaPath UTF8String];
    m_pPlayerInstance->Open(std::string(szMediaPath));
    return 0;
}

int CCPlayerViewAdapter::Pause()
{
    m_pPlayerInstance->Pause();
    return 0;
}

int CCPlayerViewAdapter::Stop()
{
    m_pPlayerInstance->Stop();
    return 0;
}

int CCPlayerViewAdapter::Seek(int64_t posInSecond)
{
    return 0;
}

int CCPlayerViewAdapter::Continue()
{
    m_pPlayerInstance->Continue();
    return 0;
}

void CCPlayerViewAdapter::SetVolume(float volume)
{
    m_pPlayerInstance->SetVolume(volume);
}

//void CCPlayerViewAdapter::SetGLRenderView()
//{
//    m_pPlayerInstance->SetGLRenderView(this);
//}

int64_t CCPlayerViewAdapter::GetTotalDuration()
{
    int64_t totalDuration = 0;
    m_pPlayerInstance->GetTotalDurationBySecond(&totalDuration);
    return totalDuration;
}

int64_t CCPlayerViewAdapter::GetCurrentPosition()
{
    int64_t currentPostion = 0;
    m_pPlayerInstance->GetCurrentPostionBySecond(&currentPostion);
    return currentPostion;
}

#pragma --mark "this is the callback functions by cclib"

void CCPlayerViewAdapter::OnCommandOpen(int ErrCode)
{
    if (delegate != nil
        && [delegate respondsToSelector:@selector(onCommandOpen:)])
    {
        NSMutableArray* param = [[NSMutableArray alloc] init];
        [param addObject:[NSNumber numberWithInt:ErrCode]];
        [delegate performSelectorOnMainThread:@selector(onCommandOpen:)
                                   withObject:param
                                waitUntilDone:NO];
    }
}

void CCPlayerViewAdapter::OnCommandStop(int ErrCode)
{
    if (delegate != nil
        && [delegate respondsToSelector:@selector(onCommandStop:)])
    {
        NSMutableArray* param = [[NSMutableArray alloc] init];
        [param addObject:[NSNumber numberWithInt:ErrCode]];
        [delegate performSelectorOnMainThread:@selector(onCommandStop:)
                                   withObject:param
                                waitUntilDone:NO];
    }
}

int CCPlayerViewAdapter::GetRenderViewRect(int* x, int* y, int* width, int* height)
{
    if (delegate != nil
        && [delegate respondsToSelector:@selector(getRenderViewRect)])
    {
        CGRect renderViewRect = [delegate getRenderViewRect];
        *x = renderViewRect.origin.x;
        *y = renderViewRect.origin.y;
        *width = renderViewRect.size.width;
        *height = renderViewRect.size.height;
    }
    return 0;
}

int CCPlayerViewAdapter::PreDrawFrame()
{
    if (delegate != nil && [delegate respondsToSelector:@selector(preDrawFrame)])
    {
        [delegate preDrawFrame];
    }
    return 0;
}

int CCPlayerViewAdapter::SwapBuffers()
{
    if (delegate != nil
        && [delegate respondsToSelector:@selector(swapBuffers)])
    {
        [delegate swapBuffers];
    }
    return 0;
}