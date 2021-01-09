#pragma once
#include "Eigen/Dense"
#include "Eigen/src/Core/Matrix.h"
#include "Linalg.h"
#include <array>
#include <sys/stat.h>
#include <vector>
#include <algorithm>



namespace core {


    template<size_t Dim>
    class SpectrumCoefficient
    {
    public:
        //Mathe operationen + - * / sqrt pow interpolation 
        SpectrumCoefficient() : _data({0,0,0}){

        }
        
    protected:
        std::array<Prec, Dim> _data;
    };

    class RGBSpectrum;

    class RGBSpectrum : public SpectrumCoefficient<3>
    {
        public:

        static RGBSpectrum FromRGB(const Vec3& rgb){
            RGBSpectrum r = RGBSpectrum();

            r._data[0] = rgb[0];
            r._data[1] = rgb[1];
            r._data[2] = rgb[2];

            return r;
        }

        static RGBSpectrum FromValue(const Prec& value)
        {
            RGBSpectrum r = RGBSpectrum();
            r._data[0] = value;
            r._data[1] = value;
            r._data[2] = value;

            return r;
        }

        Vec3 ToRGB(){
            return Vec3(_data[0], _data[1], _data[2]);
        }

        
        RGBSpectrum &operator+=(const RGBSpectrum &s2){
            for(int i = 0; i < 3; i++)
                _data[i] += s2._data[i];
            return *this;
        }
        RGBSpectrum &operator-=(const RGBSpectrum &s2){
            for(int i = 0; i < 3; i++)
                _data[i] -= s2._data[i];
            return *this;
        }
        RGBSpectrum &operator*=(const RGBSpectrum &s2){
            for(int i = 0; i < 3; i++)
                _data[i] *= s2._data[i];
            return *this;
        }
        RGBSpectrum &operator/=(const RGBSpectrum &s2){
            for(int i = 0; i < 3; i++)
                _data[i] /= s2._data[i];
            return *this;
        }

        RGBSpectrum operator+(const RGBSpectrum &s2) const{
            RGBSpectrum ret = *this;
            for(int i = 0; i < 3; i++)
                ret._data[i] += s2._data[i];
            return ret;
        }
        RGBSpectrum operator-(const RGBSpectrum &s2) const{
            RGBSpectrum ret = *this;
            for(int i = 0; i < 3; i++)
                ret._data[i] -= s2._data[i];
            return ret;
        }
        RGBSpectrum operator*(const RGBSpectrum &s2) const{
            RGBSpectrum ret = *this;
            for(int i = 0; i < 3; i++)
                ret._data[i] *= s2._data[i];
            return ret;
        }
        RGBSpectrum operator/(const RGBSpectrum &s2) const{
            RGBSpectrum ret = *this;
            for(int i = 0; i < 3; i++)
                ret._data[i] /= s2._data[i];
            return ret;
        }


        
        
        RGBSpectrum &operator+=(const Prec &s2){
            for(int i = 0; i < 3; i++)
                _data[i] += s2;
            return *this;
        }
        RGBSpectrum &operator-=(const Prec &s2){
            for(int i = 0; i < 3; i++)
                _data[i] -= s2;
            return *this;
        }
        RGBSpectrum &operator*=(const Prec &s2){
            for(int i = 0; i < 3; i++)
                _data[i] *= s2;
            return *this;
        }
        RGBSpectrum &operator/=(const Prec &s2){
            for(int i = 0; i < 3; i++)
                _data[i] /= s2;
            return *this;
        }

        RGBSpectrum operator+(const Prec &s2) const{
            RGBSpectrum ret = *this;
            for(int i = 0; i < 3; i++)
                ret._data[i] += s2;
            return ret;
        }
        RGBSpectrum operator-(const Prec &s2) const{
            RGBSpectrum ret = *this;
            for(int i = 0; i < 3; i++)
                ret._data[i] -= s2;
            return ret;
        }
        RGBSpectrum operator*(const Prec &s2) const{
            RGBSpectrum ret = *this;
            for(int i = 0; i < 3; i++)
                ret._data[i] *= s2;
            return ret;
        }
        RGBSpectrum operator/(const Prec &s2) const{
            RGBSpectrum ret = *this;
            for(int i = 0; i < 3; i++)
                ret._data[i] /= s2;
            return ret;
        }

        
        RGBSpectrum Sqrt(){
            for(int i = 0; i < 3; i++)
                _data[i] = std::sqrt(_data[i]);
            return *this;
        }
        RGBSpectrum Pow(Prec pow){
            for(int i = 0; i < 3; i++)
                _data[i] = std::pow(_data[i], pow);
            return *this;
        }
        inline RGBSpectrum Lerp(Prec t, const RGBSpectrum &s){
            for(int i = 0; i < 3; i++)
                _data[i] = _data[i] * (1 - t) + s._data[i] * t; 
            return *this;
        }

        
        RGBSpectrum Clamp(Prec low, Prec high) {
            for(int i = 0; i < 3; i++)
                _data[i] = std::clamp(_data[i], low, high);
            return *this;
        }
    };

    // RGBSpectrum sqrt(const RGBSpectrum &s){
    //     RGBSpectrum ret = s;
    //     return ret.Sqrt();
    // }

    // RGBSpectrum pow(const RGBSpectrum &s, Prec pow){
    //     RGBSpectrum ret = s;
    //     return ret.Pow(pow);
    // }

    // inline RGBSpectrum lerp(Prec t, const RGBSpectrum &s1, const RGBSpectrum &s2){
    //     RGBSpectrum ret = s1;
    //     return ret.Lerp(t, s2);
    // }
    
    // RGBSpectrum clamp(Prec low, Prec high, const RGBSpectrum &s){
    //     RGBSpectrum ret = s;
    //     ret.Clamp(low, high);
    //     return ret;
    // }


    using Spectrum = RGBSpectrum;
}