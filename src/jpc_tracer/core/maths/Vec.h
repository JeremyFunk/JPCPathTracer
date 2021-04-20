#pragma once

#include <array>
#include <algorithm>
#include <functional>
#include <cmath>

namespace jpctracer::math {
  
    template<class T, size_t _Size>
    struct Vec;

    template<class T, size_t _Size>
    struct Norm
    {
        Norm() { Data = {1};}
        Norm(const Vec<T,_Size>& vec) : Data(vec.normalized().Data) {}
        Norm(const Norm<T,_Size>& n) = default;
        Norm(Norm<T,_Size>&& n) = default;
        

        Norm(T x, T y, T z) requires (_Size == 3)
            : Data(Vec<T,3>{x,y,z}.normalized().Data) {}

        Norm<T,_Size>& operator=(const Norm<T,_Size>& n) = default;
        Norm<T,_Size>& operator=(Norm<T,_Size>&& n) = default;
        
        operator Vec<T,_Size>() const {return Vec<T,_Size>{Data};}

        

        template<class T2>
        T operator[](const T2& idx ) const{return Data[idx];}

        template<class T2>
        T operator[](const T2& idx ){return Data[idx];}

        bool operator==(const Norm<T,_Size>& v) const{ return Data==v.Data;}
        bool operator!=(const Norm<T,_Size>& v) const{ return Data!=v.Data;}

        constexpr auto begin(){return Data.begin();}
        constexpr auto end(){return Data.end();} 
        constexpr auto cbegin() const {return Data.cbegin();}
        constexpr auto cend() const{return Data.cend();}

        template<class T2>
        constexpr T dot(const Vec<T2,_Size>& a) const{
            
            T result(0);
            for(int i = 0;i<_Size;i++)
                result+=operator[](i)*a[i];
            return result;
        }

        template<class T2>
        constexpr Vec<T,3> cross(const Vec<T2,3>& b) const{
            Vec<T,3> result = {
                (*this)[1]*b[2]-(*this)[2]*b[1],
                (*this)[2]*b[0]-(*this)[0]*b[2],
                (*this)[0]*b[1]-(*this)[1]*b[0]
            };
            return result;
        }

        template<class T2>
        constexpr T dot(const Norm<T2,_Size>& a) const{
            
            T result(0);
            for(int i = 0;i<_Size;i++)
                result+=operator[](i)*a[i];
            return result;
        }

        template<class T2>
        constexpr Vec<T,3> cross(const Norm<T2,3>& b) const{
            Vec<T,3> result = {
                (*this)[1]*b[2]-(*this)[2]*b[1],
                (*this)[2]*b[0]-(*this)[0]*b[2],
                (*this)[0]*b[1]-(*this)[1]*b[0]
            };
            return result;
        }

        static Norm<T,_Size> UnsafeConversion(const Vec<T,_Size>& vec)
        {
            return Norm<T,_Size>{vec.Data,true};
        }

        void flip()
        {
            for(int i = 0; i < _Size; i++)
                Data[i] *= -1;
        }

        void flip(int axis)
        {
            Data[axis] *= -1;
        }



        static constexpr Norm<T,_Size> normalize(const Vec<T,_Size>& vec){
            
            Norm<T,_Size> result;
            T n = vec.norm();
            for(int i = 0; i< _Size;i++)
                result.Data[i] = vec[i] / n; 
            return result;
        }


    private:
        
        

        Norm(const std::array<T, _Size>& data,bool) : Data(data){}
        std::array<T, _Size> Data;
        friend Vec<T,_Size>;
    };

    

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
        constexpr T dot(const Vec<T2,_Size>& a) const{
            
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
        constexpr Norm<T,_Size> normalized() const{
            
            return Norm<T,_Size>::normalize(*this);
        }


        template<class T2>
        constexpr Vec<T,3> cross(const Vec<T2,3>& b) const{
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