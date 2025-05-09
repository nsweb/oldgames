
#ifdef SHADER_SECTION
	$input v_fs_coord

	#include <bgfx_shader.sh>
#endif

uniform vec4 u_custom_0; // ghost id, blip, flee, dead
uniform vec4 u_custom_1; // move x, move y, ghost wave

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
    float blink = step(0.5, u_custom_0.y);
    const float flee = u_custom_0.z;
    const float dead = u_custom_0.w;

    vec2 p = v_fs_coord;
    vec3 base_col = hsv2rgb(palette(u_custom_0.x*0.05));
    vec3 flee_col = mix(vec3(1,1,1), vec3(0,0,1), blink);
    vec3 body_col = mix(base_col, flee_col, flee);
    vec3 eye_col = mix(vec3(1,1,1), body_col, flee - dead);
    vec3 eyeball_flee_col = mix(vec3(1,0,0), vec3(1,0.5,0.1), blink);
    vec3 eyeball_col = mix(vec3(0,0,0), eyeball_flee_col, flee - dead);
    vec3 mouth_col = mix(base_col, eyeball_col, flee);

    const float dsphere = 0.3;
    float d = length(p) - dsphere;
    float bx = max(p.x - dsphere, -p.x - dsphere);
    float by = max(p.y - dsphere, -p.y);
    d = min(max(bx, by), d);
    d = max( d, p.y + cos(p.x * 32.0 - 2.0*u_custom_1.z) * 0.03 - 0.27 );

    float l = smoothstep(-0.01, 0.01, d + u_custom_0.w);
    vec4 rgba = mix(vec4(body_col, 1), vec4(0,0,0,0), l);

    // eye
    vec2 pe = vec2(abs(p.x), p.y);
    d = length((pe - vec2(0.1, -0.1))*vec2(1.5, 1.0)) - 0.09;
    l = smoothstep(0.0, 0.02, d);
    rgba.rgb = mix(eye_col, rgba.rgb, l);

    // eyeball
    //vec2 peb = vec2(abs(p.x - 0.025), p.y);
    vec2 peb = vec2(abs(p.x - u_custom_1.x*0.023), p.y + u_custom_1.y*0.05);
    d = length(peb - vec2(0.1, -0.09)) - 0.03;
    l = smoothstep(0.0, 0.02, d);
    rgba.rgb = mix(eyeball_col, rgba.rgb, l);

    // mouth
    d = max(abs(p.x) - 0.22, abs(p.y + sin(p.x * 32.0) * 0.035 - 0.15));
    l = smoothstep(0.0, 0.02, d);
    rgba.rgb = mix(mouth_col, rgba.rgb, l);
    
    gl_FragColor = rgba;
}
