/*
 * graphical.h
 *
 *  Created on: 2013-7-20
 *      Author: Administrator
 */

#ifndef _GRAPHICAL_H_
#define _GRAPHICAL_H_

#include "BasicGraphics.h"
#include "../DataStructures/List.h"
#include <cstdlib>


namespace adxM3
{



	/*
	 * 两点直线
	 */
	template< class GraphicsDevice >
	class Line
	{
	public:

		typedef typename GraphicsDevice::CoordType CoordType;
		typedef typename GraphicsDevice::SizeType SizeType;
		typedef Point2D<CoordType> PointType;



		Line()
			: P1(), P2()
		{ }
		Line(PointType p1,PointType p2)
			: P1(p1), P2(p2)
		{ }
		PointType P1, P2;


		/*
		 * @brief	通过x值或y值换算出另一个值
		 * @notice	请注意该线段不要垂直或者水平!
		 */
		CoordType YofX(CoordType x) const
		{
			// 请注意无法处理垂直的情况哦！
			return (x - P1.x) * (P2.y - P1.y) / (P2.x - P1.x) + P1.y;
		}
		CoordType XofY(CoordType y) const
		{
			// 请注意无法处理水平的情况哦！
			return (y - P1.y) * (P2.x - P1.x) / (P2.y - P1.y) + P1.x;
		}


		/*
		 * @brief	测试该直线知否是垂直线或者水平线
		 */
		bool IsVertical() const
		{
			return P1.x == P2.x;
		}
		bool IsHorizontal() const
		{
			return P1.y == P2.y;
		}


		/*
		 * @brief	测试该直线是否有效，即判断两个点是否坐标相同
		 */
		bool IsValid() const
		{
			return P1 != P2;
		}

		/*
		 * @brief	测试该点是否在直线上
		 * @param	p 被测点
		 */
		bool HasPointOnLine(const PointType& p) const
		{
			typename PointType::CoordValue dx = P2.x - P1.x;
		
			// 当该线不是垂直
			if (dx != 0) 
			{
				// 带入直线方程求值作为判据
				if (p.y == (p.x - P1.x) * (P2.y - P1.y) / dx + P1.y) 
				{
					return true;
				}
			} 
			// 垂直时只需比较x坐标是否相等
			else if (P1.x == p.x)
			{
				return true;
			}
		
			return false;
		}




	};


	// 两点线段，P1是闭点，P2是开点
	template< class GraphicsDevice >
	class LineSegment
		: public Line<GraphicsDevice>
	{
	public:

		typedef typename GraphicsDevice::CoordType CoordType;
		typedef typename GraphicsDevice::SizeType SizeType;
		typedef typename GraphicsDevice::ColorType ColorType;
		typedef Point2D<CoordType> PointType;

		using Line<GraphicsDevice>::P1;
		using Line<GraphicsDevice>::P2;



		LineSegment()
			: Line<GraphicsDevice>()
		{ }
		LineSegment(PointType p1, PointType p2)
			: Line<GraphicsDevice>(p1, p2)
		{ }
	
		

		/*
		 * @brief	测试该点是否在线段上
		 * @param	p 被测点
		 */
		bool HasPointOnSegment(const PointType& p) const
		{
			// 判断在线
			if (HasPointOnLine(p)) 
			{
				// 判断范围,x和y范围都要判断，分是否垂直情况
				if (this->IsVertical())
				{
					if (p.y > min(P1.y, P2.y) && p.y < max(P1.y, P2.y) || p.y == P1.y) return true;
				}
				else
				{
					if (p.x > min(P1.x, P2.x) && p.x < max(P1.x, P2.x) || p.x == P1.x) return true;
				}
			}
			return false;
		}
	


		// 两线段相交的情况枚举（两线段均为左闭右开的线段）
		enum INTERSECTIONS
		{
			Disjoint,						// 不相交
			AtEndpoint,						// 交点在端点
			AtEdge,							// 交点在边上

			Coincide_this,					// 两线段重合（且有相交，交点为this的端点）
			Coincide_L2,					// 两线段重合（且有相交，交点为L2的端点）
			Coincide_2IntersectionPoints	// 两线段重合（且有相交，两线段的端点都是交点）
		};



		/*
		 * @brief	测试与另一条线段是否相交
		 * @param	L2 另一条线段
					intersection 相交的交点
		 * @notice	如果是重合的情况，intersection将优先表示L2的端点
		 */
		int IntersectWith(const LineSegment<GraphicsDevice>& L2, PointType& intersection) const
		{
			// 两线段平行
			// mk是两线段换成积的表达式，只能用于判断是否平行
			INT32 mk = (P2.y - P1.y) * (L2.P2.x - L2.P1.x) - (P2.x - P1.x) * (L2.P2.y - L2.P1.y);
			if (mk == 0) 
			{
				// 当斜率相同时，还要判断是否重合，即只需判断端点是否在另一条线段上
				if (L2.HasPointOnSegment(P1)) 
				{
					// 重合时还要通过线段的方向是否相反判断是否有两个交点
					// 分垂直与非垂直的情况
					if (this->IsVertical())
					{
						if ((L2.P1.y - L2.P2.y) * (P1.y - P2.y) <= 0)
						{
							// dy的符号不同或为0表示方向相反
							intersection = L2.P1;
							return Coincide_2IntersectionPoints;
						}
					}
					else
					{
						if ((L2.P1.x - L2.P2.x) * (P1.x - P2.x) <= 0)
						{
							// dx的符号不同或为0表示方向相反
							intersection = L2.P1;
							return Coincide_2IntersectionPoints;
						}
					}
				
					intersection = P1;
					return Coincide_this;
				} 
				// 当进入这个分支时，则P1不可能在L2上，所以在这个分支内不可能出现双交点情况！
				// 那么只能是下面的这种情况了
				else if (HasPointOnSegment(L2.P1))
				{	
					intersection = L2.P1;
					return Coincide_L2;
				}
				return Disjoint;
			}

			// 当其中一条直线垂直时，可以直接判断相交情况
			if (this->IsVertical())
			{
				// 判断this->P1.x在不在L2的x范围内
				if (P1.x > L2.P2.x && P1.x <= L2.P1.x || P1.x < L2.P2.x && P1.x >= L2.P1.x)
				{
					// x范围满足，判断y值范围应该在this的y范围内
					typename PointType::CoordValue _y = L2.YofX(P1.x);
					if (_y > P2.y && _y < P1.y || _y < P2.y && _y > P1.y)
					{
						// 刚好相交与L2端点的情况
						if (P1.x == L2.P1.x)
						{
							intersection = L2.P1;
							return AtEndpoint;
						}
						else
						{
							intersection = PointType(P1.x,_y);
							return AtEdge;
						}
					} 
					// 相交于端点的情况另外拉出分支来讨论，保证程序正确性
					else if (_y == P1.y)
					{
						intersection = PointType(P1.x,_y);
						return AtEndpoint;
					}
				}

				// 其余x范围肯定不相交
				return Disjoint;
			}
			// 两条直线如果同时垂直那么会在斜率相同的分支判断掉，所以这里不用处理两直线同时垂直的情况
			else if (L2.IsVertical())
			{
				// 判断L2.P1.x在不在this的x范围内
				if (L2.P1.x > P2.x && L2.P1.x <= P1.x || L2.P1.x < P2.x && L2.P1.x >= P1.x)
				{
					// x范围满足，判断y值范围应该在L2的y范围内
					typename PointType::CoordValue _y = YofX(L2.P1.x);
					if (_y > L2.P2.y && _y < L2.P1.y || _y < L2.P2.y && _y > L2.P1.y)
					{
						if (L2.P1.x == P1.x)
						{
							intersection = P1;
							return AtEndpoint;
						}
						else
						{
							intersection = PointType(L2.P1.x,_y);
							return AtEdge;
						}
					}
					else if (_y == L2.P1.y) 
					{
						intersection = PointType(L2.P1.x,_y);
						return AtEndpoint;
					}
				}

				return Disjoint;
			}

			// 如果不垂直也不平行，那就计算交点值
			typename PointType::CoordValue _x 
				= ((P1.x * (P2.y - P1.y) + (L2.P1.y - P1.y) * (P2.x - P1.x)) * (L2.P2.x - L2.P1.x)
				- L2.P1.x * (L2.P2.y - L2.P1.y) * (P2.x - P1.x))
				/ mk;

			// 判断x是否落在两线段的x范围内(此时先不考虑相交于端点的情况)
			if (   _x > min(P1.x,P2.x) 
				&& _x < max(P1.x,P2.x) 
				&& _x > min(L2.P1.x,L2.P2.x) 
				&& _x < max(L2.P1.x,L2.P2.x)
			) {
				intersection.x = _x;
				intersection.y 
					= (L2.P1.y * (L2.P2.x - L2.P1.x) * (P2.y - P1.y) 
					- ((P1.y * (P2.x - P1.x) - (P1.x - L2.P1.x) * (P2.y - P1.y)) * (L2.P2.y - L2.P1.y)))
					/ mk;
				return AtEdge;
			} 
			// 以下2分支推迟到这里再判断一次某点是否在线段上可降低时间复杂度，大部分情况下交点不是端点
			else if (_x == P1.x && L2.HasPointOnSegment(P1))
			{
				// 判断是否相交在端点处
				intersection = P1;
				return AtEndpoint;
			} 
			else if (_x == L2.P1.x && HasPointOnSegment(L2.P1)) 
			{
				// 判断是否相交在端点处
				intersection = L2.P1;
				return AtEndpoint;
			}
		
			// 最后不满足任何情况就不相交
			return Disjoint;
		}



		/*
		 * @brief	向设备输出图形
		 */
		void Out(GraphicsDevice& dc, ColorType color) const
		{
			CoordType xerr = 0, yerr = 0;
			CoordType xinc = 0, yinc = 0; 

			CoordType dx = P2.x - P1.x;				//计算坐标增量 
			CoordType dy = P2.y - P1.y; 
			CoordType uRow = P1.x; 
			CoordType uCol = P1.y; 
	
			if (dx > 0) 
			{
				xinc = 1;						//设置单步方向 
			} 
			else if (dx == 0) 
			{
				xinc = 0;						//垂直线 
			} 
			else 
			{
				xinc = -1;
				dx = -dx;
			} 
			if (dy > 0) 
			{
				yinc=1; 
			} 
			else if (dy == 0) 
			{
				yinc=0;							//水平线 
			} 
			else 
			{
				yinc = -1; 
				dy = -dy;
			} 

			CoordType distance = max(dx,dy);		//选取基本增量坐标轴 
	
			//画线输出 
			for (CoordType t = 0; t <= distance + 1; ++t)	
			{  
				dc.SetPixel(uRow, uCol, color);
				xerr += dx; 
				yerr += dy; 
				if (xerr > distance) 
				{ 
					xerr -= distance; 
					uRow += xinc; 
				} 
				if (yerr > distance) 
				{ 
					yerr -= distance; 
					uCol += yinc; 
				} 
			}
		}


	};




