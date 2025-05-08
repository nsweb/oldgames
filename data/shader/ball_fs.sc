
$input v_color0, v_fs_bcrd, v_fs_coord

#include <bgfx_shader.sh>

void main()
{
    // balls
    const float dball = 0.05;
    const float dbigball = 0.12;
    vec2 p = v_fs_coord;
    
    // center
    float d = length(p - vec2(0.25, 0.25)) - mix(dball, dbigball, v_fs_bcrd.x) + 1 - v_fs_bcrd.y;
    float l = smoothstep(0, 0.02, d);
    vec3 colc = mix(vec3(1,1,1), vec3(1,1,0.6), v_fs_bcrd.x);
    vec4 rgba = mix(vec4(colc,1), vec4(0,0,0,0), l);

    // right
    d = length(p - vec2(0.75, 0.25)) - dball + 1 - v_fs_bcrd.z;
    l = smoothstep(0, 0.02, d);
    rgba.rgb = mix(vec3(1,1,1), rgba.rgb, l);

    // down
    d = length(p - vec2(0.25, 0.75)) - dball + 1 - v_fs_bcrd.w;
    l = smoothstep(0, 0.02, d);
    rgba.rgb = mix(vec3(1,1,1), rgba.rgb, l);

    gl_FragColor = rgba;
}
