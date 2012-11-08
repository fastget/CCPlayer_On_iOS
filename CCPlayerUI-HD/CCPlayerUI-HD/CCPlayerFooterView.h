//
//  CCPlayerFooterView.h
//  CCPlayerUI-HD
//
//  Created by maclion on 11/7/12.
//  Copyright (c) 2012 cc. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "ViewInterContactProtocals.h"

@interface CCPlayerFooterView : UIView

@property (strong, nonatomic) NSObject<CCFooterViewToViewControllerDelegate>* delegate;

@end