	/*
	 * 矩形类，用左上角和右下角确定一矩形
	 */
	template< class GraphicsDevice >
	class Rectangle
	{
	public:


		typedef typename GraphicsDevice::CoordType CoordType;
		typedef typename GraphicsDevice::SizeType SizeType;
		typedef typename GraphicsDevice::ColorType ColorType;
		typedef Point2D<CoordType> PointType;


		Rectangle(PointType tl, PointType br)
			: TopLeft(tl), BottomRight(br)
		{ }
		Rectangle(CoordType left, CoordType top, CoordType right, CoordType bottom)
			: TopLeft(left,top), BottomRight(right,bottom)
		{ }
		Rectangle(PointType tl, SizeType width, SizeType height)
			: TopLeft(tl), BottomRight(tl.x+width,tl.y+height)
		{ }
		PointType TopLeft, BottomRight;

		bool operator==(const Rectangle& rhs)
		{
			if (TopLeft==rhs.TopLeft && BottomRight==rhs.BottomRight) return true;
			return false;
		}
	

		/*
		 * @brief	矩形参数计算函数
		 */
		UINT32 Area() const
		{
			return abs((BottomRight.x - TopLeft.x) * (BottomRight.y - TopLeft.y));
		}
		SizeType Width() const
		{
			return abs(BottomRight.x - TopLeft.x);
		}
		SizeType Height() const
		{
			return abs(BottomRight.y - TopLeft.y);
		}


		/*
		 * @brief	判断某点是否在该矩形内
		 */
		bool HasInnerPoint(const PointType& p) const
		{
			if (p.x>TopLeft.x && p.y>TopLeft.y && p.x<BottomRight.x && p.y<BottomRight.y
				|| p.x<TopLeft.x && p.y>TopLeft.y && p.x>BottomRight.x && p.y<BottomRight.y) 
			{
				return true;
			}
			return false;
		}
	


		/*
		 * @brief	取矩形副对角线的两个点
		 */
		PointType TopRight() const
		{
			return PointType(BottomRight.x, TopLeft.y);
		}
		PointType BottomLeft() const
		{
			return PointType(TopLeft.x, BottomRight.y);
		}


	
		/*
		 * @brief	判断此矩形是否与另一矩形有相交
		 * @param	rect 与之求交集的矩形
					remain 此矩形（不是rect）中未相交的L型区域保存在这里
		 * @retval	返回相交的矩形区域
		 */
		void IntersectWith(const Rectangle& rect, Rectangle& result) const;



		/*
		 * @brief	判断此矩形是否有效，宽高是否大于1
		 * @retval	true 如果宽或高大于1，否则false
		 */
		bool IsValid() const
		{
			if (TopLeft.x==BottomRight.x || TopLeft.y==BottomRight.y) {
				return false;
			}
			return true;
		}



		void Stroke(GraphicsDevice& dc, ColorType color) const
		{
			LineSegment<GraphicsDevice>(TopLeft, PointType(BottomRight.x,TopLeft.y)).Out(dc, color);
			LineSegment<GraphicsDevice>(TopLeft, PointType(TopLeft.x,BottomRight.y)).Out(dc, color);
			LineSegment<GraphicsDevice>(PointType(TopLeft.x,BottomRight.y), BottomRight).Out(dc, color);
			LineSegment<GraphicsDevice>(PointType(BottomRight.x,TopLeft.y), BottomRight).Out(dc, color);
		}
		void Fill(GraphicsDevice& dc, ColorType color) const
		{
			// 确定输出的最大最小边界，避免超出屏幕
			CoordType xStart = TopLeft.x < 0 ? 0 : TopLeft.x > GraphicsDevice::MAX_WIDTH ? GraphicsDevice::MAX_WIDTH : TopLeft.x;
			CoordType yStart = TopLeft.y < 0 ? 0 : TopLeft.y > GraphicsDevice::MAX_HEIGHT ? GraphicsDevice::MAX_HEIGHT : TopLeft.y;
			CoordType xEnd = min(BottomRight.x, CoordType(GraphicsDevice::MAX_WIDTH));
			CoordType yEnd = min(BottomRight.y, CoordType(GraphicsDevice::MAX_HEIGHT));
		
			for (CoordType i = yStart; i < yEnd; ++i) 
			{
				dc.SequentialSetPixel(xStart, i, color, xEnd-xStart);
			}
		}
	
	};



