
in vec4 vs_fs_color;
in vec4 vs_fs_bcrd;
in vec2 vs_fs_coord;

layout (location = 0) out vec4 color;

void main(void)
{
    // balls
    const float dsphere = 0.05;
    float d = 2;
    if (vs_fs_bcrd.y > 0)   // center
        d = min(d, length(vs_fs_coord - vec2(0.25, 0.25))) - (vs_fs_bcrd.x > 0 ? dsphere*0.35 : 0.0);
    if (vs_fs_bcrd.z > 0)   // right
        d = min(d, length(vs_fs_coord - vec2(0.75, 0.25)));
    if (vs_fs_bcrd.w > 0)   // down
        d = min(d, length(vs_fs_coord - vec2(0.25, 0.75)));
    
    color = (d < dsphere ? vec4(1,1,1,1) : vec4(0,0,0,0));
    //color = (vec4(1,1,1,1));
}
