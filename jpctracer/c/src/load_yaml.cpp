extern "C"
{
#include "jpc_api.h"
#include <cglm/cglm.h>
}

#include <yaml-cpp/yaml.h>

#include <vector>
#include<algorithm>
#include<optional>

struct ParsingError
{
    std::string location;
    std::string message;
};

class ParsingErrors
{
  private:
    std::vector<ParsingError> m_missing;
    std::vector<ParsingError> m_bad_conversion;
    std::optional< ParsingError> m_critical;

    std::vector<std::string> m_current_location;

    std::string extract_location()
    {
        std::string result = m_current_location[0];

        for (size_t i = 1; i < m_current_location.size(); i++)
        {
            result += " -> " + m_current_location[i];
        }

        return result;
    }

    parsing_error_t parsing_err_ctype_init(const ParsingError& err)
    {
        parsing_error_t res{};
        size_t          loc_bsize = sizeof(char) * (err.location.size() + 1);
        res.location = (char*) malloc(loc_bsize);

        memcpy(res.location, err.location.data(), loc_bsize);

        size_t msg_bsize = sizeof(char) * (err.message.size() + 1);
        res.message = (char*)malloc(msg_bsize);

        memcpy(res.message, err.message.data(), msg_bsize);
        return res;

    }

public:
    void go_down(std::string child_name)
    {
        m_current_location.push_back(child_name);
    }

    void go_up()
    {
        m_current_location.pop_back();
    }




    void add_missing(std::string message)
    {
        m_missing.push_back(ParsingError{extract_location(), message});
    }

    void add_bad_conversion(std::string message)
    {
        m_bad_conversion.push_back(ParsingError{extract_location(), message}); 
    }

    void add_critical(std::string message)
    {
        m_critical = ParsingError{extract_location(), message};
    }

    bool is_cirtical() const
    {
        return m_critical.has_value();
    }


    parsing_errors_t to_ctype()
    {
        parsing_errors_t errors;
        errors.bad_conversion = (parsing_error_t*)malloc(
            sizeof(parsing_error_t) * m_bad_conversion.size());

        errors.bad_conversion_count = m_bad_conversion.size();
        for (uint i = 0; i < errors.bad_conversion_count; i++)
        {
            errors.bad_conversion[i]
                = parsing_err_ctype_init(m_bad_conversion[i]);
        }

        errors.missing = (parsing_error_t*)malloc(sizeof(parsing_error_t)
                                                  * m_missing.size());
        errors.missing_count = m_missing.size();

        for (uint i = 0; i < errors.missing_count; i++)
        {
            errors.missing[i] = parsing_err_ctype_init(m_missing[i]);
        }

        if (m_critical.has_value())
        {
            errors.critical = parsing_err_ctype_init(*m_critical);
        }
        else
        {
            errors.critical.location = NULL;
            errors.critical.message = NULL;
        }
        return errors;
        
        
    }

};

template<typename T> const char* type2name(T);

const char* type2name(std::string)
{
    return "std::string";
}

const char* type2name(const char*)
{
    return "const char*";
}

const char* type2name(float)
{
    return "float";
}

const char* type2name(int)
{
    return "int";
}


const char* type2name(std::array<float, 1>)
{
    return "std::array<float,1>";
}
const char* type2name(std::array<float, 2>)
{
    return "std::array<float,2>";
}
const char* type2name(std::array<float, 3>)
{
    return "std::array<float,3>";
}
const char* type2name(std::array<float, 4>)
{
    return "std::array<float,4>";
}

template<typename T>
T yaml_try_parse(const YAML::Node& node, T default_value, ParsingErrors& errors)
{
    T res;
    try{
         res = node.as<T>();
        
    }catch(const YAML::BadConversion&e)
    {
         std::string msg = std::string{"Could not convert to "}
                           + std::string{type2name(T{})};
        errors.add_bad_conversion(msg);
        res= default_value;
    }
    catch (const YAML::InvalidNode)
    {
        std::string msg = "Missing information";
        errors.add_missing(msg);
        res= default_value;
    }

    return res;
}

template <typename T>
T yaml_try_parse(const YAML::Node& node,
    const char* key,
    T                 default_value,
    ParsingErrors& errors)
{
    errors.go_down(key);
    T res = yaml_try_parse(node[key], default_value, errors);
    errors.go_up();
    return res;
}

template<typename T,size_t S>
void yaml_try_parse_array(const YAML::Node& node, T default_value, T dest[S], ParsingErrors& errors)
{
    try{
        std::array<T,S> res = node.as<std::array<T,S>>();
        std::copy_n(res.data(),S,dest);
        return;
        
    }
    catch (const YAML::BadConversion& e)
    {
        try
        {
            T res = yaml_try_parse(node, default_value, errors);
            std::fill_n(dest, S, res);
            return;
        }
        catch (const YAML::BadConversion& e)
        {

            std::string msg = std::string{"Could not convert to "}
                              + std::string{type2name(T{})};
        }
    }
    catch (const YAML::InvalidNode& e)
    {
        errors.add_missing("Missing information");
    }

    std::fill_n(dest, S, default_value);


}

