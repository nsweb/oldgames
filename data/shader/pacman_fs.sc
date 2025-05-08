
#ifdef SHADER_SECTION
	$input v_fs_coord

	#include <bgfx_shader.sh>
#endif

uniform vec4 u_custom_0; // angle
uniform vec4 u_custom_1;

#define PI 3.1415926536

void main()
{
	// pacman
    const float dsphere = 0.3;
    vec2 p = v_fs_coord;
    float d = length(p) - dsphere;
    
    float angle = u_custom_0.x;//40.0 * PI / 180.0 * (0.5 + 0.5 * sin(8.0*time));
    float cf = cos(angle), sf = sin(angle);
    vec2 nup = vec2(sf, -cf);
    vec2 ndn = vec2(sf, cf);
    d = max(d, min(dot(p, nup), dot(p, ndn)));
    float l = smoothstep(0, 0.02, d);
    vec4 rgba = mix(vec4(1,0.9,0.1,1), vec4(0,0,0,0), l);

    gl_FragColor = rgba;
}
