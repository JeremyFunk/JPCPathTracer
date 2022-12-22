extern "C"
{
#include "jpc_api.h"
#include <cglm/cglm.h>
}

#include <yaml-cpp/yaml.h>

#include <vector>
#include<algorithm>


template<typename T>
T yaml_try_parse(const YAML::Node& node,T default_value)
{

    try{
        T res = node.as<T>();
        return res;
    }catch(const YAML::BadConversion&e)
    {
        return default_value;
    }
}

template<typename T,size_t S>
void yaml_try_parse_array(const YAML::Node& node,T default_value, T dest[S])
{
    try{
        std::array<T,S> res = node.as<std::array<T,S>>();
        std::copy_n(res.data(),S,dest);
        return;
    }catch(const YAML::BadConversion& e){}
    try{
        T res = yaml_try_parse(node,default_value);
        std::fill_n(dest,S,res);
        return;
    
    }catch(const YAML::BadConversion& e){}

}



template<typename T,size_t S>
void yaml_try_parse_array(const YAML::Node& node,std::array<T,S> default_value, T dest[S])
{
    std::array<T,S> res = yaml_try_parse(node,default_value);
    std::copy_n(res.data(),S,dest);
}

sun_light_t yaml_parse_sun(const YAML::Node& node)
{
   sun_light_t res; 
   yaml_try_parse_array<float,3>(node["direction"], {0.,0.,1.}, res.direction);
   yaml_try_parse_array<float,4>(node["color"], 1., res.color);
   res.strength = yaml_try_parse<float>(node["strength"],1.);
   return res;
}

point_light_t yaml_parse_pointlight(const YAML::Node& node)
{
    point_light_t res;
    yaml_try_parse_array<float,3>(node["position"],{0.,0.,0.},res.position);

   yaml_try_parse_array<float,4>(node["color"], 1., res.color);
   res.strength = yaml_try_parse<float>(node["strength"],1.);
   res.falloff = yaml_try_parse<float>(node["falloff"],1.);
   return res;
}

template<typename T>
void init_c_vector(const std::vector<T>&  vec, T** dest, uint* length )
{
    *dest = (T*) malloc(vec.size()*sizeof(T));
    *length = vec.size();
}

lights_t yaml_parse_lights(const YAML::Node& root)
{
    const YAML::Node& yaml_lights = root["lights"];
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

    for(const YAML::Node& yaml_light:yaml_lights)
    {
      std::string type = yaml_light.as<std::string>("type");
      if(type=="sun")
      {
       sun_lights.push_back(yaml_parse_sun(yaml_light));
      }
      if(type=="point")
      {
        point_lights.push_back(yaml_parse_pointlight(yaml_light));
      }
    }

    lights_t lights;
    init_c_vector(sun_lights, &lights.sun_lights, &lights.sun_lights_count);
    init_c_vector(point_lights,&lights.point_lights,&lights.point_lights_count);
    return lights;

}



scene_t scane_load_yaml(char* path)
{
}