template <typename T, size_t S>
void yaml_try_parse_array(const YAML::Node& node,
                          const char* key,
                          T                 default_value,
                          T                 dest[S],
                          ParsingErrors&    errors)
{

    errors.go_down(key);
    yaml_try_parse_array<T,S>(node[key], default_value, dest, errors);
    errors.go_up();
}



template<typename T,size_t S>
void yaml_try_parse_array(const YAML::Node& node,const char* key,std::array<T,S> default_value, T dest[S], ParsingErrors& errors)
{
    std::array<T,S> res = yaml_try_parse(node,key,default_value,errors);
    std::copy_n(res.data(),S,dest);
}



sun_light_t yaml_parse_sun(const YAML::Node& node , ParsingErrors& errors)
{
   sun_light_t res; 
   yaml_try_parse_array<float,3>(node,"direction", {0.,0.,1.}, res.direction,errors);
   
   yaml_try_parse_array<float,4>(node,"color", 1., res.color,errors);
   res.strength = yaml_try_parse<float>(node,"strength",1.,errors);
   return res;
}

point_light_t yaml_parse_pointlight(const YAML::Node& node, ParsingErrors& errors)
{
    point_light_t res;
    yaml_try_parse_array<float,3>(node,"position",{0.,0.,0.},res.position,errors);

   yaml_try_parse_array<float,4>(node,"color", 1., res.color,errors);
   res.strength = yaml_try_parse<float>(node,"strength",1.,errors);
   res.falloff = yaml_try_parse<float>(node,"falloff",1.,errors);
   return res;
}

template<typename T>
void init_c_vector(const std::vector<T>&  vec, T** dest, uint* length )
{
    *dest = (T*) malloc(vec.size()*sizeof(T));
    *length = vec.size();
    std::copy_n(vec.data(), *length, *dest);
}

class ParserException : public std::exception {

};

std::string yaml_parse_critical_string(const YAML::Node& node, ParsingErrors& errors)
{
    try
    {
        return node.as<std::string>();
    }
    catch (const YAML::BadConversion& e)
    {
        errors.add_bad_conversion(
            "This value musst be convertable to a string.");
        throw ParserException{};
    }
    catch (const YAML::InvalidNode& e)
    {
        errors.add_missing("There musst be a value.");
        throw ParserException{};
    }
}

lights_t lights_load_yaml(const YAML::Node& yaml_lights, ParsingErrors& errors)
{
    if (!yaml_lights)
    {
        return lights_t{
            .point_lights_count = 0,
            .point_lights = NULL,
            .sun_lights_count = 0,
            .sun_lights = NULL,
        };
    }



    std::vector<sun_light_t>   sun_lights;
    std::vector<point_light_t> point_lights;

    for(const auto& yaml_light:yaml_lights)
    {
      std::string light_name = yaml_light.first.as<std::string>();
      errors.go_down(light_name);

      std::string type = yaml_parse_critical_string( yaml_light.second["type"],errors);
      
      if(type=="sun")
      {
       sun_lights.push_back(yaml_parse_sun(yaml_light.second,errors));
      }
      else if(type=="point")
      {
        point_lights.push_back(yaml_parse_pointlight(yaml_light.second,errors));
      }
      else
      {
        errors.add_critical(type + "is not a valid light type.");
        throw ParserException{};
      }

      errors.go_up();
    }

    lights_t lights;
    init_c_vector(sun_lights, &lights.sun_lights, &lights.sun_lights_count);
    init_c_vector(point_lights,&lights.point_lights,&lights.point_lights_count);
    return lights;

}

shader_t* shader_find(shaders_t shaders, const char* name)
{
    for (uint i = 0; i < shaders.count; i++)
    {
        if (strcmp(shaders.shaders[i].name, name) == NULL)
        {
            return shaders.shaders+i;
        }
       
    }
    return NULL;
}

uint uniform_find_idx(const shader_t* shader, const char* name)
{
    for (uint i = 0; i < shader->uniforms_count; i++)
    {
        if (strcmp(shader->uniforms_layout[i].name, name)==0)
        {
            return i;
        }
    }
    return shader->uniforms_count;
}


void uniform_load_yaml(const YAML::Node& node,
                       uniform_type_t    type,
                       float*            dst,
                       ParsingErrors& errors)
{
    switch (type)
    {
    case JPC_float:
        yaml_try_parse_array<float, 1>(node, 0., dst,errors);
        break;
    case JPC_float3:
        yaml_try_parse_array<float, 3>(node, 0., dst, errors);
        break;
    case JPC_float4:
        yaml_try_parse_array<float, 4>(node, 0., dst, errors);
        break;
    default:
        break;
    }
}

