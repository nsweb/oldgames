
in vec2 vs_fs_coord;

layout (location = 0) out vec4 color;

void main(void)
{
	// pacman
    const float dsphere = 0.35;
    float d = length(vs_fs_coord - vec2(0.5, 0.5));
    color = (d < dsphere ? vec4(1,0.9,0.1,1) : vec4(0,0,0,0));
}