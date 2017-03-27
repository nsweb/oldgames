
in vec2 vs_fs_coord;

uniform float custom_0;

layout (location = 0) out vec4 color;

#define PI 3.1415926536

void main(void)
{
	// pacman
    const float dsphere = 0.3;
    vec2 p = vs_fs_coord;// - vec2(0.5, 0.5);
    float d = length(p) - dsphere;
    
    float bx = max(p.x - dsphere, -p.x - dsphere);
    float by = max(p.y - dsphere, -p.y);
    d = min(max(bx, by), d);
    vec4 rgba = mix(vec4(1,0.1,0.1,1), vec4(1,0.1,1,1), custom_0);
    
    color = (d < 0.0 ? rgba : vec4(0.0,0,0,0.0));
}