	/*
	 * 圆类，圆心与半径确定一个圆
	 */
	template< class GraphicsDevice >
	class Circle
	{
	public:

		typedef typename GraphicsDevice::CoordType CoordType;
		typedef typename GraphicsDevice::SizeType SizeType;
		typedef typename GraphicsDevice::ColorType ColorType;
		typedef Point2D<CoordType> PointType;
	

		Circle(PointType center, SizeType radius)
			: Center(center), Radius(radius)
		{ }
		PointType Center;
		SizeType Radius;


		void Stroke(GraphicsDevice& dc, ColorType color) const
		{
			CoordType a = 0, b = Radius;	  
			CoordType di = 3 - (Radius << 1);             //判断下个点位置的标志
			while (a <= b) 
			{
				dc.SetPixel(Center.x - b, Center.y - a, color);             //3           
				dc.SetPixel(Center.x + b, Center.y - a, color);             //0           
				dc.SetPixel(Center.x - a, Center.y + b, color);             //1       
				dc.SetPixel(Center.x - b, Center.y - a, color);             //7           
				dc.SetPixel(Center.x - a, Center.y - b, color);             //2             
				dc.SetPixel(Center.x + b, Center.y + a, color);             //4               
				dc.SetPixel(Center.x + a, Center.y - b, color);             //5
				dc.SetPixel(Center.x + a, Center.y + b, color);             //6 
				dc.SetPixel(Center.x - b, Center.y + a, color);             
				++a;
				//使用Bresenham算法画圆     
				if (di<0) 
				{
					di += 4*a+6;	  
				} 
				else 
				{
					di += 10+4*(a-b);   
					--b;
				} 
				dc.SetPixel(Center.x + a, Center.y + b, color);
			}
		}
		void Fill(GraphicsDevice& dc, ColorType color) const
		{

		}

	};



	template <typename ColorType>
	class IMAGE;



	/*
	 * 多边形类（维护一个顶点链表）
	 */
	template< class GraphicsDevice >
	class Polygon
	{
	public:


		typedef typename GraphicsDevice::CoordType CoordType;
		typedef typename GraphicsDevice::SizeType SizeType;
		typedef typename GraphicsDevice::ColorType ColorType;
		typedef Point2D<CoordType> PointType;
		typedef Vector2D<CoordType> VectorType;


		// 请注意维护VertexCount不等于0！

		// 多边形可是是凹或凸的，可以是自相交的，可以是多个小多边形用0宽度边连接在一起的
		// 多边形构造时会自动闭合处理，即会添加额外的最后一点使之等于第一点，并设为不活动点
		// 如果最后一点本身就等于第一点则跳过这步
		// 不允许在PointType类型的初始化数组中包含多个用0宽度边连接的子多边形，
		// 需要描述多个子多边形或者多边形挖洞的请用Merge方法


		class Vertex;

		// 多边形顶点迭代子类型说明
		typedef typename CircularList<Vertex>::Iterator VertexIterator;

		// 多边形边类型说明
		typedef LineSegment<GraphicsDevice> Edge;


		/*
		 * 多边形顶点类
		 */
		class Vertex
		{
		public:

			friend class Polygon;

			Vertex(PointType coord, bool inactive = false)
				: Coord(coord), OverlapVertexIterator(NULL), IsStartPointOfInactiveEdge(inactive), Visited(false)
			{ }
			PointType Coord;

		private:
		
			// 指向交点的另一重合点（仅在多边形布尔运算时用到）
			VertexIterator OverlapVertexIterator;

			// 多个小多边形通过0宽度边来连接时，这些连接边被标记为非活动边，
			// 按照顶点连接顺序，这些边的起点的IsStartPointOfInactiveEdge值为true
			// 构造多边形时默认为false
			// 多个多边形请用Merge方法
			bool IsStartPointOfInactiveEdge;

			// 在多边形布尔运算中是否被访问过标记
			bool Visited;

		};


		Polygon()
			: VertexCount(0), Ymin(32767), Ymax(-32767)
		{ }
		Polygon(PointType pts[], UINT8 count)
			: VertexCount(count)
		{
			if (count > 0) 
			{
				Ymin = Ymax = pts[0].y;
			} 
			else 
			{
				Ymin = 32767;
				Ymax = -32767;
				return;
			}
			for (UINT8 i=0; i<count; ++i)
			{
				listVertex.PushBack(Vertex(pts[i]));
				Ymin = min(Ymin,pts[i].y);
				Ymax = max(Ymax,pts[i].y);
			}

			// 最后一个不是闭合点（第一点不同），要插入额外的这一点
			if (pts[count-1] != pts[0])
			{
				// 其实就是插入多一个第一点相同的点哦！
				listVertex.PushBack(Vertex(pts[0],true));
			}
			else
			{
				// 如果用户已经在点列表中闭合了最后一点，要设置这点不激活
				LastVertex()->Data.IsStartPointOfInactiveEdge = true;
			}

			// 让所有节点按照逆时针（正向）排在链表中
			MakeForward();
		}
		
		
		CircularList<Vertex> listVertex;
		UINT8 VertexCount;
		CoordType Ymin, Ymax;

	

		

	
		/*
		 * @brief	测试某向量关于另外两个向量的位置关系
		 * @param	a,b 从a到b的平面角范围为内部，剩下的平面角范围为外部
					test 被测向量
		 * @retval	如果顺序是a->test->b则返回Inside，否则返回Outside
		 * @notice	test和a或b重合时算Outside
		 */
		enum VectorPosition
		{
			Inside = -1, Outside = 1, Overlap_a = 2, Overlap_b = 3
		};
		static VectorPosition _PositionalOrderOfVectors(const VectorType& a,const VectorType& b,const VectorType& test)
		{
			INT32 a_x_test = a * test;
			INT32 test_x_b = test * b;
			if (a_x_test == 0)
			{
				return Overlap_a;
			}
			if (test_x_b == 0)
			{
				return Overlap_b;
			}

			if (a * b >= 0)
			{
				if (a_x_test < 0 || test_x_b < 0) return Outside;
			}
			else  
			{
				if (a_x_test > 0 && test_x_b > 0) return Outside;
			}
			return Inside;
		}


		/*
		 * 多边形相交交点信息
		 */
		struct IntersectionInfo// : public _cout_debuger_<IntersectionInfo>
		{
			PointType point;
			int position;
			VertexIterator it_that, it_that_prior;
		
			// this多边形的顶点，因为同一组交点信息中this多边形的边不会变化
			static VertexIterator it_this, it_this_prior;
		};


