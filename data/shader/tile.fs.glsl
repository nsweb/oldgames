
in vec4 vs_fs_color;
in vec4 vs_fs_lrud;
in vec2 vs_fs_coord;

layout (location = 0) out vec4 color;

void main(void)
{
    float d = 2;
    if (vs_fs_lrud.x == 0)
    {
        // wall left
        d = min(d, vs_fs_coord.x);
    }
    if (vs_fs_lrud.y == 0)
    {
        // wall right
        d = min(d, 1.f - vs_fs_coord.x);
    }
    if (vs_fs_lrud.z == 0)
    {
        // wall up
        d = min(d, vs_fs_coord.y);
    }
    if (vs_fs_lrud.w == 0)
    {
        // wall down
        d = min(d, 1.f - vs_fs_coord.y);
    }
    
    d = min(d, length(vs_fs_coord));
    d = min(d, length(vs_fs_coord - vec2(1,0)));
    d = min(d, length(vs_fs_coord - vec2(0,1)));
    d = min(d, length(vs_fs_coord - vec2(1,1)));
    
    color = (d < 0.1 ? vs_fs_color : vec4(0,0,0,0));
}
