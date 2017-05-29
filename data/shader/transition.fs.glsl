
uniform int transition_type;
uniform vec2 transition_pos;
uniform float transition_time;
//in vec4 vs_fs_color;
//in vec4 vs_fs_lrud;
//in vec2 vs_fs_coord;

layout (location = 0) out vec4 color;

in vec2 vs_fs_pos;

void main(void)
{
    float d = length( vs_fs_pos - transition_pos);
    color = vec4(0.0, 0.0, 0.0, d < transition_time ? 0.0 : 0.8);
}
