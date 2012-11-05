//the incoming vertex' position
attribute vec4 position;

//and its color
attribute vec3 color;

//the varying statement tells the shader pipeline that this variable
//has to be passed on to the next stage (so the fragment shader)
varying lowp vec3 colorVarying;


//the shader entry point is the main method
void main()
{    
    colorVarying = color; //save the color for the fragment shader
    gl_Position = position; //copy the position
}