		/*
		 * @brief	按离起点的距离排序交点信息链表
		 * @param	intersectionInfo 交点信息链表
		 * @notice	此方法只能被多边形布尔运算方法调用
		 */
		static void _InsertAndSortByDistance(CircularList<IntersectionInfo>& intersectionInfo,IntersectionInfo& ins)
		{
			if (intersectionInfo.IsEmpty()) 
			{
				// 空表则直接插入
				intersectionInfo.PushBack(ins);
				return;
			}

			INT32 distance_ins 
				= (ins.point.x - ins.it_this->Data.Coord.x) * (ins.point.x - ins.it_this->Data.Coord.x)
				+ (ins.point.y - ins.it_this->Data.Coord.y) * (ins.point.y - ins.it_this->Data.Coord.y);

			typename CircularList<IntersectionInfo>::Iterator it = intersectionInfo.First();
			typename CircularList<IntersectionInfo>::Iterator it_prior = intersectionInfo.Last();

			// 只有一个元素时且要插入到前方要特殊处理
			if (intersectionInfo.OnlyOneElement())
			{
				INT32 distance 
					= (it->Data.point.x - it->Data.it_this->Data.Coord.x) * (it->Data.point.x - it->Data.it_this->Data.Coord.x)
					+ (it->Data.point.y - it->Data.it_this->Data.Coord.y) * (it->Data.point.y - it->Data.it_this->Data.Coord.y);
				if (distance_ins <= distance)
				{
					intersectionInfo.PushFront(ins);
				}
				else
				{
					intersectionInfo.PushBack(ins);
				}
				return;
			}

			while (it)
			{
				INT32 distance 
					= (it->Data.point.x - it->Data.it_this->Data.Coord.x) * (it->Data.point.x - it->Data.it_this->Data.Coord.x)
					+ (it->Data.point.y - it->Data.it_this->Data.Coord.y) * (it->Data.point.y - it->Data.it_this->Data.Coord.y);
				if (distance_ins <= distance)
				{
					intersectionInfo.Insert(it_prior,ins);
					return;
				}
				it_prior = it;
				it = it->Next;
			}
			intersectionInfo.PushBack(ins);
		}


		/*
		 * @brief	按照交点信息将各交点在对应顶点迭代子多边形中构造好交点
		 * @param	intersectionInfo 交点信息链表
					intersectionPointQuene 输出参数，插入的交点队列（布尔运算中用到）
					polygon_this 求交时的this
					polygon_that 求交时的另一个多边形
		 * @notice	此方法只能被多边形布尔运算方法调用
					进入此方法后intersectionInfo中成员的it_this和it_this_prior会被改变，请确保调用前保存好当前顶点信息
		 */
		static void _ConstructIntersectionPoint(
			Polygon& polygon_this,
			Polygon& polygon_that,
			CircularList<IntersectionInfo>& intersectionInfo,
			SingleList<VertexIterator>& intersectionPointQuene)
		{
			if (intersectionInfo.IsEmpty()) return;

			typename CircularList<IntersectionInfo>::Iterator it_first_info = intersectionInfo.First();
			typename CircularList<IntersectionInfo>::Iterator it_info = it_first_info;
		
			// 根据情况插入交点
			do
			{
				VertexIterator _it_this = NULL;
				VertexIterator _it_that = NULL;
				VectorType this_a;
				VectorType this_b;
				VectorType that_a;
				VectorType that_b;
				bool NeedShiftThisVertexNext = true;
				switch (it_info->Data.position)
				{
				case Edge::AtEdge :
					// 插入平凡顶点
					_it_that = polygon_that.InsertVertex(it_info->Data.it_that,it_info->Data.point);
					_it_this = polygon_this.InsertVertex(it_info->Data.it_this,it_info->Data.point);
				
					// 平凡顶点互相绑定
					_it_that->Data.OverlapVertexIterator = _it_this;
					_it_this->Data.OverlapVertexIterator = _it_that;
				
					// 这种点放在最后，因为会引起bug，其余都PushFront
					intersectionPointQuene.PushBack(_it_this);
					break;

				case Edge::AtEndpoint :
					// 在端点相交时会有3种情况
					// 1.thisEdge起点与thatEdge边上相交
					// 2.thatEdge起点与thisEdge边上相交
					// 3.thisEdge起点与thatEdge起点相交
					// 第1、2种情况均可在交点出插入一点顶点转换成第3种情况

					// 当交点是thatEdge的起点时
					if (it_info->Data.point != it_info->Data.it_this->Data.Coord)
					{
						polygon_this.InsertVertex(it_info->Data.it_this,it_info->Data.point);
						it_info->Data.it_this_prior = it_info->Data.it_this;
						it_info->Data.it_this = it_info->Data.it_this->Next;
					}
					// 当交点是thisEdge的起点时
					else if (it_info->Data.point != it_info->Data.it_that->Data.Coord)
					{
						polygon_that.InsertVertex(it_info->Data.it_that,it_info->Data.point);
						it_info->Data.it_that_prior = it_info->Data.it_that;
						it_info->Data.it_that = it_info->Data.it_that->Next;
					}

					// 现在只需处理两起点重合时的情况

					// [这种情况可能会接受直接从上面两个分支跳转过来的情况]
					// 判断thatPriorEdge、thatEdge、thisPriorEdge、thisEdge 4个向量的位置顺序关系
					// 有以下4种情况（详细内容请见草稿纸）
					// 1.that一边在内，一边在外
					// 2.this两边在内，that两边在外
					// 3.that两边在内，this两边在外
					// 4.that与this两边互相在外
					this_a = it_info->Data.it_this_prior->Data.Coord - it_info->Data.it_this->Data.Coord;
					this_b = it_info->Data.it_this->Next->Data.Coord - it_info->Data.it_this->Data.Coord;
					that_a = it_info->Data.it_that_prior->Data.Coord - it_info->Data.it_that->Data.Coord;
					that_b = it_info->Data.it_that->Next->Data.Coord - it_info->Data.it_that->Data.Coord;


					if (	_PositionalOrderOfVectors(this_a,this_b,that_a) != _PositionalOrderOfVectors(this_a,this_b,that_b)
						||	_PositionalOrderOfVectors(that_a,that_b,this_a) == Inside 
						&&	_PositionalOrderOfVectors(that_a,that_b,this_b) == Inside)
					{
						// that一边在内，一边在外，
						// 或者this两边在内，that两边在外
						// 两顶点互相绑定即可
						it_info->Data.it_this->Data.OverlapVertexIterator = it_info->Data.it_that;
						it_info->Data.it_that->Data.OverlapVertexIterator = it_info->Data.it_this;

						intersectionPointQuene.PushFront(it_info->Data.it_this);
					}
					else if (	_PositionalOrderOfVectors(this_a,this_b,that_a) == Inside 
							&&	_PositionalOrderOfVectors(this_a,this_b,that_b) == Inside)
					{
						// that两边在内，this两边在外
						_it_this = polygon_this.InsertVertex(it_info->Data.it_this,it_info->Data.it_this->Data);
						_it_that = polygon_that.InsertVertex(it_info->Data.it_that,it_info->Data.it_that->Data);
						it_info->Data.it_this->Data.OverlapVertexIterator = it_info->Data.it_that;
						it_info->Data.it_that->Data.OverlapVertexIterator = it_info->Data.it_this;
						_it_this->Data.OverlapVertexIterator = _it_that;
						_it_that->Data.OverlapVertexIterator = _it_this;

						intersectionPointQuene.PushFront(it_info->Data.it_this);
					}
					else
					{
						// that与this两边互相在外
						_it_this = polygon_this.InsertVertex(it_info->Data.it_this,it_info->Data.it_this->Data);
						_it_that = polygon_that.InsertVertex(it_info->Data.it_that,it_info->Data.it_that->Data);
						_it_this->Data.OverlapVertexIterator = it_info->Data.it_that;
						it_info->Data.it_that->Data.OverlapVertexIterator = _it_this;
						_it_that->Data.OverlapVertexIterator = it_info->Data.it_this;
						it_info->Data.it_this->Data.OverlapVertexIterator = _it_that;
					
						intersectionPointQuene.PushFront(it_info->Data.it_this);
						intersectionPointQuene.PushFront(_it_this);
					}
					break;

				case Edge::Coincide_this :
					_it_that = polygon_that.InsertVertex(it_info->Data.it_that,it_info->Data.it_this->Data.Coord);
					_it_that->Data.OverlapVertexIterator = it_info->Data.it_this;
					it_info->Data.it_this->Data.OverlapVertexIterator = _it_that;

					intersectionPointQuene.PushFront(it_info->Data.it_this);
				
					// 只有这种情况比较特殊，不需要把thisEdge顺次移动，因为thisEdge还没有插入新的顶点
					NeedShiftThisVertexNext = false;
					break;

				case Edge::Coincide_L2 :
					_it_this = polygon_this.InsertVertex(it_info->Data.it_this,it_info->Data.it_that->Data.Coord);
					_it_this->Data.OverlapVertexIterator = it_info->Data.it_that;
					it_info->Data.it_that->Data.OverlapVertexIterator = _it_this;

					intersectionPointQuene.PushFront(_it_this);
					break;

				case Edge::Coincide_2IntersectionPoints :
					_it_this = polygon_this.InsertVertex(it_info->Data.it_this,it_info->Data.it_that->Data.Coord);
					_it_that = polygon_that.InsertVertex(it_info->Data.it_that,it_info->Data.it_this->Data.Coord);
					_it_this->Data.OverlapVertexIterator = it_info->Data.it_that;
					it_info->Data.it_that->Data.OverlapVertexIterator = _it_this;
					_it_that->Data.OverlapVertexIterator = it_info->Data.it_this;
					it_info->Data.it_this->Data.OverlapVertexIterator = _it_that;

					intersectionPointQuene.PushFront(it_info->Data.it_this);		
					break;

				case Edge::Disjoint :
				default:;
					// do nothing
				}

				// 插入完一个平凡顶点，this顶点顺次挪一个
				if (NeedShiftThisVertexNext)
				{
					it_info->Data.it_this_prior = it_info->Data.it_this;
					it_info->Data.it_this = it_info->Data.it_this->Next;
				}

				it_info = it_info->Next;
			} while (it_info != it_first_info);
		}
									   

