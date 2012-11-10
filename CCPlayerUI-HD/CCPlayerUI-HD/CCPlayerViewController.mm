//
//  CCPlayerViewController.m
//  CCPlayer
//
//  Created by maclion on 10/31/12.
//  Copyright (c) 2012 cc. All rights reserved.
//

#import "CCPlayerViewController.h"
#import "CCPlayerRenderView.h"
#import "PlayerAdapter.h"

#import "CCMaskViewController.h"
#import "CCHeaderViewController.h"
#import "CCFooterViewController.h"
#import "CCVolumeViewController.h"

#define MASK_VIEW_XIB_NAME      @"CCMaskViewController"
#define HEADER_VIEW_XIB_NAME    @"CCHeaderViewController"
#define FOOTER_VIEW_XIB_NAME    @"CCFooterViewController"
#define VOLUME_VIEW_XIB_NAME    @"CCVolumeViewController"

#define VOLUME_VIEW_SPAN    20

@interface CCPlayerViewController ()
{
    //this is a C++ object
    CCPlayerViewAdapter*    m_pPlayerAdapter;
    
    //this is a oc object
    CCPlayerRenderView*     _playerRenderView;
    
    //this is the controllers
    CCMaskViewController*       _maskViewController;
    CCHeaderViewController*     _headerViewController;
    CCFooterViewController*     _footerViewController;
    CCVolumeViewController*     _volumeViewController;
    
    NSTimer*    _updateProgressTimer;
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
    m_pPlayerAdapter = new CCPlayerViewAdapter(self);
    
    CGRect rectScreen = [[UIScreen mainScreen] bounds];
    
    CGRect rectDisplay = CGRectMake(rectScreen.origin.x,
                                    rectScreen.origin.y,
                                    rectScreen.size.height,
                                    rectScreen.size.width);
    _playerRenderView = [[CCPlayerRenderView alloc] initWithFrame:rectDisplay];
    
    //add the render view to the object
    [self.view addSubview:_playerRenderView];
    
    CGRect rectControls = [_playerRenderView bounds];
    
    _maskViewController = [[CCMaskViewController alloc] initWithNibName:MASK_VIEW_XIB_NAME bundle:nil];
    CGPoint centerPointMaskView = CGPointMake(
                                     rectControls.size.width / 2,
                                     rectControls.size.height / 2);
    _maskViewController.view.center = centerPointMaskView;
    [self.view addSubview:_maskViewController.view];
    _maskViewController.delegate = self;
    
    _headerViewController = [[CCHeaderViewController alloc] initWithNibName:HEADER_VIEW_XIB_NAME bundle:nil];
    //adjust the header view postion in main screen
    CGRect rectHeaderBounds = _headerViewController.view.bounds;
    CGPoint centerPointHeaderView = CGPointMake(rectControls.size.width / 2,
                                                rectHeaderBounds.size.height / 2);
    _headerViewController.view.center = centerPointHeaderView;
    [self.view addSubview:_headerViewController.view];
    _headerViewController.delegate = self;
    
    _footerViewController = [[CCFooterViewController alloc] initWithNibName:FOOTER_VIEW_XIB_NAME bundle:nil];
    //adjust the footer view postion in main screen
    CGRect rectFooterBounds = _footerViewController.view.bounds;
    CGPoint centerPointFooterView = CGPointMake(rectControls.size.width / 2,
                                                rectControls.size.height - rectFooterBounds.size.height / 2);
    _footerViewController.view.center = centerPointFooterView;
    [self.view addSubview:_footerViewController.view];
    _footerViewController.delegate = self;
    
    _volumeViewController = [[CCVolumeViewController alloc] initWithNibName:VOLUME_VIEW_XIB_NAME bundle:nil];
    //adjust the volume view postion in main screen
    CGRect rectVolumeBounds = _volumeViewController.view.bounds;
    CGPoint centerVolumePoint = CGPointMake(rectControls.size.width - VOLUME_VIEW_SPAN,
                                            (rectControls.size.height - rectVolumeBounds.size.height) / 2);
    
    
    _volumeViewController.view.center = centerVolumePoint;
    [self.view addSubview:_volumeViewController.view];
    _volumeViewController.delegate = self;
    
    NSString* medaiPath = [[NSBundle mainBundle] pathForResource:@"test" ofType:@"flv"];
//    _playerHeaderView.textViewTitle.text = @"扔掉假睫毛";
    
    NSLog(@"The mediaPath is : %@", medaiPath);
    
    m_pPlayerAdapter->Open(medaiPath);
    
    _updateProgressTimer = [NSTimer scheduledTimerWithTimeInterval:1 target:self selector:@selector(updateProgress:) userInfo:nil repeats:YES];
    
    //Start the time
    [_updateProgressTimer fire];
}

- (void)viewDidUnload
{
    _updateProgressTimer = nil;
    
    //clean up the C++ object
    if (m_pPlayerAdapter != NULL)
    {
        delete m_pPlayerAdapter;
        m_pPlayerAdapter = NULL;
    }
    
    //clean up the oc object
    _playerRenderView = nil;
    _maskViewController = nil;
    _headerViewController = nil;
    _footerViewController = nil;
    _volumeViewController = nil;
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

#pragma --mark "cc player callbacks"
- (void)onCommandOpen:(NSArray*)errCode
{
//    m_pPlayerAdapter->SetGLRenderView();
    m_pPlayerAdapter->SetVolume(0.0f);
}

- (void)onCommandStop:(NSArray*)errCode
{
    [_updateProgressTimer invalidate];
    
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

- (void)playButtonPressed
{
    //the movie auto play by the default , so do this , just continue play
    m_pPlayerAdapter->Continue();
}

- (void)pauseButtonPressed
{
    m_pPlayerAdapter->Pause();
}

- (void)backButtonPressed
{
    m_pPlayerAdapter->Stop();
}

- (void)setVolume:(float)volume
{
    m_pPlayerAdapter->SetVolume(volume);
}

- (void)twoFingersTapTwice
{
    _headerViewController.view.hidden = !_headerViewController.view.hidden;
    _footerViewController.view.hidden = !_footerViewController.view.hidden;
    _volumeViewController.view.hidden = !_volumeViewController.view.hidden;
}

#include <string>
std::string FormatTime(int64_t time)
{
    std::string result;
    
    int64_t divide = 60 * 60 * 60;
    
    while (divide)
    {
        int64_t tmp = time / divide;
        
        char buf[3] = {0};
        sprintf(buf,"%02lld", tmp);
        
        result += buf;
        
        if (divide > 60)
        {
            result += ":";
        }
        
        time %= divide;
        divide /= 60;
    }
    
    return result;
}

#pragma --mark "this is just a timer call back"
- (void)updateProgress:(id)userInfo
{
    int64_t totalDuration = m_pPlayerAdapter->GetTotalDuration();
    static int64_t currentPosition = 0; //m_pPlayerAdapter->GetCurrentPosition();
    
    currentPosition ++;
    std::string strTotalDuration = FormatTime(totalDuration);
    std::string strCurrentPostion = FormatTime(currentPosition);
    
    [_footerViewController setIndicationTimeValue:[[NSString alloc] initWithUTF8String:std::string(strCurrentPostion + "/" + strTotalDuration).c_str()]];
}

@end
