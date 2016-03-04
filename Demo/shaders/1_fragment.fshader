#version 140



out vec4 LFragment;

uniform float time;
uniform float screenWidth;
uniform float screenHeight;

void main() 
{ 
    float x = gl_FragCoord.x/screenWidth-0.5;
    float y = gl_FragCoord.y/screenHeight-0.5;
    
    vec2 up = vec2(0, 1); 
    
    x *= 2;
    y *= 2;
    
    x *= screenWidth/screenHeight;
    
    float r = 1.0, g = 1.0, b = 1.0;
    float time2 = time*time;
    float dota = dot(up,normalize(vec2(x+sin(time),y+sin(time))));
    dota*=dota;
    float len = length(vec2(x,y));
    
    b = 0;
    r = 0;
    g = sin(len*5)*dota;
    
    LFragment = vec4( r, g, b, 1.0 ); 
}
