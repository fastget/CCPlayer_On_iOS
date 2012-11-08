//
//  CCPlayerMaskView.h
//  CCPlayerUI-HD
//
//  Created by maclion on 11/7/12.
//  Copyright (c) 2012 cc. All rights reserved.
//

//this is the mask view protocals
@protocol CCMaskViewToViewControllerDelegate <NSObject>
@required
- (void)twoFingersTapTwice;
@end

//this is the header view protocals
@protocol CCHeaderViewToViewControllerDelegate <NSObject>
@required
- (void)backButtonPressed;

@end

//this is the footer view protocals
@protocol CCFooterViewToViewControllerDelegate <NSObject>
@required

@end