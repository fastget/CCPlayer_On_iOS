//
//  CCHeaderViewController.m
//  CCPlayerUI-HD
//
//  Created by maclion on 11/8/12.
//  Copyright (c) 2012 cc. All rights reserved.
//

#import "CCHeaderViewController.h"

@interface CCHeaderViewController ()

@end

@implementation CCHeaderViewController

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
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	return YES;
}

- (IBAction)backButtonPressed:(id)sender {
    if (_delegate != nil
        && [_delegate respondsToSelector:@selector(backButtonPressed)]) {
        [_delegate backButtonPressed];
    }
}

@end
