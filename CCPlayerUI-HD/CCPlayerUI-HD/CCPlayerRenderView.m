//
//  CCPlayerRenderView.m
//  CCPlayer
//
//  Created by maclion on 11/2/12.
//  Copyright (c) 2012 cc. All rights reserved.
//

#import "CCPlayerRenderView.h"
#import "QuartzCore/QuartzCore.h"

@interface CCPlayerRenderView ()
{
    CAEAGLLayer* _eaglLayer;
//    EAGLContext* _context;
    
    //implements the opengl
    GLuint _colorRenderBuffer;
    GLuint _depthRenderBuffer;
    GLuint _defaultFramebuffer;
}
@end

@implementation CCPlayerRenderView

@synthesize context;

+ (Class)layerClass {
    return [CAEAGLLayer class];
}

- (id)initWithFrame:(CGRect)frame
{
    NSLog(@"render view with :%f, height %f\n", frame.size.width,
          frame.size.height);
    
    self = [super initWithFrame:frame];
    if (self) {
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        
        //we don't want a transparent surface
        eaglLayer.opaque = TRUE;
        
        //here we configure the properties of our canvas, most important is the color depth RGBA8 !
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking,
                                        kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat,
                                        nil];
        
        //create an OpenGL ES 2 context
        self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    
        _colorRenderBuffer = 0;
        _depthRenderBuffer = 0;
        _defaultFramebuffer = 0;
        //if this failed or we cannot set the context for some reason, quit
        if (!self.context || ![EAGLContext setCurrentContext:self.context]) {
            NSLog(@"Could not create context!");
            return nil;
        }
        
        [self createGLContext];
    }
    return self;
}

- (void)createGLContext
{    
    NSLog(@"EAGLView: creating Framebuffer");
    
    if(![EAGLContext setCurrentContext: self.context]) {
        NSLog(@"Could not create context!");
        return ;
    }
    
    // Create default framebuffer object and bind it
    glGenFramebuffers(1, &_defaultFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _defaultFramebuffer);
    
    // Create color render buffer
    glGenRenderbuffers(1, &_colorRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBuffer);
    
    //get the storage from iOS so it can be displayed in the view
    [ self.context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];
    //get the frame's width and height
    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &framebufferWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &framebufferHeight);
    
    //attach this color buffer to our framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _colorRenderBuffer);
    
    //create a depth renderbuffer
    glGenRenderbuffers(1, &_depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderBuffer);
    //create the storage for the buffer, optimized for depth values, same size as the colorRenderbuffer
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, framebufferWidth, framebufferHeight);
    //attach the depth buffer to our framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRenderBuffer);
    
    //check that our configuration of the framebuffer is valid
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
}

- (int)preDrawFrame
{
//    if (_context == nil || ![EAGLContext setCurrentContext:_context]) {
//        NSLog(@"Failed to set current OpenGL context");
//    }
    
    if (![EAGLContext setCurrentContext: self.context])
    {
        NSLog(@"Cannot set current context");
        return -1;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, _defaultFramebuffer);
    
    //glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
    return 0;
}

- (void)swapBuffers
{
    //finally, get the color buffer we rendered to, and pass it to iOS
    //so it can display our awesome results!
    glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBuffer);
    
    [ self.context presentRenderbuffer:GL_RENDERBUFFER];
}

- (void)dealloc
{
//     self.context = nil;
//    glDeleteBuffers(1, &_depthRenderBuffer);
//    glDeleteBuffers(1, &_colorRenderBuffer);
}

- (void)GLCheckError
{
    GLenum error = glGetError();
    
    switch (error) {
        case GL_INVALID_ENUM:
            printf("GL Error: Enum argument is out of range\n");
            break;
        case GL_INVALID_VALUE:
            printf("GL Error: Numeric value is out of range\n");
            break;
        case GL_INVALID_OPERATION:
            printf("GL Error: Operation illegal in current state\n");
            break;
        case GL_OUT_OF_MEMORY:
            printf("GL Error: Not enough memory to execute command\n");
            break;
        case GL_NO_ERROR:
            printf("GL Error: No error\n");
            break;
        default:
            printf("Unknown GL Error");
            break;
    }
}

@end