void material_set_uniforms_yaml(const YAML::Node& material_node, const shader_t* shader, material_t* material, ParsingErrors& errors)
{

    float uniform_value_bfr[4];
    for (const auto& key_val : material_node)
    {
        auto key = key_val.first.as<std::string>();

        if (key == "shader")
        {
            continue;
        }

        uint uniform_id = uniform_find_idx(shader, key.data());

        if (uniform_id >= shader->uniforms_count)
        {
            errors.go_down(key);
            errors.add_bad_conversion(
                std::string{"Shader "} + std::string{shader->name}
                + std::string{" has no property "} + key + std::string{"."});

            errors.go_up();
            continue;
        }

        auto value = key_val.second;

        errors.go_down(key);
        uniform_load_yaml(
            value, shader->uniforms_layout[uniform_id].type, uniform_value_bfr, errors);
        errors.go_up();

        material_set_uniform(material, shader, uniform_id, uniform_value_bfr);
    }
}

using MaterialIndexMap = std::unordered_map<std::string,uint>;

materiallib_t materiallib_load_yaml(const YAML::Node& node, 
    MaterialIndexMap& material_names, ParsingErrors& errors)
{
    shaders_t all_shaders = shaders_init();
    shaders_load_defaults(all_shaders);

    std::vector<shader_t> shader_per_materials;

    for (const auto& material_node : node)
    {
        std::string shader_name = material_node.second["shader"].as<std::string>();
        shader_t*   shader = shader_find(all_shaders, shader_name.data());

        if (shader == NULL)
        {
            std::string material_name = material_node.first.as<std::string>();
            errors.go_down(material_name);
            errors.add_bad_conversion(
                std::string{"Shader "} + shader_name
                + std::string{" is unkown. The default shader "}
                + std::string{all_shaders.shaders->name} + " will be used.");
            errors.go_up();
            shader = all_shaders.shaders;
        }
        shader_per_materials.push_back(*shader);

    }

    materiallib_t matlib;

    matlib.materials_n = shader_per_materials.size();

    matlib.materials
        = (material_t*)malloc(matlib.materials_n * sizeof(material_t));

    matlib.buffer = materials_init(
        matlib.materials, shader_per_materials.data(), matlib.materials_n);


    uint i = 0;


    for (const auto& material_node : node)
    {
        const shader_t* shader = &shader_per_materials[i];
        material_t*     material = matlib.materials + i;
        
        material_set_uniforms_yaml(material_node.second, shader, material,errors);
        std::string material_name = material_node.first.as<std::string>(); 
        material_names[material_name] = i;
        i++;
    }
    
    shaders_free(all_shaders);

    return matlib;

    
}




sphere_mesh_t sphere_mesh_load_ymal(const YAML::Node& node,const MaterialIndexMap& material_map ,ParsingErrors& errors)
{
    sphere_mesh_t mesh{};
    mesh.count = 1;

    mesh.geometries = (sphere_geometry_t*)malloc(sizeof(sphere_geometry_t));

    mesh.geometries[0].radius = yaml_try_parse<float>(node,"radius", 0.,errors);
    
    mesh.material_ids = (uint*)malloc(sizeof(uint));

    errors.go_down("material");
    std::string material_name = yaml_parse_critical_string( node["material"],errors);
    errors.go_up();

    mesh.material_ids[0] = material_map.at(material_name);
    

    yaml_try_parse_array<float, 3>(
        node, "position", 0., mesh.geometries[0].position, errors);

    printf("debug %p count: %d\n",&mesh, mesh.count);

    mesh.bvh_tree = bvhtree_spheres_build(&mesh);
    return mesh;
}

void idenity_transform(float4x4 transform)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (i == j )
            {
                transform[i][j] = 1;
            }
            else
            {
                transform[i][j] = 0;
            }
        }
    }
}

