
$input a_position
$output v_fs_coord

#include <bgfx_shader.sh>

void main()
{
    vec4 position = vec4(a_position, -0.02, 1.0);
    
    vec4 view_pos = mul(u_modelView, position);
    gl_Position = mul(u_proj, view_pos);
    v_fs_coord = a_position;
}
