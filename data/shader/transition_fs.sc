
#ifdef SHADER_SECTION
	$input v_fs_pos

	#include <bgfx_shader.sh>
#endif

uniform vec4 u_transition; // transition_type, transition_pos.xy, transition_time

void main()
{
    float d = length( v_fs_pos - u_transition.yz);
    gl_FragColor = vec4(0.0, 0.0, 0.0, d < u_transition.w ? 0.0 : 0.8);
}
