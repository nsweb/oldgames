
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
    vec4 position = pos;
    position.z = -0.02;
    gl_Position = proj_mat * (view_mat * (world_mat * position));
    vs_fs_coord = pos.xy;
}
