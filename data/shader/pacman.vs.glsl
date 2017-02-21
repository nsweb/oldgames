
//layout (std140, binding = 0) uniform WVP
//{
	uniform mat4 world_mat;
	uniform mat4 view_mat;
	uniform mat4 proj_mat;
//};


layout (location = 0) in vec4 pos;
out vec2 vs_fs_coord;

void main(void)
{
    gl_Position = proj_mat * (view_mat * (world_mat * pos));
    vs_fs_coord = pos.xy;
}
