
$input v_color0, v_fs_bcrd, v_fs_coord

#include <bgfx_shader.sh>

void main()
{
    // balls
    const float dball = 0.05;
    const float dbigball = 0.12;
    float d = 2;
    vec4 ball_color = vec4(1,1,1,1);
    
    if (v_fs_bcrd.y > 0)   // center
    {
        if (v_fs_bcrd.x > 0)
        {
            d = length(v_fs_coord - vec2(0.25, 0.25)) - dbigball;
            if (d < 0.0)
            {
                gl_FragColor = vec4(1,1,0.6,1);
                return;
            }
        }
        else
            d = length(v_fs_coord - vec2(0.25, 0.25)) - dball;
    }
    
    if (v_fs_bcrd.z > 0)   // right
        d = min(d, length(v_fs_coord - vec2(0.75, 0.25)) - dball);
    if (v_fs_bcrd.w > 0)   // down
        d = min(d, length(v_fs_coord - vec2(0.25, 0.75)) - dball);
    
    gl_FragColor = (d < 0.0 ? ball_color : vec4(0,0,0,0));
}
