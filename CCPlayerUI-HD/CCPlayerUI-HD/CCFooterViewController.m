//
//  CCFooterViewController.m
//  CCPlayerUI-HD
//
//  Created by maclion on 11/8/12.
//  Copyright (c) 2012 cc. All rights reserved.
//

#import "CCFooterViewController.h"

@interface CCFooterViewController ()
@property (strong, nonatomic) IBOutlet UIButton *playButton;
@property (strong, nonatomic) IBOutlet UIButton *pauseButton;
@property (strong, nonatomic) IBOutlet UIButton *prevButton;
@property (strong, nonatomic) IBOutlet UIButton *nextButton;
@property (weak, nonatomic) IBOutlet UISlider *progressSlider;

- (void)changeButtonStatus:(BOOL)bPlay;

@end

@implementation CCFooterViewController

@synthesize delegate = _delegate;

@synthesize playButton;
@synthesize pauseButton;
@synthesize prevButton;
@synthesize nextButton;
@synthesize progressSlider;

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
    
    [self changeButtonStatus:NO];
    
    prevButton.enabled = NO;
    nextButton.enabled = NO;
    
}

- (void)viewDidUnload
{
    [self setPlayButton:nil];
    [self setPauseButton:nil];
    [self setPrevButton:nil];
    [self setNextButton:nil];
    [self setProgressSlider:nil];
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	return YES;
}

- (IBAction)playButtonPressed:(id)sender {
    if (_delegate != nil
        && [_delegate respondsToSelector:@selector(playButtonPressed)]) {
        [_delegate playButtonPressed];
        [self changeButtonStatus:NO];
    }
    
    NSLog(@"play button pressed");
}

- (IBAction)pauseButtonPressed:(id)sender {
    if (_delegate != nil
        && [_delegate respondsToSelector:@selector(pauseButtonPressed)]) {
        [_delegate pauseButtonPressed];
        [self changeButtonStatus:YES];
    }
    
    NSLog(@"pause button pressed");
}

- (IBAction)progressSilderValueChanged:(id)sender {

}

- (void)changeButtonStatus:(BOOL)bPlay
{
    playButton.hidden = !bPlay;
    pauseButton.hidden = bPlay;
}

@end
