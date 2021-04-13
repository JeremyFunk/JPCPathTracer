#Renderer Api

```cpp
int main()
{
    OrthoCameraSettings camera;
    camera.FocalLength = 2;

    DirectIntegratorSettings integrator;
    integrator.MaxGlossyRays = 20;
    integrator.SubSamples = 25;

    auto jpcbuilder = RendererBuilder()
                    .SetCamera(camera)
                    .SetSampler(StratifiedSamplerSettings())
                    .SetIntegrator(integrator);

    auto renderer = jpcbuilder.Build();


    renderer.MaterialLib.Register(
        "Material2", //Name
        [=](MaterialSettings settings) //Builder
        {
            return ShaderBind(Uber,
                settings.GetTexture("Color"),
                settings.GetTexture("Roughness"),
                settings.GetTexture("Metalic"),
                settings.GetValue("Fresnel"));
        },
        {   //Default Settings
            {"Color","pink"},
            {"Fresnel",1.45}
        }
    );

    renderer.MaterialLib.RegisterBackground(material_builder,DefaultSettings);

    
    renderer.TileSize = 16;
    renderer.Acceleration.DynamicBVH = LBVH;

    MaterialSettings material1 = renderer.MaterialLib.Create("Material2");
    
    material1.SetMultiple(
        {"Color","Path"},
        {"Roughness","Path"},
        {"Metalic","Path"},
        {"Fresnel",1.33}
    );

    IGeometry sphere = LoadObj("path...");

    spherer.SetMaterial(0,material1);
    
    Transformation trans1 = {{-1,-2,3},{2,2,2},{30,90,180}};
    Transformation trans2 = {{-1,5,3},{1,2,2},{90,90,180}};
    
    Light = PointLight({0,0,20});
    Light.Material.SetValue("Influence",100);

    Light.Material = renderer.MaterialLib.Create("SuperSun");
    Light.Material.SetValue("Influence",100);
    

    renderer.Draw(sphere,material1,trans1);
    renderer.Draw(sphere,material1,trans2);
    renderer.Draw(Light);
    
    Image result = renderer.Render(width,height);
    result.Save("dest");
}
```


```cpp
int main()
{
    auto renderer = RendererBuilder.Build();
    MaterialSettings material1 = renderer.MaterialLib.Create("Plastik");
    
    material1.SetTexture("Color","path");

    IGeometry sphere = Sphere({1,2,3},4);
    sphere.SetMaterial(0,material1);

    renderer.Draw(sphere);
    renderer.Draw(PointLight({1,2,0},200))
    
    Image result = renderer.Render();
    result.Save("dest");
}
```


```cpp
int main()
{
    auto renderer = RendererBuilder.Build();
    MaterialSettings material1 = renderer.MaterialLib.Create(ShaderBind(Uber,Red(),0,1.33));

    IGeometry sphere = Sphere({1,2,3},4);
    sphere.SetMaterial(0,material1);

    renderer.Draw(sphere);
    renderer.Draw(PointLight({1,2,0},200))
    
    Image result = renderer.Render();
    result.Save("dest");
}
```