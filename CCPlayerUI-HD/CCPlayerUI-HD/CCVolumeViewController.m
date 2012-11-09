//
//  CCVolumeViewController.m
//  CCPlayerUI-HD
//
//  Created by maclion on 11/9/12.
//  Copyright (c) 2012 cc. All rights reserved.
//

#import "CCVolumeViewController.h"

@interface CCVolumeViewController ()

@property (strong, nonatomic) IBOutlet UISlider *volumeSlider;

@end

@implementation CCVolumeViewController
@synthesize volumeSlider = _volumeSlider;

@synthesize delegate = _delegate;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    
    
    CGAffineTransform sliderTrans = CGAffineTransformMakeRotation(M_PI * 1.5);
    self.view.transform = sliderTrans;
}

- (void)viewDidUnload
{
    [self setVolumeSlider:nil];
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	return YES;
}

- (IBAction)volumeSliderValueChanged:(id)sender {
    
    if ([sender isKindOfClass:[UISlider class]]
        && _delegate != nil
        && [_delegate respondsToSelector:@selector(setVolume:)]
        ) {
        float volume = ((UISlider*)sender).value;
        [_delegate setVolume:volume];
    }
}


@end
