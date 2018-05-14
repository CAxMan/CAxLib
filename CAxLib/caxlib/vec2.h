/**
 @author    Marco Livesu (marco.livesu@gmail.com)
 @copyright Marco Livesu 2017.
*/

#ifndef VEC2_H
#define VEC2_H

#include "caxlib.h"

#include <cmath>
#include <iostream>
#include <assert.h>

namespace caxlib
{

template<typename real> class vec2
{
    private:

        union
        {
            struct
            {
                real m_x;
                real m_y;
            };
            real m_v[2];
        };

    public:

        vec2()
        {
            set( 0.0, 0.0 );
        }

        vec2(const real x, const real y=0.0)
        {
            set(x, y);
        }

        inline const real *ptr() const
        {
            return m_v;
        }

        inline void set( const real x, const real y)
        {
            m_x = x;
            m_y = y;
        }

        inline real dot(const vec2<real> & in) const
        {
            return m_x * in.m_x +
                   m_y * in.m_y;
        }

        inline vec2<real> cross(const vec2<real> & in) const
        {
            return vec2<real>( m_x * in.m_y,
                              -m_y * in.m_x);
        }

        inline real length_squared() const
        {
            return m_x * m_x +
                   m_y * m_y;
        }

        inline real length() const
        {
            return sqrt(m_x * m_x +
                        m_y * m_y);
        }

        inline real normalize()
        {
            real len = std::max(length(), 1e-5);
            m_x /= len;
            m_y /= len;
            return len;
        }

        inline real &x() { return m_x; }
        inline real &y() { return m_y; }

        inline const real &x() const { return m_x; }
        inline const real &y() const { return m_y; }

        inline real &operator[](const int id)
        {
            return m_v[id];
        }

        inline const real &operator[](const int id) const
        {
            return m_v[id];
        }

        inline vec2<real> operator-() const
        {
            return vec2<real>(-m_x, -m_y);
        }

        inline vec2<real> operator+(const vec2<real> b) const
        {
            return vec2<real>(m_x + b.m_x,
                              m_y + b.m_y);
        }

        inline vec2<real> operator-(const vec2<real> b) const
        {
            return vec2<real>(m_x - b.m_x,
                              m_y - b.m_y);
        }

        inline vec2<real> operator*(const real b) const
        {
            return vec2<real>(m_x * b,
                              m_y * b);
        }


        inline vec2<real> operator*(const vec2<real> b) const
        {
            return vec2<real>(m_x * b.x(),
                              m_y * b.y());
        }


        inline vec2<real> operator/(const real b) const
        {
            return vec2<real>(m_x / b,
                              m_y / b);
        }

        inline vec2<real> operator+=(const vec2<real> b)
        {
            vec2<real> tmp(m_x + b.m_x,
                           m_y + b.m_y);
            *this=tmp;
            return tmp;
        }

        inline vec2<real> operator-=(const vec2<real> b)
        {
            vec2<real> tmp(m_x - b.m_x,
                           m_y - b.m_y);
            *this = tmp;
            return tmp;
        }

        inline vec2<real> operator*=(const real b)
        {
            vec2<real> tmp(m_x * b,
                           m_y * b);
            *this = tmp;
            return tmp;
        }

        inline vec2<real> operator/=(const real b)
        {
            vec2<real> tmp(m_x / b,
                           m_y / b);
            *this = tmp;
            return tmp;
        }

        inline bool operator<(const vec2<real> in) const
        {
            for( int i=0; i<2; ++i)
            {
                if( this->operator[](i) < in[i] ) return true;
                if( this->operator[](i) > in[i] ) return false;
            }
            return false;
        }

        inline double min_entry() const
        {
            if (m_x < m_y) return m_x;
            return m_y;
        }

        inline double max_entry() const
        {
            if (m_x > m_y) return m_x;
            return m_y;
        }

        inline vec2<real> min(const vec2<real> &in) const
        {
            return vec2<real>(std::min(x(), in.x()),
                              std::min(y(), in.y()));
        }

        inline vec2<real> max(const vec2<real> &in) const
        {
            return vec2<real>(std::max(x(), in.x()),
                              std::max(y(), in.y()));
        }
};

template<typename real>
CAX_INLINE
vec2<real> operator*(const double &b, const vec2<real> &a )
{
    return vec2<real>(b * a.x(), b * a.y());
}

template<typename real>
CAX_INLINE
std::ostream& operator<<(std::ostream &in, const vec2<real> &val)
{
    in << "[" << val.x() << ", " << val.y() << "]";
    return in;
}

typedef vec2<double> vec2d;
typedef vec2<float>  vec2f;

}

#endif // VEC2_H
