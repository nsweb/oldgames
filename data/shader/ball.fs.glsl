
in vec4 vs_fs_color;
in vec4 vs_fs_bcrd;
in vec2 vs_fs_coord;

layout (location = 0) out vec4 color;

void main(void)
{
    // balls
    const float dball = 0.05;
    const float dbigball = 0.08;
    float d = 2;
    vec4 ball_color = vec4(1,1,1,1);
    
    if (vs_fs_bcrd.y > 0)   // center
    {
        if (vs_fs_bcrd.x > 0)
        {
            d = length(vs_fs_coord - vec2(0.25, 0.25)) - dbigball;
            if (d < 0.0)
            {
                color = vec4(1,1,0.6,1);
                return;
            }
        }
        else
            d = length(vs_fs_coord - vec2(0.25, 0.25)) - dball;
    }
    
    if (vs_fs_bcrd.z > 0)   // right
        d = min(d, length(vs_fs_coord - vec2(0.75, 0.25)) - dball);
    if (vs_fs_bcrd.w > 0)   // down
        d = min(d, length(vs_fs_coord - vec2(0.25, 0.75)) - dball);
    
    color = (d < 0.0 ? ball_color : vec4(0,0,0,0));
}
