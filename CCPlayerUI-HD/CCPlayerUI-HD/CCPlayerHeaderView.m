//
//  CCPlayerHeaderView.m
//  CCPlayerUI-HD
//
//  Created by maclion on 11/7/12.
//  Copyright (c) 2012 cc. All rights reserved.
//

#import "CCPlayerHeaderView.h"

#define BACK_BUTTON_X       20
#define BACK_BUTTON_Y       20
#define BACK_BUTTON_WIDTH   80
#define BACK_BUTTON_HEIGHT  40
#define TITLE_TEXT_WIDTH    200
#define TITLE_TEXT_HEIGHT   80

@interface CCPlayerHeaderView ()
{
    UIImageView*    _imageBackGroundLeft;
    UIImageView*    _imageBackGroundRight;

    //the return button
    UIButton*       _playerBackButtun;
}

@end

@implementation CCPlayerHeaderView

@synthesize delegate = _delegate;
@synthesize textViewTitle = _textViewTitle;

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
        CGRect rectBackground = self.bounds;
        
        CGRect rectImageBackgroundLeft = CGRectMake(rectBackground.origin.x,
                                                    rectBackground.origin.y,
                                                    rectBackground.size.width / 2,
                                                    rectBackground.size.height);
        CGRect rectImageBackgroundRight = CGRectMake(rectBackground.size.width / 2,
                                                    rectBackground.origin.y,
                                                    rectBackground.size.width / 2,
                                                    rectBackground.size.height);
        
        CGRect rectTextTitle = CGRectMake((rectBackground.size.width - TITLE_TEXT_WIDTH) / 2,
                                          (rectBackground.size.height - TITLE_TEXT_HEIGHT) / 2,
                                          TITLE_TEXT_WIDTH,
                                          TITLE_TEXT_HEIGHT);
        
        CGRect rectBackButton = CGRectMake(BACK_BUTTON_X,
                                           BACK_BUTTON_Y,
                                           BACK_BUTTON_WIDTH,
                                           BACK_BUTTON_HEIGHT);
        
        _imageBackGroundLeft = [[UIImageView alloc] initWithFrame:rectImageBackgroundLeft];
        _imageBackGroundRight = [[UIImageView alloc] initWithFrame:rectImageBackgroundRight];
     
        _imageBackGroundLeft.image = [UIImage imageNamed:@"header_bg_left"];
        _imageBackGroundRight.image = [UIImage imageNamed:@"header_bg_right"];
        
        _textViewTitle = [[UITextView alloc] initWithFrame:rectTextTitle];
        _textViewTitle.backgroundColor = [UIColor clearColor];
        
        _playerBackButtun = [[UIButton alloc] initWithFrame:rectBackButton];
        [_playerBackButtun setImage:[UIImage imageNamed:@"return_normal"] forState:UIControlStateNormal];
        [_playerBackButtun setImage:[UIImage imageNamed:@"return_holdover"] forState:UIControlStateHighlighted];
        [_playerBackButtun addTarget:self action:@selector(backButtonPressed:) forControlEvents:UIControlEventTouchUpInside];
        
        [self addSubview:_imageBackGroundLeft];
        [self addSubview:_imageBackGroundRight];
        [self addSubview:_textViewTitle];
        [self addSubview:_playerBackButtun];
    }
    return self;
}

- (void)backButtonPressed:(id)sender
{
    if (_delegate != nil
        && [_delegate respondsToSelector:@selector(backButtonPressed)])
    {
        [_delegate backButtonPressed];
    }
}

@end
