//
//  CCPlayerHeaderView.m
//  CCPlayerUI-HD
//
//  Created by maclion on 11/7/12.
//  Copyright (c) 2012 cc. All rights reserved.
//

#import "CCPlayerHeaderView.h"

@interface CCPlayerHeaderView ()
{
    UIImageView* _imageBackGroundLeft;
    UIImageView* _imageBackGroundRight;
}

@end

@implementation CCPlayerHeaderView

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
        
        _imageBackGroundLeft = [[UIImageView alloc] initWithFrame:rectImageBackgroundLeft];
        _imageBackGroundRight = [[UIImageView alloc] initWithFrame:rectImageBackgroundRight];
     
        _imageBackGroundLeft.image = [UIImage imageNamed:@"header_bg_left"];
        _imageBackGroundRight.image = [UIImage imageNamed:@"header_bg_right"];
        
        [self addSubview:_imageBackGroundLeft];
        [self addSubview:_imageBackGroundRight];
    }
    return self;
}

@end