		/*
		 * @brief	求出两多边形全部交点，并将多边形有向图存放在result中
		 * @param	polygon 求交的另一多边形
					result1/result2 相交后两多边形存储位置
					intersectionPointQuene 交点队列
		 * @notice	此方法只能被多边形布尔运算方法调用
					即使是不活动边也参与相交哦！
		 */
		static void _ComputeAllIntersectionPoint(
			Polygon& result1,
			Polygon& result2,
			SingleList<VertexIterator>& intersectionPointQuene)
		{
			// 此方法只能被多边形布尔运算等方法调用，无需在此判断多边形有效性

			VertexIterator thisFirstVertex = result1.FirstVertex();
			VertexIterator thisVertex = thisFirstVertex;
			VertexIterator thisPriorVertex = result1.LastVertex();

			do 
			{	
				VertexIterator thatFirstVertex = result2.FirstVertex();
				VertexIterator thatVertex = thatFirstVertex;
				VertexIterator thatPriorVertex = result2.LastVertex();

				// 保存当前边所有的交点的链表
				CircularList<IntersectionInfo> allInterPoints;

				Edge thisEdge(thisVertex->Data.Coord,thisVertex->Next->Data.Coord);
				if (thisEdge.IsValid())
				{
					IntersectionInfo interInfo;
					interInfo.it_this = thisVertex;
					interInfo.it_this_prior = thisPriorVertex;
					// 遍历that的所有边
					do 
					{
						Edge thatEdge(thatVertex->Data.Coord,thatVertex->Next->Data.Coord);
						if (thatEdge.IsValid())
						{
							// 如果有交点则存储交点信息
							interInfo.position = thisEdge.IntersectWith(thatEdge,interInfo.point);
							if (interInfo.position != Edge::Disjoint)
							{
								interInfo.it_that = thatVertex;
								interInfo.it_that_prior = thatPriorVertex;
								_InsertAndSortByDistance(allInterPoints,interInfo);
							}
						}
						thatPriorVertex = thatVertex;
						thatVertex = thatVertex->Next;				
					} while (thatVertex != thatFirstVertex);

				}
			
				// this多边形的边先递增到下一边
				thisPriorVertex = thisVertex;
				thisVertex = thisVertex->Next;

				// 然后才依次插入到两多边形中，并建立好绑定关系（因为插入新顶点后会影响原多边形的边递增）
				_ConstructIntersectionPoint(result1,result2,allInterPoints,intersectionPointQuene);

			} while (thisVertex != thisFirstVertex);

		}


