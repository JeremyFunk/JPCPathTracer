#include<jpc_api.h>
#include<assert.h>
#include<malloc.h>
#include<stdio.h>


int main()
{

    int*        test = _aligned_malloc(sizeof(double)*4, 4);
    _aligned_free(test);
    const char* path
        = "C:\\Users\\thoma\\source\\repos\\JPCPathTracer\\jpctracer\\c_tests\\scene.yaml";
    path = "C:\\Users\\chris\\dev\\jpc_tracer\\JPCPathTracer\\jpctracer\\c_tests\\scene.yaml";
    scene_t scene;

    arena_t* scene_arena = arena_make();  
    arena_t* error_arena = arena_make();

    parsing_errors_t errs = scene_load_yaml(
        scene_arena,
        error_arena,
        path,&scene
        );

    parsing_errors_print(&errs, 1, 1);

    render_settings_t settings;
    settings.light_samples = 2;
    settings.max_depth = 1;
    settings.subpixels = 1;
    settings.tile_size = 1;

    render_and_save(&scene,settings,(uint2){1,1},"C:\\Users\\chris\\dev\\jpc_tracer\\Results\\YAML1");

    printf("Mem used %d\n",arena_memory_used(scene_arena));
    printf("Mem allocted %d\n",arena_memory_allocated(scene_arena));

    arena_release(scene_arena);
    arena_release(error_arena);





}