//
//  CCViewController.m
//  CCPlayer
//
//  Created by maclion on 10/31/12.
//  Copyright (c) 2012 cc. All rights reserved.
//

#import "CCRootViewController.h"
#import "CCPlayerViewController.h"

#define PLAYER_VIEW_XIB_NAME    @"CCPlayerViewController"

@interface CCRootViewController ()
{
    CCPlayerViewController*     _playerViewController;
}

@end

@implementation CCRootViewController

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
	// Do any additional setup after loading the view, typically from a nib.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)playMoviePressed:(id)sender {
    
    _playerViewController = [[CCPlayerViewController alloc] initWithNibName:PLAYER_VIEW_XIB_NAME bundle:nil];
    [self presentModalViewController:_playerViewController animated:NO];
}


@end
