//
//  PlayerAdapterProtocol.h
//  CCPlayer
//
//  Created by maclion on 11/2/12.
//  Copyright (c) 2012 cc. All rights reserved.
//

@protocol PlayerDelegate <NSObject>

@required
//directly call the adapter
//- (int)createGLContext;
- (int)swapBuffers;
- (int)preDrawFrame;
- (CGRect)getRenderViewRect;

//call the run the function in the main thread
- (void)onCmmandOpen:(int)errCode;

@end
