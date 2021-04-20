#pragma once

#include "Vec.h"

#include <algorithm>
#include <array>

namespace jpctracer::math {

    template <class T, size_t _Y_dim, size_t _X_dim>
    struct Mat {

        std::array<T, _X_dim * _Y_dim> Data;

        template<class T2>
        T& operator[](const T2& idx) {return Data[idx];}

        template<class T2> 
        T operator[](const T2& idx) const {return Data[idx];}

        

        bool operator==(const Mat<T,_X_dim, _Y_dim>& v) const { return Data==v.Data;}
        bool operator!=(const Mat<T,_X_dim, _Y_dim>& v) const { return Data!=v.Data;}
 
        constexpr auto begin() {return Data.begin();}
        constexpr auto end() {return Data.end();} 
        constexpr auto cbegin() const {return Data.cbegin();}
        constexpr auto cend() const {return Data.cend();}

        template<class T2>
        constexpr Vec<T, _Y_dim> dot(Vec<T2, _X_dim> a) const {
            
            Vec<T, _Y_dim> result;
            for(int y = 0; y < _Y_dim; y++)
            {
                T temp = 0;
                for (int x = 0; x < _X_dim; x++)
                {
                    temp += Data[x + y*_X_dim] * a[x];
                }

                result[y] = temp;
            }
            return result;
        }

        template<class T2, size_t _Y_dim2>
        constexpr Mat<T, _Y_dim, _Y_dim2> dot(Mat<T2,_X_dim, _Y_dim2> a) const{
            
            Mat<T, _Y_dim, _Y_dim2> result;
            for(unsigned int i = 0; i < _Y_dim; i++)
            {
                for(unsigned  int j = 0; j < _Y_dim2; j++)
                {
                    result[{i,j}] = 0;

                    for(unsigned int k = 0; k < _X_dim; k++)
                    {
                        result[{i,j}] += (*this)[{i,k}] * a[{k,j}];
                    }
                }
            }
            return result;
        }

    private:
        // y,x
        T operator[](const Vec<unsigned int,2>& coord) const {
            return Data[coord[0]*_X_dim+coord[1]];
        }
        // y,x
        T& operator[](const Vec<unsigned int,2>& coord) {
            return Data[coord[0]*_X_dim+coord[1]];
        }

    };

    namespace detail
    {
        template<class T1, class T2, size_t _X_dim, size_t _Y_dim, class BinOp>
        constexpr Mat<T1,_X_dim, _Y_dim> MatImmutableOp(const Mat<T1,_X_dim, _Y_dim>& a,const Mat<T2,_X_dim, _Y_dim>& b,BinOp op)
        {
            Mat<T1,_X_dim, _Y_dim> result;
            std::transform(a.cbegin(),a.cend(),b.cbegin(),result.begin(),op);
            return result;
        }
        template<class T, class S, size_t _X_dim, size_t _Y_dim, class BinOp>
        constexpr Mat<T,_X_dim, _Y_dim> MatImmutableOpScalar(const Mat<T,_X_dim, _Y_dim>& a,const S& s,BinOp op)
        {
            Mat<T,_X_dim, _Y_dim> result;
            using namespace std::placeholders;
            std::transform(a.cbegin(),a.cend(),result.begin(),std::bind(op,_1,s));
            return result;
        }

        template<class T1, class T2, size_t _X_dim, size_t _Y_dim, class BinOp>
        constexpr void MatMutableOp(Mat<T1,_X_dim, _Y_dim>& a,const Mat<T2,_X_dim, _Y_dim>& b,BinOp op)
        {
            std::transform(a.begin(),a.end(),b.cbegin(),a.begin(),op);
        }

        template<class T, class S, size_t _X_dim, size_t _Y_dim, class BinOp>
        constexpr void MatMutableOpScalar(Mat<T,_X_dim, _Y_dim>& a, const S& s,BinOp op)
        {
            using namespace std::placeholders;
            std::transform(a.begin(),a.end(),a.begin(),std::bind(op,_1,s));
        }
    };

