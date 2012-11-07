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

@interface CCPlayerViewController ()
{
    //this is a C++ object
    CCPlayerViewAdapter* _playerViewAdapter;
    
    //this is a oc object
    CCPlayerRenderView* _playerRenderView;
}

@end

@implementation CCPlayerViewController

-(id)initWithCoder:(NSCoder *)aDecoder {
    
    if(self = [super initWithCoder:aDecoder])
    {
        // Do any additional setup after loading the view.

        // Custom initialization
        //create the C++ object
        _playerViewAdapter = new CCPlayerViewAdapter(self);
        
        //create the oc object
        CGRect screenBounds = [[UIScreen mainScreen] bounds];
        _playerRenderView = [[CCPlayerRenderView alloc] initWithFrame:screenBounds];
        [self.view addSubview:_playerRenderView];
    }
    
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    NSString* medaiPath = [[NSBundle mainBundle] pathForResource:@"test" ofType:@"flv"];
    NSLog(@"The mediaPath is : %@", medaiPath);
    
    _playerViewAdapter->Open(medaiPath);
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    NSLog(@"Prepare change to second view");
    NSLog(@"%@", [segue identifier]);
}

//call the run the function in the main thread
- (void)onCmmandOpen:(int)errCode
{
    _playerViewAdapter->SetGLRenderView();
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

@end
