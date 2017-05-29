
in vec2 vs_fs_coord;

uniform vec4 custom_0; // hue, alpha, eye dir, time
uniform vec4 custom_1;

layout (location = 0) out vec4 color;

#define PI 3.1415926536

void main(void)
{
	// ghost
    vec2 p = vs_fs_coord;
    vec3 base_col = custom_0.x < 0.5 ? vec3(1.0, 0.1, 0.1) :
                    custom_0.x < 1.5 ? vec3(1.0, 0.1, 1.0) :
                    custom_0.x < 2.5 ? vec3(0.1, 0.8, 0.1) : vec3(0.1, 0.1, 0.8);
    vec4 rgba = vec4(0.0, 0.0, 0.0, 0.0);
    
    // left eye
    if(length((p - vec2(-0.1, -0.1))*vec2(1.5, 1.0)) < 0.1)
    {
        if(length((p - vec2(-0.1, -0.075))) < 0.05)
            rgba = vec4(0., 0., 0., custom_0.y);
        else
            rgba = vec4(1., 1., 1., custom_0.y);
    }
    // right eye
    else if(length((p - vec2(0.1, -0.1))*vec2(1.5, 1.0)) < 0.1)
    {
        if(length((p - vec2(0.1, -0.075))) < 0.05)
            rgba = vec4(0., 0., 0., custom_0.y);
        else
            rgba = vec4(1., 1., 1., custom_0.y);
    }
    else
    {
        const float dsphere = 0.3;
    
        float d = length(p) - dsphere;
    
        
        float bx = max(p.x - dsphere, -p.x - dsphere);
        float by = max(p.y - dsphere, -p.y);
        d = min(max(bx, by), d);
        
        p.y += cos(p.x * 32.0 - 2.0*custom_0.w) * 0.05;
        d = max( d, p.y - 0.3 );
        
        if( d < 0.0 )
            rgba = vec4( base_col, custom_0.y);
    }
    
    color = rgba;
}
