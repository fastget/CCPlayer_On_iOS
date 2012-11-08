//
//  CCPlayerMaskView.m
//  CCPlayerUI-HD
//
//  Created by maclion on 11/7/12.
//  Copyright (c) 2012 cc. All rights reserved.
//

#import "CCPlayerMaskView.h"

@interface CCPlayerMaskView ()
{
    UITapGestureRecognizer*     _twoFingersTapTwice;
}

@end

@implementation CCPlayerMaskView

@synthesize delegate = _delegate;

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
        _twoFingersTapTwice = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersTapTwice:)];
        [_twoFingersTapTwice setNumberOfTapsRequired:2];
        [_twoFingersTapTwice setNumberOfTouchesRequired:1];
        
        [self addGestureRecognizer:_twoFingersTapTwice];
    }
    return self;
}

- (void)twoFingersTapTwice:(UITapGestureRecognizer *)recognizer
{
    if (_delegate != nil
        && [_delegate respondsToSelector:@selector(twoFingersTapTwice)])
    {
        [_delegate twoFingersTapTwice];
    }
}

@end
