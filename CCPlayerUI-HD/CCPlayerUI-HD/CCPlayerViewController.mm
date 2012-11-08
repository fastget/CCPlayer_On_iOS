//
//  CCPlayerViewController.m
//  CCPlayer
//
//  Created by maclion on 10/31/12.
//  Copyright (c) 2012 cc. All rights reserved.
//

#import "CCPlayerViewController.h"
#import "CCPlayerHeaderView.h"
#import "CCPlayerMaskView.h"
#import "CCPlayerFooterView.h"
#import "CCPlayerRenderView.h"
#import "PlayerAdapter.h"

#define PLAYER_HEADER_VIEW_HEIGHT   80
#define PLAYER_FOOTER_VIEW_HEIGHT   100

@interface CCPlayerViewController ()
{
    //this is a C++ object
    CCPlayerViewAdapter*    _playerViewAdapter;
    
    //this is a oc object
    CCPlayerRenderView*     _playerRenderView;
    CCPlayerMaskView*       _playerMaskView;
    CCPlayerHeaderView*     _playerHeaderView;
    CCPlayerFooterView*     _playerFooterView;
}

@end

@implementation CCPlayerViewController

#pragma --mark "The system interface"

-(id)initWithCoder:(NSCoder *)aDecoder {
    
    if(self = [super initWithCoder:aDecoder])
    {
    }
    
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    // Do any additional setup after loading the view.
    
    // Custom initialization
    //create the C++ object
    _playerViewAdapter = new CCPlayerViewAdapter(self);
    
    CGRect rectScreen = [[UIScreen mainScreen] bounds];
    
    //pretend the views OrientationIsLandscape
    CGRect rectDisplay = CGRectMake(rectScreen.origin.x,
                                    rectScreen.origin.y,
                                    rectScreen.size.height,
                                    rectScreen.size.width);
    CGRect rectMask = CGRectMake(rectScreen.origin.x,
                                    rectScreen.origin.y,
                                    rectScreen.size.height,
                                 rectScreen.size.width);
    CGRect rectHeader = CGRectMake(rectScreen.origin.x,
                                   rectScreen.origin.y,
                                   rectScreen.size.height,
                                   PLAYER_HEADER_VIEW_HEIGHT);
    CGRect rectFooter = CGRectMake(rectScreen.origin.x,
                                   rectScreen.size.width - PLAYER_FOOTER_VIEW_HEIGHT,
                                   rectScreen.size.height,
                                   PLAYER_FOOTER_VIEW_HEIGHT);
    
    _playerRenderView = [[CCPlayerRenderView alloc] initWithFrame:rectDisplay];
    _playerMaskView = [[CCPlayerMaskView alloc] initWithFrame:rectMask];
    _playerMaskView.backgroundColor = [UIColor clearColor];
    _playerMaskView.delegate = self;
    _playerHeaderView = [[CCPlayerHeaderView alloc] initWithFrame:rectHeader];
    _playerHeaderView.backgroundColor = [UIColor clearColor];
    _playerHeaderView.delegate = self;
    _playerFooterView = [[CCPlayerFooterView alloc] initWithFrame:rectFooter];
    _playerFooterView.backgroundColor = [UIColor clearColor];
    _playerFooterView.delegate = self;
    
    [self.view addSubview:_playerRenderView];
    [self.view addSubview:_playerMaskView];
    [self.view addSubview:_playerHeaderView];
    [self.view addSubview:_playerFooterView];
    
    NSString* medaiPath = [[NSBundle mainBundle] pathForResource:@"test" ofType:@"flv"];
    _playerHeaderView.textViewTitle.text = @"扔掉假睫毛";
    
    NSLog(@"The mediaPath is : %@", medaiPath);
    
    _playerViewAdapter->Open(medaiPath);
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation
{
    return UIInterfaceOrientationIsLandscape(toInterfaceOrientation);
}

- (void)layoutSubviews
{
}

- (void)dealloc
{
    if (_playerRenderView != nil)
    {
        _playerRenderView = nil;
    }
    
    if (_playerViewAdapter != NULL)
    {
        delete _playerViewAdapter;
        _playerViewAdapter = NULL;
    }
}


#pragma --mark "cc player callbacks"
- (void)onCommandOpen:(NSArray*)errCode
{
    _playerViewAdapter->SetGLRenderView();
}

- (void)onCommandStop:(NSArray*)errCode
{
    if (_playerViewAdapter != NULL)
    {
        delete _playerViewAdapter;
        _playerViewAdapter = NULL;
    }
    
    [self dismissModalViewControllerAnimated:YES];
}


- (CGRect)getRenderViewRect
{
    return _playerRenderView.frame;
}

- (int)preDrawFrame
{
    if (_playerRenderView != nil)
    {
        [_playerRenderView preDrawFrame];
    }
    return 0;
}

- (int)swapBuffers
{
    if (_playerRenderView != nil)
    {
        [_playerRenderView swapBuffers];
    }
    return 0;
}

#pragma --mark "player user actions"

- (void)backButtonPressed
{
    _playerViewAdapter->Stop();
}

- (void)twoFingersTapTwice
{
    _playerHeaderView.hidden = !_playerHeaderView.hidden;
    _playerFooterView.hidden = !_playerFooterView.hidden;
}

@end
