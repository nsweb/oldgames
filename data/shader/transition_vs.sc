
$input a_position
$output v_fs_pos

#include <bgfx_shader.sh>

void main()
{
    v_fs_pos = a_position;
    gl_Position = vec4(a_position.xy,0.0,1.0);
}
