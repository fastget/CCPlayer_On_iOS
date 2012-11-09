//
//  CCMaskViewController.m
//  CCPlayerUI-HD
//
//  Created by maclion on 11/8/12.
//  Copyright (c) 2012 cc. All rights reserved.
//

#import "CCMaskViewController.h"

@interface CCMaskViewController ()
@property (strong, nonatomic) IBOutlet UITapGestureRecognizer *fingerTapGesture;

@end

@implementation CCMaskViewController
@synthesize fingerTapGesture = _fingerTapGesture;

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
    // Do any additional setup after loading the view from its nib.
    
    _fingerTapGesture.numberOfTapsRequired = 2;
    _fingerTapGesture.numberOfTouchesRequired = 1;
}

- (void)viewDidUnload
{
    [self setFingerTapGesture:nil];
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	return YES;
}

- (IBAction)twoFingersTapTwice:(id)sender {
    if (_delegate != nil
        && [_delegate respondsToSelector:@selector(twoFingersTapTwice)]) {
        [_delegate twoFingersTapTwice];
    }
}

@end
