
$input a_position, i_data0
$output v_fs_lrud, v_fs_coord, v_color0

uniform vec4 u_tile_dim;

#include <bgfx_shader.sh>

void main()
{
    float y = float (gl_InstanceID / (int)u_tile_dim.x);
    float x = float (gl_InstanceID % (int)u_tile_dim.x);
    vec4 position = vec4(a_position.x + x, a_position.y + y, a_position.z, a_position.w);
    
    vec4 view_pos = mul(u_modelView, position);
    gl_Position = mul(u_proj, view_pos);
    
    v_color0 = vec4(0.0, 0.0, 1.0, 1.0);
    v_fs_lrud = i_data0;
    v_fs_coord = a_position.xy;
}