		/*
		 * @brief	两多边形求交
		 * @param	polygon 求交的另一多边形
					result 相交后的多边形交集部分
		 */
		void IntersectWith(const Polygon& polygon,Polygon& result)
		{
			// 算法简述
			// 1.	求出两多边形全部交点，并将交点添加到多边形的平凡顶点中，
			//		关于求交点的算法参见_ComputeAllIntersectionPoint方法
			//		并从_ComputeAllIntersectionPoint方法中得到一个关键交点链表

			// 2.	先预处理两多边形包含的情形

			// 3.	遍历关键交点链表中的每一个顶点迭代子，从某一迭代子出发，
			//		如果出发的这一边的中点不在polygon外部，那就开始沿边爬行到下一顶点
			//		每经过一个顶点要把顶点标记为visited，并且将坐标和活动性两个属性拷贝并PushBack到交集多边形中
			//		如果这个顶点有OverlapVertex，那就切换到OverlapVertex并标记visited
			//		继续沿着边爬行，直到爬到一个顶点和最开始的顶点完全相同（指顶点迭代子相同）
			//		如果交集多边形只有两个顶点则忽略，否则将其Merge到result中

			// 4.	将关键交点链表第一个元素Pop，重复步骤3，直到关键交点链表为空
		

			// 多边形有效性预处理
			if (listVertex.IsEmpty() || polygon.listVertex.IsEmpty()) 
			{
				// 交集为空
				result = Polygon();
				return;
			}
		
			// 完全嵌套的情况预处理
			VertexIterator _firstVertex = FirstVertex();
			VertexIterator _vertex = _firstVertex;
			do 
			{
				if (polygon.TestPoint(_vertex->Data.Coord) == ExternalPoint)
				{
					_vertex = _vertex->Next;
					break;
				}
				_vertex = _vertex->Next; 
			} while (_vertex != _firstVertex);

			// 若退出循环时是起始点是第一个点的话，表示this的所有点都是内点，交集就是this
			if (_vertex == _firstVertex)
			{
				// 顶点深拷贝
				result = *this;
				return;
			}

			// 完全嵌套情况预处理2
			_firstVertex = polygon.FirstVertex();
			_vertex = _firstVertex;
			do 
			{
				if (this->TestPoint(_vertex->Data.Coord) == ExternalPoint)
				{
					_vertex = _vertex->Next;
					break;
				}
				_vertex = _vertex->Next; 
			} while (_vertex != _firstVertex);
			if (_vertex == _firstVertex)
			{
				// 顶点深拷贝
				result = polygon;
				return;
			}

			// 以下先求出多边形的所有交点
			// poly1存放this插入了平凡顶点后的多边形，poly2存放polygon插入了平凡顶点后的多边形
			Polygon poly1 = *this, poly2 = polygon;
			SingleList<VertexIterator> intersectionPointQuene;
			_ComputeAllIntersectionPoint(poly1,poly2,intersectionPointQuene);

		
			typename SingleList<VertexIterator>::Iterator it_it_vertex = NULL;
		

			it_it_vertex = intersectionPointQuene.First();
			while (it_it_vertex)
			{
				VertexIterator _IT_VERTEX_ = it_it_vertex->Data;

	#define _VERTEX_				_IT_VERTEX_->Data
	#define _VERTEX_NEXT_			_IT_VERTEX_->Next->Data
	#define _VERTEX_OVERLAP_		_VERTEX_.OverlapVertexIterator->Data
	#define _VERTEX_OVERLAP_NEXT_	_VERTEX_.OverlapVertexIterator->Next->Data

				if (!_VERTEX_.Visited)
				{
					_VERTEX_.Visited = true;
					PointType midPoint_startFromThis = (_VERTEX_.Coord + _VERTEX_NEXT_.Coord) / 2;
					PointType midPoint_startFromThat = (_VERTEX_OVERLAP_.Coord + _VERTEX_OVERLAP_NEXT_.Coord) / 2;
					PointPosition pos_this = polygon.TestPoint(midPoint_startFromThis);
					PointPosition pos_that = this->TestPoint(midPoint_startFromThat);
				
					// 至少有一个在内部才可能
					if (pos_this == InteriorPoint || pos_that == InteriorPoint)
					{
						// 两个出发边不是都在对方的内部、边上、外部，才有可能是出发边
						// 且求交集的出发边为在对方内部的边
						Polygon intersectionPart;
						if (pos_that == InteriorPoint)
						{
							_IT_VERTEX_ = _VERTEX_.OverlapVertexIterator;
						}

						// 先处理起点
						_VERTEX_.Visited = true;
						VertexIterator justInserted = intersectionPart.InsertVertex(_VERTEX_);

						// 只保留坐标与活动性两个属性，所以下面两个清0，（下同）
						justInserted->Data.Visited = false;
						justInserted->Data.OverlapVertexIterator = NULL;

						_IT_VERTEX_ = _IT_VERTEX_->Next;

						// 开始顺着边爬行（爬回坐标相等的点就爬行结束）
						while (_VERTEX_.Coord != it_it_vertex->Data->Data.Coord)
						{
							_VERTEX_.Visited = true;
							justInserted = intersectionPart.InsertVertex(_VERTEX_);
							justInserted->Data.Visited = false;
							justInserted->Data.OverlapVertexIterator = NULL;

							if (_VERTEX_.OverlapVertexIterator)
							{
								// 切换到另一个多边形
								_IT_VERTEX_ = _VERTEX_.OverlapVertexIterator;
								_VERTEX_.Visited = true;
							}
							_IT_VERTEX_ = _IT_VERTEX_->Next;
						}

						// 爬行结束，判断intersectionPart有效性
						if (	!intersectionPart.listVertex.IsEmpty() 
							&&	intersectionPart.FirstVertex()->Next != intersectionPart.LastVertex())
						{
							// 闭合多边形
							justInserted = intersectionPart.InsertVertex(intersectionPart.FirstVertex()->Data.Coord);
							justInserted->Data.IsStartPointOfInactiveEdge = true;
							// 大于等于3个顶点就合并多边形
							result.Merge(intersectionPart);
						}
					}
				}
				intersectionPointQuene.PopFront();
				it_it_vertex = intersectionPointQuene.First();
			}
		}



		/*
		 * @brief	两多边形求并
		 * @param	polygon 求并的另一多边形
					result 相并后的多边形交集部分
		 */
		void UnionWith(const Polygon& polygon,Polygon& result)
		{

		}



		/*
		 * @brief	多边形填充和描边
		 */
		static int descending(const void* a,const void* b)
		{
			return *(const CoordType*)a < *(const CoordType*)b;
		}
		void Fill(GraphicsDevice& dc, ColorType color)
		{
			if (listVertex.IsEmpty()) return;

			LineSegment<GraphicsDevice>* edge = new LineSegment<GraphicsDevice>[VertexCount];

			// 初始化边表   
			// 相邻两点依次连接，并把y值小点放到P1，大的放到P2，水平边可以不放入表中(能一定程度上提高速度)
			VertexIterator firstVertex = FirstVertex();
			VertexIterator vertex = firstVertex;
			INT16 edgeCount = 0;
			do
			{
				if (!vertex->Data.IsStartPointOfInactiveEdge)
				{
					if (vertex->Data.Coord.y > vertex->Next->Data.Coord.y)
					{
						edge[edgeCount].P1 = vertex->Next->Data.Coord;
						edge[edgeCount].P2 = vertex->Data.Coord;
						++edgeCount;
					}
					else if (vertex->Data.Coord.y < vertex->Next->Data.Coord.y)
					{
						edge[edgeCount].P1 = vertex->Data.Coord;
						edge[edgeCount].P2 = vertex->Next->Data.Coord;
						++edgeCount;
					}
				}
				vertex = vertex->Next;
			} while (vertex != firstVertex);


			//开始扫描   
			CoordType xv[12] = { 0 };	// 存放扫描线与多个边的交点的x坐标的数组
			INT16 xv_index = 0;								// 保存数组下标
		
			for(INT16 y=Ymin; y<Ymax; ++y)   
			{   
				xv_index = 0;
				for(INT16 i=0; i<edgeCount; ++i)   
				{   
					// 算出活化的边
					// 边的上面的点作闭点处理，下面的点作开点处理
					// 水平边不会进入到下面的分支但是能被很好地解决，
					// 因为水平边的左右两相邻边总有非水平边，这些非水平边的顶点区间内是可以被连在一起的
					// 【请在草稿纸上画出几种情况分析一下就明白了】
					if (y >= edge[i].P1.y && y < edge[i].P2.y)
					{   
						xv[xv_index++] = edge[i].P1.x + (y - edge[i].P1.y) * (edge[i].P2.x - edge[i].P1.x) / (edge[i].P2.y - edge[i].P1.y);
					}   
				}

				std::qsort(xv, xv_index, sizeof(CoordType), descending);
			
				for (INT16 i=0; i<=xv_index-2; i+=2) 
				{
					dc.SequentialSetPixel(xv[i+1], y, color, xv[i]-xv[i+1]);
				} 
			}   
			//结束绘制
			delete [] edge;
		}
		void Stroke(GraphicsDevice& dc, ColorType color) const
		{
			if (listVertex.IsEmpty()) return;

			VertexIterator first = FirstVertex();
			VertexIterator vertex = first;
			do 
			{
				if (!vertex->Data.IsStartPointOfInactiveEdge)
				{
					LineSegment<GraphicsDevice>(vertex->Data.Coord, vertex->Next->Data.Coord).Out(dc, color);
				}
				vertex = vertex->Next;
			} while (vertex != first);
		}
	


