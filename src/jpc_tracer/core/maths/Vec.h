#pragma once

#include <array>
#include <algorithm>
#include <functional>
#include <cmath>

namespace jpctracer::math {
  
    template<class T, size_t _Size>
    struct Vec
    {

        std::array<T, _Size> Data;


        template<class T2>
        T& operator[](const T2& idx ){return Data[idx];}

        template<class T2>
        T operator[](const T2& idx ) const{return Data[idx];}

        bool operator==(const Vec<T,_Size>& v) const{ return Data==v.Data;}
        bool operator!=(const Vec<T,_Size>& v) const{ return Data!=v.Data;}

        constexpr auto begin(){return Data.begin();}
        constexpr auto end(){return Data.end();} 
        constexpr auto cbegin() const {return Data.cbegin();}
        constexpr auto cend() const{return Data.cend();}

        template<class T2>
        constexpr T dot(Vec<T2,_Size> a) const{
            
            T result(0);
            for(int i = 0;i<_Size;i++)
                result+=operator[](i)*a[i];
            return result;
        }

        constexpr T norm() const{
            
            T sum(0);
            for(int i = 0;i<_Size;i++)
            {
                const T& x_i =  operator[](i);
                sum+=x_i*x_i;
            }
            return std::sqrt(sum);
        }
        constexpr Vec<T,_Size> normalized() const{
            
            Vec<T,_Size> result = *this;
            return result / norm();
        }


        template<class T2>
        constexpr Vec<T,3> cross(Vec<T2,3> b) const{
            const Vec<T,3>& a = *this;
            Vec<T,3> result = {
                a[1]*b[2]-a[2]*b[1],
                a[2]*b[0]-a[0]*b[2],
                a[0]*b[1]-a[1]*b[0]
            };
            return result;
        }



    };

    namespace detail
    {
        template<class T1, class T2,size_t _Size,class BinOp>
        constexpr Vec<T1,_Size> VecImmutableOp(const Vec<T1,_Size>& a,const Vec<T2,_Size>& b,BinOp op)
        {
            Vec<T1,_Size> result;
            std::transform(a.cbegin(),a.cend(),b.cbegin(),result.begin(),op);
            return result;
        }
        template<class T, class S,size_t _Size,class BinOp>
        constexpr Vec<T,_Size> VecImmutableOpScalar(const Vec<T,_Size>& a,const S& s,BinOp op)
        {
            Vec<T,_Size> result;
            using namespace std::placeholders;
            std::transform(a.cbegin(),a.cend(),result.begin(),std::bind(op,_1,s));
            return result;
        }

        template<class T1, class T2,size_t _Size,class BinOp>
        constexpr void VecMutableOp(Vec<T1,_Size>& a,const Vec<T2,_Size>& b,BinOp op)
        {
            std::transform(a.begin(),a.end(),b.cbegin(),a.begin(),op);
        }
        template<class T, class S,size_t _Size,class BinOp>
        constexpr void VecMutableOpScalar(Vec<T,_Size>& a,const S& s,BinOp op)
        {
            using namespace std::placeholders;
            std::transform(a.begin(),a.end(),a.begin(),std::bind(op,_1,s));

        }
    };

    template<class T1, class T2,size_t _Size>
    constexpr Vec<T1,_Size> operator+(const Vec<T1,_Size>& a,const Vec<T2,_Size>& b){ return detail::VecImmutableOp(a,b, std::plus<T1>()); }

    template<class T1, class T2,size_t _Size>
    constexpr Vec<T1,_Size> operator-(const Vec<T1,_Size>& a,const Vec<T2,_Size>& b){ return detail::VecImmutableOp(a,b, std::minus<T1>()); }
    template<class T, class S,size_t _Size>
    constexpr Vec<T,_Size> operator*(const Vec<T,_Size>& a,const S s){ return detail::VecImmutableOpScalar(a,s, std::multiplies<T>()); }
    template<class T, class S,size_t _Size>
    constexpr Vec<T,_Size> operator/(const Vec<T,_Size>& a,const S s){ return detail::VecImmutableOpScalar(a,s, std::divides<T>()); }
    template<class T, class S,size_t _Size>
    constexpr Vec<T,_Size> operator%(const Vec<T,_Size>& a,const S s){ return detail::VecImmutableOpScalar(a,s, std::modulus<T>()); }

    template<class T, class S,size_t _Size>
    constexpr Vec<T,_Size> operator*(const S s,const Vec<T,_Size>& a){ return detail::VecImmutableOpScalar(a,s, std::multiplies<T>()); }
    template<class T, class S,size_t _Size>
    constexpr Vec<T,_Size> operator/(const S s,const Vec<T,_Size>& a){ return detail::VecImmutableOpScalar(a,s, std::divides<T>()); }
    template<class T, class S,size_t _Size>
    constexpr Vec<T,_Size> operator%(const S s,const Vec<T,_Size>& a){ return detail::VecImmutableOpScalar(a,s, std::modulus<T>()); }


    template<class T1, class T2,size_t _Size>
    constexpr void operator+=(Vec<T1,_Size>& a,const Vec<T2,_Size>& b){ detail::VecMutableOp(a,b, std::plus<T1>()); }
    template<class T1, class T2,size_t _Size>
    constexpr void operator-=(Vec<T1,_Size>& a,const Vec<T2,_Size>& b){ detail::VecMutableOp(a,b, std::minus<T1>()); }
    template<class T, class S,size_t _Size>
    constexpr void operator*=(Vec<T,_Size>& a,const S& s){ detail::VecMutableOpScalar(a,s, std::multiplies<T>()); }
    template<class T, class S,size_t _Size>
    constexpr void operator/=(Vec<T,_Size>& a,const S& s){ detail::VecMutableOpScalar(a,s, std::divides<T>()); }
    template<class T, class S,size_t _Size>
    constexpr void operator%=(Vec<T,_Size>& a,const S& s){ detail::VecMutableOpScalar(a,s, std::modulus<T>()); }




}