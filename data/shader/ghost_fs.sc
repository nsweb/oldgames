
#ifdef SHADER_SECTION
	$input v_fs_coord

	#include <bgfx_shader.sh>
#endif

uniform vec4 u_custom_0; // ghost id, alpha, z, w
uniform vec4 u_custom_1; 

#define PI 3.1415926536

vec3 hsv2rgb( in vec3 c )
{
    vec3 rgb = clamp( abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0, 1.0 );
    return mix( vec3(1.0,1.0,1.0), rgb, c.y) * c.z;
}

vec3 pal(float t, vec3 a, vec3 b, vec3 c, vec3 d)
{
    return a + b*cos( 6.28318*(c*t+d) );
}
vec3 palette(float t)
{
    return pal( t, vec3(0.5,0.8,0.8), vec3(0.5,0.2,0.2), vec3(1.0,1.0,1.0), vec3(0.525,1.0238,0.064) );
}

void main()
{
	// ghost
    vec2 p = v_fs_coord;
    vec3 base_col = hsv2rgb(palette(u_custom_0.x*0.05));
    vec4 rgba = vec4(0,0,0,0);
    
    // left eye
    if(length((p - vec2(-0.1, -0.1))*vec2(1.5, 1.0)) < 0.1)
    {
        float l = pow(smoothstep(0.0, 0.06, length(p - vec2(-0.1, -0.075))), 8);
        rgba = vec4(l, l, l, u_custom_0.y);
    }
    // right eye
    else if(length((p - vec2(0.1, -0.1))*vec2(1.5, 1.0)) < 0.1)
    {
        float l = pow(smoothstep(0.0, 0.06, length(p - vec2(0.1, -0.075))), 8);
        rgba = vec4(l, l, l, u_custom_0.y);
    }
    else
    {
        const float dsphere = 0.3;
        float d = length(p) - dsphere;
        
        float bx = max(p.x - dsphere, -p.x - dsphere);
        float by = max(p.y - dsphere, -p.y);
        d = min(max(bx, by), d);
        
        p.y += cos(p.x * 32.0 - 2.0*u_custom_0.w) * 0.05;
        d = max( d, p.y - 0.3 );
        
        float l = smoothstep(-0.01, 0.01, d);
        vec3 col = mix(vec3(1,1,1), base_col, max(1, 0.1 + u_custom_0.y));
        rgba = mix(vec4(col, u_custom_0.y), vec4(0,0,0,0), l);
    }
    
    gl_FragColor = rgba;
}
