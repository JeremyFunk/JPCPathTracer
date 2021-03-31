# Engine Api

## Shader

```cpp

int main()
{
    auto jpcbuilder = EngineBuilder()
                    .SetCamera()
                    .SetSampler()
                    .SetIntegrator();

    auto engine = jpcbuilder.Build();
    auto material_builder = [](MaterialSettings ){
        return [](context)();
    };
    engine.MaterialLib.Register("Material1",material_builder,DefaultSettings);
    engine.MaterialLib.RegisterBackground(material_builder,DefaultSettings);



    MaterialSettings material1 = engine.MaterialLib.Create("Material1");
    material1.SetFilePath("name","path");
    IGeometry sphere = LoadObj("path...");
    engine.DrawSphere(sphere,material1);
    

}

```