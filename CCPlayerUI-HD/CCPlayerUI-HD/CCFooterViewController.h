//
//  CCFooterViewController.h
//  CCPlayerUI-HD
//
//  Created by maclion on 11/8/12.
//  Copyright (c) 2012 cc. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "ViewInterContactProtocals.h"

@interface CCFooterViewController : UIViewController

@property (strong, nonatomic) NSObject<CCFooterViewToViewControllerDelegate>* delegate;

- (void)setIndicationTimeValue:(NSString*)value;

@end
