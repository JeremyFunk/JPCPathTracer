#include<jpc_api.h>
#include<stdio.h>

int main()
{
    arena_t* arena = arena_make();
    char* error;
    const char* load_path = "C:\\Users\\chris\\dev\\jpc_tracer\\JPCPathTracer\\jpctracer\\c_tests\\assets\\icosphere.fbx";
    load_path = "C:\\Users\\thoma\\source\\repos\\JPCPathTracer\\assets\\two_lights.fbx";

    const char* out_dir = "C:\\Users\\chris\\dev\\jpc_tracer\\Results\\FBX1";
    out_dir = "C:\\Users\\thoma\\Desktop\\raytracing\\two_lights";

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

    render_and_save(scene,settings,(uint2){128,128},out_dir);

    arena_print_stats(arena);
    arena_release(arena);


}