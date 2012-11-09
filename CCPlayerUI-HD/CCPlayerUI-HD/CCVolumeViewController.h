//
//  CCVolumeViewController.h
//  CCPlayerUI-HD
//
//  Created by maclion on 11/9/12.
//  Copyright (c) 2012 cc. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "ViewInterContactProtocals.h"

@interface CCVolumeViewController : UIViewController

@property (strong, nonatomic) NSObject<CCVolumeViewToViewControllerDelegate>* delegate;

@end
