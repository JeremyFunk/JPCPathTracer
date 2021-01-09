#pragma once
#include "Spectrum.h"
#include <vcruntime.h>
namespace core {

    class CombinedPass
    {
    public:
        CombinedPass(Spectrum spectrum) : _combined_pass(spectrum){}
        CombinedPass() : _combined_pass(Spectrum()){}

        //Math
        CombinedPass &operator+=(const CombinedPass &s2){
            _combined_pass += s2._combined_pass;
            return *this;
        }
        CombinedPass &operator-=(const CombinedPass &s2){
            _combined_pass -= s2._combined_pass;
            return *this;
        }
        CombinedPass &operator*=(const CombinedPass &s2){
            _combined_pass *= s2._combined_pass;
            return *this;
        }
        CombinedPass &operator/=(const CombinedPass &s2){
            _combined_pass /= s2._combined_pass;
            return *this;
        }

        CombinedPass operator+(const CombinedPass &s2) const{
            CombinedPass ret = *this;
            ret._combined_pass += s2._combined_pass;
            return ret;
        }
        CombinedPass operator-(const CombinedPass &s2) const{
            CombinedPass ret = *this;
            ret._combined_pass -= s2._combined_pass;
            return ret;
        }
        CombinedPass operator*(const CombinedPass &s2) const{
            CombinedPass ret = *this;
            ret._combined_pass *= s2._combined_pass;
            return ret;
        }
        CombinedPass operator/(const CombinedPass &s2) const{
            CombinedPass ret = *this;
            ret._combined_pass /= s2._combined_pass;
            return ret;
        }


        
        
        CombinedPass &operator+=(const Prec &s2){
            _combined_pass += s2;
            return *this;
        }
        CombinedPass &operator-=(const Prec &s2){
            _combined_pass -= s2;
            return *this;
        }
        CombinedPass &operator*=(const Prec &s2){
            _combined_pass *= s2;
            return *this;
        }
        CombinedPass &operator/=(const Prec &s2){
            _combined_pass /= s2;
            return *this;
        }

        CombinedPass operator+(const Prec &s2) const{
            CombinedPass ret = *this;
            ret._combined_pass += s2;
            return ret;
        }
        CombinedPass operator-(const Prec &s2) const{
            CombinedPass ret = *this;
            ret._combined_pass -= s2;
            return ret;
        }
        CombinedPass operator*(const Prec &s2) const{
            CombinedPass ret = *this;
            ret._combined_pass *= s2;
            return ret;
        }
        CombinedPass operator/(const Prec &s2) const{
            CombinedPass ret = *this;
            ret._combined_pass /= s2;
            return ret;
        }

        
        CombinedPass Sqrt(){
            _combined_pass.Sqrt();
            return *this;
        }
        CombinedPass Pow(Prec pow){
            _combined_pass.Pow(pow);
            return *this;
        }
        inline CombinedPass Lerp(Prec t, CombinedPass &s){
            _combined_pass.Lerp(t, s._combined_pass);
            return *this;
        }

        CombinedPass Clamp(Prec low, Prec high) {
            _combined_pass.Clamp(low, high);
            return *this;
        }

        static CombinedPass FromValue(const Prec& value);



        Spectrum GetCombined() {return _combined_pass;};
    private:
        Spectrum _combined_pass;
    };

    
        
    CombinedPass sqrt(const CombinedPass &c);
    CombinedPass pow(const CombinedPass &c, Prec pow);
    CombinedPass lerp(Prec t, CombinedPass &s1, CombinedPass &s2);

    CombinedPass clamp(Prec low, Prec high, const CombinedPass &c);






    using SpectrumPasses = CombinedPass;
}