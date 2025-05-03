$input v_color0, v_fs_lrud, v_fs_coord

#include <bgfx_shader.sh>

//#define DEBUG_GRID

void main()
{
    float d = 2;
    if (v_fs_lrud.x == 0)
    {
        // wall left
        d = min(d, v_fs_coord.x);
    }
    if (v_fs_lrud.y == 0)
    {
        // wall right
        d = min(d, 1.f - v_fs_coord.x);
    }
    if (v_fs_lrud.z == 0)
    {
        // wall up
        d = min(d, v_fs_coord.y);
    }
    if (v_fs_lrud.w == 0)
    {
        // wall down
        d = min(d, 1.f - v_fs_coord.y);
    }
    
    // Round corners
    d = min(d, length(v_fs_coord));
    d = min(d, length(v_fs_coord - vec2(1,0)));
    d = min(d, length(v_fs_coord - vec2(0,1)));
    d = min(d, length(v_fs_coord - vec2(1,1)));
    vec4 col = (d < 0.1 ? v_color0 : vec4(0,0,0,0));

#ifdef DEBUG_GRID
    vec2 grid = abs(v_fs_coord.xy - vec2(0.5,0.5))*2.0;
    grid = pow(grid, 32.0);
    float dg = smoothstep(0.0,1.0, max(grid.x, grid.y));
    col = mix(col, vec4(0.5,0.5,0.5,1.0), dg);
#endif

    gl_FragColor = col;
}
