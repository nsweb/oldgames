
#ifdef SHADER_SECTION
	$input v_fs_coord

	#include <bgfx_shader.sh>
#endif

uniform vec4 u_custom_0; // ghost id, alpha, ghost dir x, ghost dir y
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

    const float dsphere = 0.3;
    float d = length(p) - dsphere;
    float bx = max(p.x - dsphere, -p.x - dsphere);
    float by = max(p.y - dsphere, -p.y);
    d = min(max(bx, by), d);
    d = max( d, p.y + cos(p.x * 32.0 - 2.0*u_custom_0.w) * 0.03 - 0.27 );
        
    float l = smoothstep(-0.01, 0.01, d);
    vec3 col = mix(vec3(1,1,1), base_col, max(1, 0.1 + u_custom_0.y));
    vec4 rgba = mix(vec4(col, u_custom_0.y), vec4(0,0,0,0), l);

    // eye
    vec2 pe = vec2(abs(p.x), p.y);
    d = length((pe - vec2(0.1, -0.1))*vec2(1.5, 1.0)) - 0.09;
    l = smoothstep(0.0, 0.02, d);
    rgba.rgb = mix(vec3(1,1,1), rgba.rgb, l);

    // eyeball
    vec2 peb = vec2(abs(p.x - 0.025), p.y);
    //vec2 peb = vec2(abs(p.x - u_custom_0.z), p.y - u_custom_0.w);
    d = length(peb - vec2(0.1, -0.075)) - 0.03;
    l = smoothstep(0.0, 0.02, d);
    rgba.rgb = mix(vec3(0,0,0), rgba.rgb, l);
    
    gl_FragColor = rgba;
}
