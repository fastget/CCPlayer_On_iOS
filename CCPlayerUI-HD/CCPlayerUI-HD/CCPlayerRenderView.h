//
//  CCPlayerRenderView.h
//  CCPlayer
//
//  Created by maclion on 11/2/12.
//  Copyright (c) 2012 cc. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface CCPlayerRenderView : UIView

- (void)createGLContext;
- (int)preDrawFrame;
- (void)swapBuffers;

@property (nonatomic, retain) EAGLContext *context;

@end
