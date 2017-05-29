
//layout (std140, binding = 0) uniform WVP
//{
	//uniform mat4 world_mat;
	//uniform mat4 view_mat;
	//uniform mat4 proj_mat;
    //uniform int tile_dim_x;
//};

layout (location = 0) in vec2 pos;

out vec2 vs_fs_pos;

void main(void)
{
    vs_fs_pos = pos;
    gl_Position = vec4(pos.xy,0.0,1.0);
}
