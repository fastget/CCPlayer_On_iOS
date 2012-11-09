//
//  CCHeaderViewController.h
//  CCPlayerUI-HD
//
//  Created by maclion on 11/8/12.
//  Copyright (c) 2012 cc. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "ViewInterContactProtocals.h"

@interface CCHeaderViewController : UIViewController

@property (strong, nonatomic) NSObject<CCHeaderViewToViewControllerDelegate>* delegate;

@end
