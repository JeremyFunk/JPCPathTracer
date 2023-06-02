#include<jpc_api.h>
#include<assert.h>
#include<malloc.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include"stb/stb_image_write.h"

int main()
{

    int*        test = _aligned_malloc(sizeof(double)*4, 4);
    _aligned_free(test);
    const char* path1
        = "C:\\Users\\thoma\\source\\repos\\JPCPathTracer\\jpctracer\\c_tests\\scene.yaml";
    const char* path2 = "C:\\Users\\thoma\\source\\repos\\JPCPathTracer\\jpctracer\\c_tests\\test_yaml.yaml";
    scene_t scene;
    parsing_errors_t errs = scene_load_yaml(
        path1,&scene
        );

    parsing_errors_print(&errs, 1, 1);

    render_settings_t settings;
    settings.light_samples = 2;
    settings.max_depth = 3;
    settings.subpixels = 1;
    settings.tile_size = 16;
    image_t image;
    image.channels = 4;
    image.height = 256;
    image.width = 256;
    image.data = (float*)malloc(image.height * image.width * image.channels* sizeof(float));
    render(&scene, settings,&image);

    float max_val = 0;
    for (uint i = 0; i < image.width * image.height * image.channels; i++)
    {
        max_val = max(max_val, image.data[i]);
    }



    uint8_t* uint8_data = (uint8_t*)malloc(image.height * image.width * image.channels
                                * sizeof(uint8_t));

    for (uint i = 0; i < image.width * image.height * image.channels; i++)
    {
        uint8_data[i] = (uint8_t) (min(image.data[i],1.) * 255.);
    }
    

    stbi_write_png("C:\\Users\\thoma\\Desktop\\tmp.png",image.width,image.height,image.channels,uint8_data,
       image.width*image.channels*sizeof(uint8_t));

    free(uint8_data);
    free(image.data);
    





}