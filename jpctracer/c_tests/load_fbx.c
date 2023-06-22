#include<jpc_api.h>

int main()
{
    arena_t* arena = arena_make();
    char* error;
    const char* load_path = "C:\\Users\\chris\\dev\\jpc_tracer\\JPCPathTracer\\jpctracer\\c_tests\\assets\\icosphere.fbx";
    scene_t*  scene = scene_load_fbx(arena,load_path,&error);

    if(scene==NULL)
    {
        printf("Error of fbx loader: %s\n",error);
    }

    render_settings_t settings = {
        .light_samples = 2,
        .max_depth = 5,
        .passes = JPC_PASS_DEPTH|JPC_PASS_NORMAL,
        .subpixels = 1,
        .tile_size = 1,
    };

    render_and_save(scene,settings,(uint2){1920,1080},"C:\\Users\\chris\\dev\\jpc_tracer\\Results\\FBX1");

    arena_print_stats(arena);
    arena_release(arena);


}