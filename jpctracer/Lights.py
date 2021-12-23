from jpctracer.types import point_light_t, sun_light_t, lights_t

class PointLight:
    def __init__(self,lights, color,strength,falloff,position):
        self.color = color
        self.strength = strength
        self.falloff = falloff
        self.position = position
        self._lights = lights
    
    def _get_ctype(self):
        result = point_light_t()
        for i in range(4):
            result.color[i] = self.color[i]
        result.strength = self.strength
        result.falloff = self.falloff
        for i in range(3):
            result.position[i] = self.position[i]
        return result
    def __del__(self):
        self._lights._pointlights.remove(self)

class SunLight:

    def __init__(self,lights, color,strength,direction):
        self.color = color
        self.strength = strength
        self.direction = direction
        self._lights = lights
    
    def _get_ctype(self):
        result = sun_light_t()
        for i in range(4):
            result.color[i] = self.color[i]
        result.strength = self.strength
        for i in range(3):
            result.direction[i] = self.direction[i]
        return result

    def __del__(self):
        self._lights._sunlights.remove(self)
    


class Lights:
    _pointlights = []
    _sunlights = []

    def add_pointlight(self,color,strength,falloff,position):
        result = PointLight(self,color,strength,falloff,position)
        self._pointlights.append(result)
        return result 
    
    def add_sunlight(self,color,strength,direction):
        result = SunLight(self,color,strength,direction)
        self._sunlights.append(result)
        return result
    
    def _get_ctype(self) -> lights_t:
        point_lights_n = len(self._pointlights)
        sun_light_n = len(self._sunlights)
        self._c_pointlights = point_light_t * point_lights_n
        self._c_sunlights = sun_light_t * sun_light_n
        for i in range(point_lights_n):
            self._c_pointlights[i] = self._pointlights[i]._get_ctype()
        for i in range(sun_light_n):
            self._c_sunlights[i] = self._sunlights[i]._get_ctype()
        
        result = lights_t()
        result.point_lights_count = point_lights_n
        result.point_lights = self._c_sunlights
        result.sun_lights_count = sun_light_n
        result.sun_lights = self._c_sunlights
        return result