		/*
		 * @brief	多边形图像填充
		 * @param	img 图像
					offx 图像偏移起点x(在GDI中的x坐标作为图像的注册点)
					offy 图像偏移起点y
		 * @notice	多边形顶点坐标系与为参考系，超出图像范围的部分不输出
		 */
		void Fill(GraphicsDevice& dc, IMAGE<ColorType>& img, CoordType offx = 0, CoordType offy = 0)
		{
			if (listVertex.IsEmpty()) return;

			LineSegment<GraphicsDevice>* edge = new LineSegment<GraphicsDevice>[VertexCount];

			// 初始化边表   
			// 相邻两点依次连接，并把y值小点放到P1，大的放到P2，水平边可以不放入表中(能一定程度上提高速度)
			VertexIterator firstVertex = FirstVertex();
			VertexIterator vertex = firstVertex;
			INT16 edgeCount = 0;
			do
			{
				if (!vertex->Data.IsStartPointOfInactiveEdge)
				{
					if (vertex->Data.Coord.y > vertex->Next->Data.Coord.y)
					{
						edge[edgeCount].P1 = vertex->Next->Data.Coord;
						edge[edgeCount].P2 = vertex->Data.Coord;
						++edgeCount;
					}
					else if (vertex->Data.Coord.y < vertex->Next->Data.Coord.y)
					{
						edge[edgeCount].P1 = vertex->Data.Coord;
						edge[edgeCount].P2 = vertex->Next->Data.Coord;
						++edgeCount;
					}
				}
				vertex = vertex->Next;
			} while (vertex != firstVertex);


			// 开始扫描   
			typename PointType::CoordValue xv[12] = { 0 };	// 存放扫描线与多个边的交点的x坐标的数组
			INT16 xv_index = 0;								// 保存数组下标

			// y范围需要重新确定，非图像区域不输出
			INT16 ymin = max(Ymin, offy);
			INT16 ymax = min((int)Ymax, offy+ img.Height);
			for(INT16 y=ymin; y<ymax; ++y)   
			{   
				xv_index = 0;
				for(INT16 i=0; i<edgeCount; ++i)   
				{   
					// 算出活化的边
					// 边的上面的点作闭点处理，下面的点作开点处理
					// 水平边不会进入到下面的分支但是能被很好地解决，
					// 因为水平边的左右两相邻边总有非水平边，这些非水平边的顶点区间内是可以被连在一起的
					// 【请在草稿纸上画出几种情况分析一下就明白了】
					if (y >= edge[i].P1.y && y < edge[i].P2.y)
					{   
						xv[xv_index] = edge[i].P1.x + (y - edge[i].P1.y) * (edge[i].P2.x - edge[i].P1.x) / (edge[i].P2.y - edge[i].P1.y);
					
						// 限定x的输出范围
						xv[xv_index] = xv[xv_index] > offx + img.Width ? offx + img.Width : xv[xv_index] < offx ? offx : xv[xv_index];
						++xv_index;
					}   
				}

				qsort(xv,xv_index,sizeof(typename PointType::CoordValue),descending);

				for (INT16 i=0; i<=xv_index-2; i+=2) 
				{   
					dc.SequentialSetPixel(xv[i+1], y, img.Position(xv[i+1]-offx,y-offy), xv[i]-xv[i+1]);
				} 
			}   
			//结束绘制
			delete [] edge;
		}

	

		/*
		 * @brief	求多变形面积，顶点按照逆时针顺序为正，顺时针为负
		 */
		INT32 Area() const
		{
			if (listVertex.IsEmpty()) return 0;

			VertexIterator v1 = listVertex.First();
			VertexIterator pv = v1;
			VertexIterator v2 = pv->Next;
			VertexIterator v3 = v2->Next;

			// 若多边形只有一个或两个顶点，那么v1 == v3恒成立，可以不用判断边数
			INT32 area = 0;
			while (v3 != v1)
			{
				area += (v3->Data.Coord.x - v1->Data.Coord.x) 
					  * (v2->Data.Coord.y - v1->Data.Coord.y) 
					  - (v2->Data.Coord.x - v1->Data.Coord.x) 
					  * (v3->Data.Coord.y - v1->Data.Coord.y);
				pv = pv->Next;
				v2 = pv->Next;
				v3 = v2->Next;
			}
			return area/2;
		}


		/*
		 * @brief	使多边形正向，各顶点逆时针排序，或者反向，各顶点顺时针排序
		 */
		void MakeForward()
		{
			if (Area() < 0)
			{
				listVertex.Reverse();
			
				// 逆序后还要将闭合顶点处的活动状态向后挪一下
				FirstVertex()->Data.IsStartPointOfInactiveEdge = false;
				LastVertex()->Data.IsStartPointOfInactiveEdge = true;
			}
		}
		void MakeReverse()
		{
			// 仅仅是判断条件不同
			if (Area() > 0)
			{
				listVertex.Reverse();

				// 逆序后还要将闭合顶点处的活动状态向后挪一下
				FirstVertex()->Data.IsStartPointOfInactiveEdge = false;
				LastVertex()->Data.IsStartPointOfInactiveEdge = true;
			}
		}


		/*
		 * @brief	获取第一个或最后一个节点迭代子
		 */
		VertexIterator FirstVertex() const
		{
			return listVertex.First();
		}
		VertexIterator LastVertex() const
		{
			return listVertex.Last();
		}

	

		/*
		 * @brief	在某个节点后插入一个节点或者插入到最后一个节点末尾
		 * @param	afterThis 在这个节点后
					withThis 插入这个节点
		 * @notice	插入的顶点请保证是活动顶点且不使得边边重合，顶点活动状态与前一点保持一直
					请注意多边形是自动闭合的，默认插入节点到不活动尾节点后是没有显示效果的
		 */
		VertexIterator InsertVertex(const Vertex& withThis)
		{
			++VertexCount;
			Ymax = max(Ymax,withThis.Coord.y);
			Ymin = min(Ymin,withThis.Coord.y);
			return listVertex.PushBack(withThis);
		}
		VertexIterator InsertVertex(const VertexIterator afterThis,const Vertex& withThis)
		{
			++VertexCount;
			Ymax = max(Ymax,withThis.Coord.y);
			Ymin = min(Ymin,withThis.Coord.y);
			return listVertex.Insert(afterThis,withThis);
		}
		VertexIterator InsertVertex(const PointType& withThis)
		{
			// 此方法插入的顶点跟随上一顶点活动状态
			return InsertVertex(Vertex(withThis,LastVertex()->Data.IsStartPointOfInactiveEdge));
		}
		VertexIterator InsertVertex(const VertexIterator afterThis,const PointType& withThis)
		{
			// 此方法插入的顶点跟随上一顶点活动状态
			return InsertVertex(afterThis,Vertex(withThis,afterThis->Data.IsStartPointOfInactiveEdge));
		}



		/*
		 * @brief	与另一多边形合并成个一个包含多个小多边形的多边形
		 * @param	poly 另个一多边形（顶点值拷贝）
		 * @notice	0宽度边会自动生成
		 */
		void Merge(const Polygon& poly)
		{
			// 暂时还没实现两个链表拼接，以后看看有没有什么办法吧
			VertexIterator first = poly.FirstVertex();
			VertexIterator vertex = first;
			do 
			{
				// 整个顶点插入，包括顶点活动状态
				InsertVertex(vertex->Data);
				vertex = vertex->Next;
			} while (vertex != first);
		
			// 闭合操作
			InsertVertex(Vertex(FirstVertex()->Data.Coord,true));
		}



