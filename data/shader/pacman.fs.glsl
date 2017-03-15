
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
    
    float angle = custom_0;//40.0 * PI / 180.0 * (0.5 + 0.5 * sin(8.0*time));
    float cf = cos(angle), sf = sin(angle);
    vec2 nup = vec2(sf, -cf);
    vec2 ndn = vec2(sf, cf);
    //d = dot(p, nup);
    d = max(d, min(dot(p, nup), dot(p, ndn)));
    color = (d < 0.0 ? vec4(1,0.9,0.1,1) : vec4(0,0,0,0));
}
