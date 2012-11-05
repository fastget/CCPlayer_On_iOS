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
}

CCPlayerViewAdapter::~CCPlayerViewAdapter()
{
    this->delegate = nil;
}

int CCPlayerViewAdapter::Open(NSString* mediaPath)
{
    const char* szMediaPath = [mediaPath UTF8String];
    m_pPlayerInstance = new CCPlayer::CCPlayer(this);
    m_pPlayerInstance->Open(std::string(szMediaPath));
    return 0;
}

int CCPlayerViewAdapter::Pause()
{
    return 0;
}

int CCPlayerViewAdapter::Stop()
{
    return 0;
}

int CCPlayerViewAdapter::Seek(int64_t posInSecond)
{
    return 0;
}

int CCPlayerViewAdapter::Continue()
{
    return 0;
}

void CCPlayerViewAdapter::SetGLRenderView()
{
    m_pPlayerInstance->SetGLRenderView(this);
}

void CCPlayerViewAdapter::OnCmmandOpen(int ErrCode)
{
    if (delegate != nil
        && [delegate respondsToSelector:@selector(onCmmandOpen:)])
    {
        NSMutableArray* param = [[NSMutableArray alloc] init];
        [param addObject:[NSNumber numberWithInt:ErrCode]];
        [delegate performSelectorOnMainThread:@selector(onCmmandOpen:)
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
        //[delegate performSelectorOnMainThread:@selector(preDrawFrame) withObject:nil waitUntilDone:YES];
    }
    return 0;
}

int CCPlayerViewAdapter::SwapBuffers()
{
    if (delegate != nil
        && [delegate respondsToSelector:@selector(swapBuffers)])
    {
        [delegate swapBuffers];
        //[delegate performSelectorOnMainThread:@selector(swapBuffers) withObject:nil waitUntilDone:YES];
    }
    return 0;
}