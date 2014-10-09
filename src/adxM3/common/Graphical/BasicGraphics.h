/*
 * BasicGraphics.h
 *
 *  Created on: 2013-10-4
 *      Author: Administrator
 */

#ifndef _BASICGRAPHICS_H_
#define _BASICGRAPHICS_H_

#include "../defs.h"



template< typename vCoordType >
class Vector2D;


/*
 * 二维点
 */
template< typename CoordType >
class Point2D
{
public:
	

	Point2D(CoordType _x=0,CoordType _y=0)
		: x(_x), y(_y)
	{ }
	CoordType x,y;


	bool operator==(const Point2D& rhs) const
	{
		if (x == rhs.x && y == rhs.y) return true;
		return false;
	}
	bool operator!=(const Point2D& rhs) const
	{
		return !operator==(rhs);
	}

	
	template< typename vCoordType >
	Vector2D<vCoordType> operator-(const Point2D& rhs) const;
	
	template< typename vCoordType >
	Point2D operator+(const Vector2D<vCoordType>& rhs) const;
	
	template< typename vCoordType >
	Point2D operator-(const Vector2D<vCoordType>& rhs) const;
	
	
	Point2D operator+(const Point2D& rhs) const
	{
		return Point2D(x + rhs.x, y + rhs.y);
	}
	Point2D operator/(const CoordType factor) const
	{
		return Point2D(x / factor, y / factor);
	}

};





/*
 * 二维向量类
 */
template< typename CoordType >
class Vector2D
{
public:



	Vector2D(CoordType _x = 0,CoordType _y = 0)
		: x(_x), y(_y)
	{ }
	CoordType x,y;

	bool operator==(const Vector2D& rhs) const
	{
		if (x==rhs.x && y==rhs.y) return true;
		return false;
	}
	Vector2D operator+(const Vector2D& rhs) const
	{
		return Vector2D(x+rhs.x,y+rhs.y);
	}
	Vector2D operator-(const Vector2D& rhs) const
	{
		return Vector2D(x-rhs.x, y-rhs.y);
	}
	INT32 DotProduct(const Vector2D& rhs) const
	{
		return x * rhs.x + y * rhs.y;
	}
	INT32 operator*(const Vector2D& rhs) const
	{
		return x * rhs.y - rhs.x * y;
	}
};




//////////////////////////////////////////////////////////////////////////
// 二维点的方法实现


template< typename CoordType >
template< typename vCoordType >
inline Vector2D<vCoordType>
Point2D<CoordType>::operator-(const Point2D<CoordType>& rhs) const
{
	return Vector2D<CoordType>(x - rhs.x, y - rhs.y);
}


template< typename CoordType >
template< typename vCoordType >
inline Point2D<CoordType> 
Point2D<CoordType>::operator+(const Vector2D<vCoordType>& rhs) const
{
	return Point2D(x + rhs.x, y + rhs.y);
}


template< typename CoordType >
template< typename vCoordType >
inline Point2D<CoordType> 
Point2D<CoordType>::operator-(const Vector2D<vCoordType>& rhs) const
{
	return Point2D<CoordType>(x - rhs.x, y - rhs.y);
}

#endif /* _BASICGRAPHICS_H_ */