    template<class T1, class T2, size_t _X_dim, size_t _Y_dim>
    constexpr Mat<T1,_X_dim, _Y_dim> operator+(const Mat<T1,_X_dim, _Y_dim>& a,const Mat<T2,_X_dim, _Y_dim>& b){ return detail::MatImmutableOp(a,b, std::plus<T1>()); }

    template<class T1, class T2, size_t _X_dim, size_t _Y_dim>
    constexpr Mat<T1,_X_dim, _Y_dim> operator-(const Mat<T1,_X_dim, _Y_dim>& a,const Mat<T2,_X_dim, _Y_dim>& b){ return detail::MatImmutableOp(a,b, std::minus<T1>()); }
    template<class T, class S,size_t _X_dim, size_t _Y_dim>
    constexpr Mat<T,_X_dim, _Y_dim> operator*(const Mat<T,_X_dim, _Y_dim>& a,const S s){ return detail::MatImmutableOpScalar(a,s, std::multiplies<T>()); }
    template<class T, class S,size_t _X_dim, size_t _Y_dim>
    constexpr Mat<T,_X_dim, _Y_dim> operator/(const Mat<T,_X_dim, _Y_dim>& a,const S s){ return detail::MatImmutableOpScalar(a,s, std::divides<T>()); }
    template<class T, class S,size_t _X_dim, size_t _Y_dim>
    constexpr Mat<T,_X_dim, _Y_dim> operator%(const Mat<T,_X_dim, _Y_dim>& a,const S s){ return detail::MatImmutableOpScalar(a,s, std::modulus<T>()); }

    template<class T, class S,size_t _X_dim, size_t _Y_dim>
    constexpr Mat<T,_X_dim, _Y_dim> operator*(const S s,const Mat<T,_X_dim, _Y_dim>& a){ return detail::MatImmutableOpScalar(a,s, std::multiplies<T>()); }
    template<class T, class S,size_t _X_dim, size_t _Y_dim>
    constexpr Mat<T,_X_dim, _Y_dim> operator/(const S s,const Mat<T,_X_dim, _Y_dim>& a){ return detail::MatImmutableOpScalar(a,s, std::divides<T>()); }
    template<class T, class S,size_t _X_dim, size_t _Y_dim>
    constexpr Mat<T,_X_dim, _Y_dim> operator%(const S s,const Mat<T,_X_dim, _Y_dim>& a){ return detail::MatImmutableOpScalar(a,s, std::modulus<T>()); }


    template<class T1, class T2,size_t _X_dim, size_t _Y_dim>
    constexpr void operator+=(Mat<T1,_X_dim, _Y_dim>& a,const Mat<T2,_X_dim, _Y_dim>& b){ detail::MatMutableOp(a,b, std::plus<T1>()); }
    template<class T1, class T2,size_t _X_dim, size_t _Y_dim>
    constexpr void operator-=(Mat<T1,_X_dim, _Y_dim>& a,const Mat<T2,_X_dim, _Y_dim>& b){ detail::MatMutableOp(a,b, std::minus<T1>()); }
    template<class T, class S,size_t _X_dim, size_t _Y_dim>
    constexpr void operator*=(Mat<T,_X_dim, _Y_dim>& a,const S& s){ detail::MatMutableOpScalar(a,s, std::multiplies<T>()); }
    template<class T, class S,size_t _X_dim, size_t _Y_dim>
    constexpr void operator/=(Mat<T,_X_dim, _Y_dim>& a,const S& s){ detail::MatMutableOpScalar(a,s, std::divides<T>()); }
    template<class T, class S,size_t _X_dim, size_t _Y_dim>
    constexpr void operator%=(Mat<T,_X_dim, _Y_dim>& a,const S& s){ detail::MatMutableOpScalar(a,s, std::modulus<T>()); }




}