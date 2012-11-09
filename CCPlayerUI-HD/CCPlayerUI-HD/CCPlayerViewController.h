//
//  CCPlayerViewController.h
//  CCPlayer
//
//  Created by maclion on 10/31/12.
//  Copyright (c) 2012 cc. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "PlayerAdapterProtocol.h"
#import "ViewInterContactProtocals.h"

@interface CCPlayerViewController : UIViewController <PlayerDelegate,
                                                      CCMaskViewToViewControllerDelegate,
                                                      CCHeaderViewToViewControllerDelegate,
                                                      CCFooterViewToViewControllerDelegate,
                                                      CCVolumeViewToViewControllerDelegate>

@end
