
//layout (std140, binding = 0) uniform WVP
//{
	uniform mat4 world_mat;
	uniform mat4 view_mat;
	uniform mat4 proj_mat;
    uniform int tile_dim_x;
//};


layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 lrud; // left-right-up-down
//layout (location = 2) in mat4 world_mat;

out vec4 vs_fs_color;
out vec4 vs_fs_lrud;
out vec2 vs_fs_coord;

void main(void)
{
    float y = float (gl_InstanceID / tile_dim_x);
    float x = float (gl_InstanceID % tile_dim_x);
    vec4 position = vec4(pos.x + x, pos.y + y, pos.z, pos.w);
    
    gl_Position = proj_mat * (view_mat * (world_mat * position));
    
    vs_fs_color = vec4(0.0, 0.0, 1.0, 1.0);
    vs_fs_lrud = lrud;
    vs_fs_coord = pos.xy;
}