		enum PointPosition
		{
			ExternalPoint,
			InteriorPoint,
			BoundaryPoint
		};
		/*
		 * @brief	测试一个点是否在多边形内
		 * @retval	内点返回InteriorPoint，外点返回ExternalPoint，边界点返回BoundaryPoint
		 */
		PointPosition TestPoint(const PointType& point) const
		{
			typename PointType::CoordValue xv[12] = { 0 };					// 存放扫描线与多个边的交点的x坐标的数组
			INT16 xv_index = 0;												// 保存数组下标
			VertexIterator firstVertex = listVertex.First();
			VertexIterator vertex = firstVertex;
			do 
			{
				if (!vertex->Data.IsStartPointOfInactiveEdge)
				{
					// 边点的话，水平边要考虑进来
					if (	vertex->Data.Coord.y == vertex->Next->Data.Coord.y 
						&&	vertex->Data.Coord.y == point.y)
					{
						// x范围匹配
						if (	vertex->Data.Coord.x >= point.x && vertex->Next->Data.Coord.x <= point.x
							||	vertex->Data.Coord.x <= point.x && vertex->Next->Data.Coord.x >= point.x)
						{
							return BoundaryPoint;
						}
					}
					else if (	vertex->Data.Coord.y <= point.y && vertex->Next->Data.Coord.y > point.y
							||	vertex->Next->Data.Coord.y <= point.y && vertex->Data.Coord.y > point.y)
					{
						xv[xv_index++] = vertex->Data.Coord.x 
							+ (point.y - vertex->Data.Coord.y) 
							* (vertex->Next->Data.Coord.x - vertex->Data.Coord.x) 
							/ (vertex->Next->Data.Coord.y - vertex->Data.Coord.y);
					}
				}
				vertex = vertex->Next;
			} while (vertex != firstVertex);
		
			qsort(xv,xv_index,sizeof(typename PointType::CoordValue),descending);
			for (INT16 i=0; i<=xv_index-2; i+=2) 
			{   
				if (point.x == xv[i] || point.x == xv[i+1])
				{
					return BoundaryPoint;
				}
				else if (point.x < xv[i] && point.x > xv[i+1])
				{
					return InteriorPoint;
				}
			}

			return ExternalPoint;
		}


		/*
		 * @brief	测试顶点中是否含有某个点
		 * @param	point 要测试的点（仅测试坐标）
					vertex 要测试的顶点是否是多边形上的顶点
		 * @retval	含有则返回true
		 */
		bool HasVertex(const PointType& point) const
		{
			if (listVertex.IsEmpty()) return false;

			VertexIterator first = FirstVertex();
			VertexIterator vertex = first;
			do 
			{
				if (vertex->Data.Coord == point)
				{
					return true;
				}
				vertex = vertex->Next;
			} while (vertex != first);
			return false;
		}
		bool HasVertex(const VertexIterator _vertex) const
		{
			if (listVertex.IsEmpty()) return false;

			VertexIterator first = FirstVertex();
			VertexIterator vertex = first;
			do 
			{
				if (vertex == _vertex)
				{
					return true;
				}
				vertex = vertex->Next;
			} while (vertex != first);
			return false;
		}

	};


	template< class GraphicsDevice >
	typename Polygon<GraphicsDevice>::VertexIterator Polygon<GraphicsDevice>::IntersectionInfo::it_this = NULL;
	template< class GraphicsDevice >
	typename Polygon<GraphicsDevice>::VertexIterator Polygon<GraphicsDevice>::IntersectionInfo::it_this_prior = NULL;



	template< typename GraphicsDevice >
	class IMAGE
	{
	public:


		typedef typename GraphicsDevice::CoordType CoordType;
		typedef typename GraphicsDevice::SizeType SizeType;
		typedef typename GraphicsDevice::ColorType ColorType;
		typedef Point2D<CoordType> PointType;
		typedef Vector2D<CoordType> VectorType;


		IMAGE(const void* img, SizeType width, SizeType height)
			: Source((const ColorType*)img), Width(width), Height(height)
		{ }


		const ColorType* Source;
		SizeType Width, Height;


		const ColorType* Position(CoordType x, CoordType y) const
		{
			return Source + y * Width + x;
		}
		const ColorType* Position(PointType p) const
		{
			return Source + p.y * Width + p.x;
		}



		/*
		 * @brief	图像输出函数
		 * @param	pos 左上角坐标(注册点)
		 *			img 图像源对象
		 *			imgRef 图像的相对坐标作为输出起始位置
		 *			ref 图像左上角为原点的向量的终点作为输出起始位置
		 *			clientWidth/clientHeight 图像宽和高
		 */
		void Out(GraphicsDevice& dc, PointType pos) const
		{
			if (pos.x >= GraphicsDevice::MAX_WIDTH || pos.y >= GraphicsDevice::MAX_HEIGHT ) return;

			// 输出整张
			const ColorType* source = Source;
			SizeType OutputWidth = min(int(Width),(int)GraphicsDevice::MaxWidth-pos.x);
			SizeType OutputHeight = min(int(Height),(int)GraphicsDevice::MaxHeight-pos.y);
			for (SizeType i=0; i < OutputHeight; i++) 
			{
				dc.SequentialSetPixel(pos.x, pos.y + i, source, OutputWidth);	// 输出这一行
				source += Width;												// 转到下一行
			}
		}
		void Out(GraphicsDevice& dc, 
				 PointType pos, 
				 PointType imgRef, 
				 const SizeType clientWidth, 
				 const SizeType clientHeight) const
		{
			if (pos.x >= GraphicsDevice::MAX_WIDTH || pos.y >= GraphicsDevice::MAX_HEIGHT ) return;

			// 输出一部分
			const ColorType* source = Position(imgRef);
			UINT16 OutputWidth = min(int(clientWidth),(int)GraphicsDevice::MaxWidth-pos.x);
			UINT16 OutputHeight = min(int(clientHeight),(int)GraphicsDevice::MaxHeight-pos.y);
			for (UINT16 i=0; i<OutputHeight; i++) 
			{
				dc.SequentialSetPixel(pos.x, pos.y + i, source, OutputWidth);	// 输出这一行
				source += Width;												// 转到下一行
			}
		}
		void Out(GraphicsDevice& dc, 
				 PointType pos, 
				 VectorType ref, 
				 const SizeType clientWidth, 
				 const SizeType clientHeight) const
		{
			if (pos.x >= GraphicsDevice::MAX_WIDTH || pos.y >= GraphicsDevice::MAX_HEIGHT ) return;

			// 输出一部分
			const ColorType* source = Position(ref.x,ref.y);
			SizeType OutputWidth = min(int(clientWidth),(int)GraphicsDevice::MaxWidth-pos.x);
			SizeType OutputHeight = min(int(clientHeight),(int)GraphicsDevice::MaxHeight-pos.y);
			for (UINT16 i=0; i<OutputHeight; i++) 
			{
				dc.SequentialSetPixel(pos.x, pos.y + i, source, OutputWidth);	// 输出这一行
				source += Width;												// 转到下一行
			}
		}

	};



}


#endif /* _GRAPHICAL_H_ */
