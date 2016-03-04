#version 140



out vec4 LFragment;

uniform float time;
uniform float screenWidth;
uniform float screenHeight;

void main() 
{ 
    LFragment = vec4( 1.0, 1.0, gl_FragCoord.x/screenWidth, 1.0 ); 
}