geometries_t geometries_load_yaml(const YAML::Node& node,const MaterialIndexMap& material_map,ParsingErrors& errors)
{
    std::vector<instance_t> instances;
    std::vector<sphere_mesh_t>   spheres;

    for (const auto& instance_node : node)
    {
        instance_t  instance;


        std::string key_name = instance_node.first.as<std::string>();

        errors.go_down(key_name);
    
        const YAML::Node& tmp = instance_node.second["type"];


        errors.go_down("type");
        std::string type_name = yaml_parse_critical_string(tmp, errors);
        errors.go_up();

        if (type_name == "sphere")
        {
            instance.type = JPC_SPHERE;
            sphere_mesh_t sphere_mesh = sphere_mesh_load_ymal(instance_node.second,material_map,errors);

            instance.mesh_id = spheres.size();
            
            spheres.push_back(sphere_mesh);
            
            idenity_transform(instance.transformations);

        
        }
        else
        {
            errors.add_critical(type_name + " is an unkown geometry type.");
            throw ParserException{};
        }
        instances.push_back(instance);
        errors.go_up();
    }

    geometries_t geoms;

    geoms.instances_count = instances.size();
    geoms.instances
        = (instance_t*)malloc(geoms.instances_count * sizeof(instance_t));

    init_c_vector(instances, &geoms.instances, &geoms.instances_count);

    init_c_vector(spheres, &geoms.spheres, &geoms.sphere_mesh_count);

    geoms.triangles = NULL;
    geoms.triangle_mesh_count = 0;

    geoms.bvhtree_instances = bvhtree_instances_build(&geoms);

    return geoms;
    
}

camera_t camera_load_yaml(const YAML::Node& node,ParsingErrors& errors)
{
    camera_t cam;
    cam.clip_end = yaml_try_parse<float>(node,"clip_end", 400.,errors);
    //TODO
    cam.near_plane = yaml_try_parse<float>(node,"near_plane", 0.1,errors);
    
    /*
    yaml_try_parse_array<float, 3>(
        node,"direction", {1., 0, 0}, cam.direction,errors);

    glm_vec3_normalize(cam.direction);
    */

    yaml_try_parse_array<float, 3>(
        node,"position", {0., 0., 0.}, cam.position,errors);

    return cam;
}

scene_t scene_init()
{
    scene_t res;
    res.geometries.bvhtree_instances = NULL;
    res.geometries.instances = NULL;
    res.geometries.spheres = NULL;
    res.geometries.triangles = NULL;
    res.lights.point_lights = NULL;
    res.lights.sun_lights = NULL;
    res.materiallib.buffer = NULL;
    res.materiallib.materials = NULL;
    res.materiallib.textures = NULL;
    return res;
}

parsing_errors_t scene_load_yaml(const char* path, scene_t* scene)
{

    YAML::Node node = YAML::LoadFile(path);

    ParsingErrors errors;

    *scene = scene_init();


    try
    {

        errors.go_down("CAMERA");
        scene->camera = camera_load_yaml(node["CAMERA"],errors);
        errors.go_up();

        MaterialIndexMap material_names;
        errors.go_down("MATERIALS");
        scene->materiallib
            = materiallib_load_yaml(node["MATERIALS"], material_names,errors);
        errors.go_up();

        errors.go_down("GEOMETRIES");
        scene->geometries
            = geometries_load_yaml(node["GEOMETRIES"], material_names,errors);
        errors.go_up();
        
        errors.go_down("LIGHTS");
        scene->lights = lights_load_yaml(node["LIGHTS"],errors);
        errors.go_up();
        return errors.to_ctype();
    }
    catch (const ParserException& e)
    {
    }
    catch (const YAML::BadConversion& e)
    {
        errors.add_critical("YAML BadConversion");
    }
    catch (const YAML::InvalidNode& e)
    {
        errors.add_critical("YAML Invaild Node");
    }


    return errors.to_ctype();
}


void parsing_error_free( parsing_error_t* err)
{
    free(err->location);
    free(err->message);
}

void parsing_error_arr_free(parsing_error_t* errs, uint n)
{
    for (uint i = 0; i < n; i++)
    {
        parsing_error_free(errs + i);
    }
}

void parsing_errors_free(parsing_errors_t errors)
{
    parsing_error_arr_free(errors.bad_conversion, errors.bad_conversion_count);
    parsing_error_arr_free(errors.missing, errors.missing_count);
    parsing_error_free(&errors.critical);
}
int  parsing_errors_is_critical(const parsing_errors_t* errors)
{
    return errors->critical.location != NULL;
}

void parsing_errors_print(const parsing_errors_t* errors,
                          int                     bad_conversion,
                          int                     missing)
{
    if (bad_conversion)
    {
        printf("======== Bad Conversion ========\n");
        for (uint i = 0; i < errors->bad_conversion_count; i++)
        {
            printf("%s:\n", errors->bad_conversion[i].location);
            printf("\t%s\n", errors->bad_conversion[i].message);
        }
    }
    if (missing)
    {
        printf("============ Missing ===========\n");
        for (uint i = 0; i < errors->missing_count; i++)
        {
            printf("%s:\n", errors->missing[i].location);
            printf("\t%s\n", errors->missing[i].message);
        }
    }
    
    printf("=========== Critical ===========\n");
    if (parsing_errors_is_critical(errors))
    {
        printf("%s:\n", errors->critical.location);
        printf("\t%s\n", errors->critical.message);
    }
    printf("================================\n");
}
