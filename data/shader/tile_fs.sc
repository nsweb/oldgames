$input v_fs_lrud, v_fs_coord

#include <bgfx_shader.sh>

//#define DEBUG_GRID

// edgetype : None = 0, Portal = 1, Wall = 2, GhostSink = 3,

void main()
{
    vec2 p = v_fs_coord;
    vec3 col_sink = vec3(0.92,0.68,0.65);
    vec4 lrud_clamp = clamp(v_fs_lrud - vec4(1,1,1,1), vec4(0,0,0,0), vec4(1,1,1,1));
    vec4 lrud_dist = lrud_clamp * 0.1;
    vec4 col = vec4(0,0,0,1);

    // wall left
    vec3 col_left = lrud_clamp.x * mix(vec3(0,0,1), col_sink, v_fs_lrud.x - 2);
    float d = p.x;
    float l = smoothstep(0, 0.02, d - lrud_dist.x);
    col = mix(vec4(col_left,1), col, l);

    // wall right
    vec3 col_right = lrud_clamp.y * mix(vec3(0,0,1), col_sink, v_fs_lrud.y - 2);
    d = 1.f - p.x;
    l = smoothstep(0, 0.02, d - lrud_dist.y);
    col = mix(vec4(col_right,1), col, l);

    // wall up
    vec3 col_up = lrud_clamp.z * mix(vec3(0,0,1), col_sink, v_fs_lrud.z - 2);
    d = p.y;
    l = smoothstep(0, 0.02, d - lrud_dist.z);
    col = mix(vec4(col_up,1), col, l);

    // wall down
    vec3 col_down = lrud_clamp.w * mix(vec3(0,0,1), col_sink, v_fs_lrud.w - 2);
    d = 1.f - p.y;
    l = smoothstep(0, 0.02, d - lrud_dist.w);
    col = mix(vec4(col_down,1), col, l);
    
    // Round corners
    d = length(p);
    d = min(d, length(p - vec2(1,0)));
    d = min(d, length(p - vec2(0,1)));
    d = min(d, length(p - vec2(1,1)));
    l = smoothstep(0, 0.02, d - 0.1);
    col = mix(vec4(0,0,1,1), col, l);

#ifdef DEBUG_GRID
    vec2 grid = abs(p.xy - vec2(0.5,0.5))*2.0;
    grid = pow(grid, 32.0);
    float dg = smoothstep(0.0,1.0, max(grid.x, grid.y));
    col = mix(col, vec4(0.5,0.5,0.5,1.0), dg);
#endif

    gl_FragColor = col;
}
