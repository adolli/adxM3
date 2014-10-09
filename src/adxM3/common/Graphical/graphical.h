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
	 * ����ֱ��
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
		 * @brief	ͨ��xֵ��yֵ�������һ��ֵ
		 * @notice	��ע����߶β�Ҫ��ֱ����ˮƽ!
		 */
		CoordType YofX(CoordType x) const
		{
			// ��ע���޷�����ֱ�����Ŷ��
			return (x - P1.x) * (P2.y - P1.y) / (P2.x - P1.x) + P1.y;
		}
		CoordType XofY(CoordType y) const
		{
			// ��ע���޷�����ˮƽ�����Ŷ��
			return (y - P1.y) * (P2.x - P1.x) / (P2.y - P1.y) + P1.x;
		}


		/*
		 * @brief	���Ը�ֱ��֪���Ǵ�ֱ�߻���ˮƽ��
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
		 * @brief	���Ը�ֱ���Ƿ���Ч�����ж��������Ƿ�������ͬ
		 */
		bool IsValid() const
		{
			return P1 != P2;
		}

		/*
		 * @brief	���Ըõ��Ƿ���ֱ����
		 * @param	p �����
		 */
		bool HasPointOnLine(const PointType& p) const
		{
			typename PointType::CoordValue dx = P2.x - P1.x;
		
			// �����߲��Ǵ�ֱ
			if (dx != 0) 
			{
				// ����ֱ�߷�����ֵ��Ϊ�о�
				if (p.y == (p.x - P1.x) * (P2.y - P1.y) / dx + P1.y) 
				{
					return true;
				}
			} 
			// ��ֱʱֻ��Ƚ�x�����Ƿ����
			else if (P1.x == p.x)
			{
				return true;
			}
		
			return false;
		}




	};


	// �����߶Σ�P1�Ǳյ㣬P2�ǿ���
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
		 * @brief	���Ըõ��Ƿ����߶���
		 * @param	p �����
		 */
		bool HasPointOnSegment(const PointType& p) const
		{
			// �ж�����
			if (HasPointOnLine(p)) 
			{
				// �жϷ�Χ,x��y��Χ��Ҫ�жϣ����Ƿ�ֱ���
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
	


		// ���߶��ཻ�����ö�٣����߶ξ�Ϊ����ҿ����߶Σ�
		enum INTERSECTIONS
		{
			Disjoint,						// ���ཻ
			AtEndpoint,						// �����ڶ˵�
			AtEdge,							// �����ڱ���

			Coincide_this,					// ���߶��غϣ������ཻ������Ϊthis�Ķ˵㣩
			Coincide_L2,					// ���߶��غϣ������ཻ������ΪL2�Ķ˵㣩
			Coincide_2IntersectionPoints	// ���߶��غϣ������ཻ�����߶εĶ˵㶼�ǽ��㣩
		};



		/*
		 * @brief	��������һ���߶��Ƿ��ཻ
		 * @param	L2 ��һ���߶�
					intersection �ཻ�Ľ���
		 * @notice	������غϵ������intersection�����ȱ�ʾL2�Ķ˵�
		 */
		int IntersectWith(const LineSegment<GraphicsDevice>& L2, PointType& intersection) const
		{
			// ���߶�ƽ��
			// mk�����߶λ��ɻ��ı��ʽ��ֻ�������ж��Ƿ�ƽ��
			INT32 mk = (P2.y - P1.y) * (L2.P2.x - L2.P1.x) - (P2.x - P1.x) * (L2.P2.y - L2.P1.y);
			if (mk == 0) 
			{
				// ��б����ͬʱ����Ҫ�ж��Ƿ��غϣ���ֻ���ж϶˵��Ƿ�����һ���߶���
				if (L2.HasPointOnSegment(P1)) 
				{
					// �غ�ʱ��Ҫͨ���߶εķ����Ƿ��෴�ж��Ƿ�����������
					// �ִ�ֱ��Ǵ�ֱ�����
					if (this->IsVertical())
					{
						if ((L2.P1.y - L2.P2.y) * (P1.y - P2.y) <= 0)
						{
							// dy�ķ��Ų�ͬ��Ϊ0��ʾ�����෴
							intersection = L2.P1;
							return Coincide_2IntersectionPoints;
						}
					}
					else
					{
						if ((L2.P1.x - L2.P2.x) * (P1.x - P2.x) <= 0)
						{
							// dx�ķ��Ų�ͬ��Ϊ0��ʾ�����෴
							intersection = L2.P1;
							return Coincide_2IntersectionPoints;
						}
					}
				
					intersection = P1;
					return Coincide_this;
				} 
				// �����������֧ʱ����P1��������L2�ϣ������������֧�ڲ����ܳ���˫���������
				// ��ôֻ������������������
				else if (HasPointOnSegment(L2.P1))
				{	
					intersection = L2.P1;
					return Coincide_L2;
				}
				return Disjoint;
			}

			// ������һ��ֱ�ߴ�ֱʱ������ֱ���ж��ཻ���
			if (this->IsVertical())
			{
				// �ж�this->P1.x�ڲ���L2��x��Χ��
				if (P1.x > L2.P2.x && P1.x <= L2.P1.x || P1.x < L2.P2.x && P1.x >= L2.P1.x)
				{
					// x��Χ���㣬�ж�yֵ��ΧӦ����this��y��Χ��
					typename PointType::CoordValue _y = L2.YofX(P1.x);
					if (_y > P2.y && _y < P1.y || _y < P2.y && _y > P1.y)
					{
						// �պ��ཻ��L2�˵�����
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
					// �ཻ�ڶ˵���������������֧�����ۣ���֤������ȷ��
					else if (_y == P1.y)
					{
						intersection = PointType(P1.x,_y);
						return AtEndpoint;
					}
				}

				// ����x��Χ�϶����ཻ
				return Disjoint;
			}
			// ����ֱ�����ͬʱ��ֱ��ô����б����ͬ�ķ�֧�жϵ����������ﲻ�ô�����ֱ��ͬʱ��ֱ�����
			else if (L2.IsVertical())
			{
				// �ж�L2.P1.x�ڲ���this��x��Χ��
				if (L2.P1.x > P2.x && L2.P1.x <= P1.x || L2.P1.x < P2.x && L2.P1.x >= P1.x)
				{
					// x��Χ���㣬�ж�yֵ��ΧӦ����L2��y��Χ��
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

			// �������ֱҲ��ƽ�У��Ǿͼ��㽻��ֵ
			typename PointType::CoordValue _x 
				= ((P1.x * (P2.y - P1.y) + (L2.P1.y - P1.y) * (P2.x - P1.x)) * (L2.P2.x - L2.P1.x)
				- L2.P1.x * (L2.P2.y - L2.P1.y) * (P2.x - P1.x))
				/ mk;

			// �ж�x�Ƿ��������߶ε�x��Χ��(��ʱ�Ȳ������ཻ�ڶ˵�����)
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
			// ����2��֧�Ƴٵ��������ж�һ��ĳ���Ƿ����߶��Ͽɽ���ʱ�临�Ӷȣ��󲿷�����½��㲻�Ƕ˵�
			else if (_x == P1.x && L2.HasPointOnSegment(P1))
			{
				// �ж��Ƿ��ཻ�ڶ˵㴦
				intersection = P1;
				return AtEndpoint;
			} 
			else if (_x == L2.P1.x && HasPointOnSegment(L2.P1)) 
			{
				// �ж��Ƿ��ཻ�ڶ˵㴦
				intersection = L2.P1;
				return AtEndpoint;
			}
		
			// ��������κ�����Ͳ��ཻ
			return Disjoint;
		}



		/*
		 * @brief	���豸���ͼ��
		 */
		void Out(GraphicsDevice& dc, ColorType color) const
		{
			CoordType xerr = 0, yerr = 0;
			CoordType xinc = 0, yinc = 0; 

			CoordType dx = P2.x - P1.x;				//������������ 
			CoordType dy = P2.y - P1.y; 
			CoordType uRow = P1.x; 
			CoordType uCol = P1.y; 
	
			if (dx > 0) 
			{
				xinc = 1;						//���õ������� 
			} 
			else if (dx == 0) 
			{
				xinc = 0;						//��ֱ�� 
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
				yinc=0;							//ˮƽ�� 
			} 
			else 
			{
				yinc = -1; 
				dy = -dy;
			} 

			CoordType distance = max(dx,dy);		//ѡȡ�������������� 
	
			//������� 
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
	 * �����࣬�����ϽǺ����½�ȷ��һ����
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
		 * @brief	���β������㺯��
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
		 * @brief	�ж�ĳ���Ƿ��ڸþ�����
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
		 * @brief	ȡ���θ��Խ��ߵ�������
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
		 * @brief	�жϴ˾����Ƿ�����һ�������ཻ
		 * @param	rect ��֮�󽻼��ľ���
					remain �˾��Σ�����rect����δ�ཻ��L�����򱣴�������
		 * @retval	�����ཻ�ľ�������
		 */
		void IntersectWith(const Rectangle& rect, Rectangle& result) const;



		/*
		 * @brief	�жϴ˾����Ƿ���Ч������Ƿ����1
		 * @retval	true ������ߴ���1������false
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
			// ȷ������������С�߽磬���ⳬ����Ļ
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
	 * Բ�࣬Բ����뾶ȷ��һ��Բ
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
			CoordType di = 3 - (Radius << 1);             //�ж��¸���λ�õı�־
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
				//ʹ��Bresenham�㷨��Բ     
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
	 * ������ࣨά��һ����������
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


		// ��ע��ά��VertexCount������0��

		// ����ο����ǰ���͹�ģ����������ཻ�ģ������Ƕ��С�������0��ȱ�������һ���
		// ����ι���ʱ���Զ��պϴ���������Ӷ�������һ��ʹ֮���ڵ�һ�㣬����Ϊ�����
		// ������һ�㱾��͵��ڵ�һ���������ⲽ
		// ��������PointType���͵ĳ�ʼ�������а��������0��ȱ����ӵ��Ӷ���Σ�
		// ��Ҫ��������Ӷ���λ��߶�����ڶ�������Merge����


		class Vertex;

		// ����ζ������������˵��
		typedef typename CircularList<Vertex>::Iterator VertexIterator;

		// ����α�����˵��
		typedef LineSegment<GraphicsDevice> Edge;


		/*
		 * ����ζ�����
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
		
			// ָ�򽻵����һ�غϵ㣨���ڶ���β�������ʱ�õ���
			VertexIterator OverlapVertexIterator;

			// ���С�����ͨ��0��ȱ�������ʱ����Щ���ӱ߱����Ϊ�ǻ�ߣ�
			// ���ն�������˳����Щ�ߵ�����IsStartPointOfInactiveEdgeֵΪtrue
			// ��������ʱĬ��Ϊfalse
			// ������������Merge����
			bool IsStartPointOfInactiveEdge;

			// �ڶ���β����������Ƿ񱻷��ʹ����
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

			// ���һ�����Ǳպϵ㣨��һ�㲻ͬ����Ҫ����������һ��
			if (pts[count-1] != pts[0])
			{
				// ��ʵ���ǲ����һ����һ����ͬ�ĵ�Ŷ��
				listVertex.PushBack(Vertex(pts[0],true));
			}
			else
			{
				// ����û��Ѿ��ڵ��б��бպ������һ�㣬Ҫ������㲻����
				LastVertex()->Data.IsStartPointOfInactiveEdge = true;
			}

			// �����нڵ㰴����ʱ�루��������������
			MakeForward();
		}
		
		
		CircularList<Vertex> listVertex;
		UINT8 VertexCount;
		CoordType Ymin, Ymax;

	

		

	
		/*
		 * @brief	����ĳ����������������������λ�ù�ϵ
		 * @param	a,b ��a��b��ƽ��Ƿ�ΧΪ�ڲ���ʣ�µ�ƽ��Ƿ�ΧΪ�ⲿ
					test ��������
		 * @retval	���˳����a->test->b�򷵻�Inside�����򷵻�Outside
		 * @notice	test��a��b�غ�ʱ��Outside
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
		 * ������ཻ������Ϣ
		 */
		struct IntersectionInfo// : public _cout_debuger_<IntersectionInfo>
		{
			PointType point;
			int position;
			VertexIterator it_that, it_that_prior;
		
			// this����εĶ��㣬��Ϊͬһ�齻����Ϣ��this����εı߲���仯
			static VertexIterator it_this, it_this_prior;
		};


		/*
		 * @brief	�������ľ������򽻵���Ϣ����
		 * @param	intersectionInfo ������Ϣ����
		 * @notice	�˷���ֻ�ܱ�����β������㷽������
		 */
		static void _InsertAndSortByDistance(CircularList<IntersectionInfo>& intersectionInfo,IntersectionInfo& ins)
		{
			if (intersectionInfo.IsEmpty()) 
			{
				// �ձ���ֱ�Ӳ���
				intersectionInfo.PushBack(ins);
				return;
			}

			INT32 distance_ins 
				= (ins.point.x - ins.it_this->Data.Coord.x) * (ins.point.x - ins.it_this->Data.Coord.x)
				+ (ins.point.y - ins.it_this->Data.Coord.y) * (ins.point.y - ins.it_this->Data.Coord.y);

			typename CircularList<IntersectionInfo>::Iterator it = intersectionInfo.First();
			typename CircularList<IntersectionInfo>::Iterator it_prior = intersectionInfo.Last();

			// ֻ��һ��Ԫ��ʱ��Ҫ���뵽ǰ��Ҫ���⴦��
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
		 * @brief	���ս�����Ϣ���������ڶ�Ӧ��������Ӷ�����й���ý���
		 * @param	intersectionInfo ������Ϣ����
					intersectionPointQuene �������������Ľ�����У������������õ���
					polygon_this ��ʱ��this
					polygon_that ��ʱ����һ�������
		 * @notice	�˷���ֻ�ܱ�����β������㷽������
					����˷�����intersectionInfo�г�Ա��it_this��it_this_prior�ᱻ�ı䣬��ȷ������ǰ����õ�ǰ������Ϣ
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
		
			// ����������뽻��
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
					// ����ƽ������
					_it_that = polygon_that.InsertVertex(it_info->Data.it_that,it_info->Data.point);
					_it_this = polygon_this.InsertVertex(it_info->Data.it_this,it_info->Data.point);
				
					// ƽ�����㻥���
					_it_that->Data.OverlapVertexIterator = _it_this;
					_it_this->Data.OverlapVertexIterator = _it_that;
				
					// ���ֵ���������Ϊ������bug�����඼PushFront
					intersectionPointQuene.PushBack(_it_this);
					break;

				case Edge::AtEndpoint :
					// �ڶ˵��ཻʱ����3�����
					// 1.thisEdge�����thatEdge�����ཻ
					// 2.thatEdge�����thisEdge�����ཻ
					// 3.thisEdge�����thatEdge����ཻ
					// ��1��2����������ڽ��������һ�㶥��ת���ɵ�3�����

					// ��������thatEdge�����ʱ
					if (it_info->Data.point != it_info->Data.it_this->Data.Coord)
					{
						polygon_this.InsertVertex(it_info->Data.it_this,it_info->Data.point);
						it_info->Data.it_this_prior = it_info->Data.it_this;
						it_info->Data.it_this = it_info->Data.it_this->Next;
					}
					// ��������thisEdge�����ʱ
					else if (it_info->Data.point != it_info->Data.it_that->Data.Coord)
					{
						polygon_that.InsertVertex(it_info->Data.it_that,it_info->Data.point);
						it_info->Data.it_that_prior = it_info->Data.it_that;
						it_info->Data.it_that = it_info->Data.it_that->Next;
					}

					// ����ֻ�账��������غ�ʱ�����

					// [����������ܻ����ֱ�Ӵ�����������֧��ת���������]
					// �ж�thatPriorEdge��thatEdge��thisPriorEdge��thisEdge 4��������λ��˳���ϵ
					// ������4���������ϸ��������ݸ�ֽ��
					// 1.thatһ�����ڣ�һ������
					// 2.this�������ڣ�that��������
					// 3.that�������ڣ�this��������
					// 4.that��this���߻�������
					this_a = it_info->Data.it_this_prior->Data.Coord - it_info->Data.it_this->Data.Coord;
					this_b = it_info->Data.it_this->Next->Data.Coord - it_info->Data.it_this->Data.Coord;
					that_a = it_info->Data.it_that_prior->Data.Coord - it_info->Data.it_that->Data.Coord;
					that_b = it_info->Data.it_that->Next->Data.Coord - it_info->Data.it_that->Data.Coord;


					if (	_PositionalOrderOfVectors(this_a,this_b,that_a) != _PositionalOrderOfVectors(this_a,this_b,that_b)
						||	_PositionalOrderOfVectors(that_a,that_b,this_a) == Inside 
						&&	_PositionalOrderOfVectors(that_a,that_b,this_b) == Inside)
					{
						// thatһ�����ڣ�һ�����⣬
						// ����this�������ڣ�that��������
						// �����㻥��󶨼���
						it_info->Data.it_this->Data.OverlapVertexIterator = it_info->Data.it_that;
						it_info->Data.it_that->Data.OverlapVertexIterator = it_info->Data.it_this;

						intersectionPointQuene.PushFront(it_info->Data.it_this);
					}
					else if (	_PositionalOrderOfVectors(this_a,this_b,that_a) == Inside 
							&&	_PositionalOrderOfVectors(this_a,this_b,that_b) == Inside)
					{
						// that�������ڣ�this��������
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
						// that��this���߻�������
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
				
					// ֻ����������Ƚ����⣬����Ҫ��thisEdge˳���ƶ�����ΪthisEdge��û�в����µĶ���
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

				// ������һ��ƽ�����㣬this����˳��Ųһ��
				if (NeedShiftThisVertexNext)
				{
					it_info->Data.it_this_prior = it_info->Data.it_this;
					it_info->Data.it_this = it_info->Data.it_this->Next;
				}

				it_info = it_info->Next;
			} while (it_info != it_first_info);
		}
									   

		/*
		 * @brief	����������ȫ�����㣬�������������ͼ�����result��
		 * @param	polygon �󽻵���һ�����
					result1/result2 �ཻ��������δ洢λ��
					intersectionPointQuene �������
		 * @notice	�˷���ֻ�ܱ�����β������㷽������
					��ʹ�ǲ����Ҳ�����ཻŶ��
		 */
		static void _ComputeAllIntersectionPoint(
			Polygon& result1,
			Polygon& result2,
			SingleList<VertexIterator>& intersectionPointQuene)
		{
			// �˷���ֻ�ܱ�����β�������ȷ������ã������ڴ��ж϶������Ч��

			VertexIterator thisFirstVertex = result1.FirstVertex();
			VertexIterator thisVertex = thisFirstVertex;
			VertexIterator thisPriorVertex = result1.LastVertex();

			do 
			{	
				VertexIterator thatFirstVertex = result2.FirstVertex();
				VertexIterator thatVertex = thatFirstVertex;
				VertexIterator thatPriorVertex = result2.LastVertex();

				// ���浱ǰ�����еĽ��������
				CircularList<IntersectionInfo> allInterPoints;

				Edge thisEdge(thisVertex->Data.Coord,thisVertex->Next->Data.Coord);
				if (thisEdge.IsValid())
				{
					IntersectionInfo interInfo;
					interInfo.it_this = thisVertex;
					interInfo.it_this_prior = thisPriorVertex;
					// ����that�����б�
					do 
					{
						Edge thatEdge(thatVertex->Data.Coord,thatVertex->Next->Data.Coord);
						if (thatEdge.IsValid())
						{
							// ����н�����洢������Ϣ
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
			
				// this����εı��ȵ�������һ��
				thisPriorVertex = thisVertex;
				thisVertex = thisVertex->Next;

				// Ȼ������β��뵽��������У��������ð󶨹�ϵ����Ϊ�����¶�����Ӱ��ԭ����εıߵ�����
				_ConstructIntersectionPoint(result1,result2,allInterPoints,intersectionPointQuene);

			} while (thisVertex != thisFirstVertex);

		}


		/*
		 * @brief	���������
		 * @param	polygon �󽻵���һ�����
					result �ཻ��Ķ���ν�������
		 */
		void IntersectWith(const Polygon& polygon,Polygon& result)
		{
			// �㷨����
			// 1.	����������ȫ�����㣬����������ӵ�����ε�ƽ�������У�
			//		�����󽻵���㷨�μ�_ComputeAllIntersectionPoint����
			//		����_ComputeAllIntersectionPoint�����еõ�һ���ؼ���������

			// 2.	��Ԥ����������ΰ���������

			// 3.	�����ؼ����������е�ÿһ����������ӣ���ĳһ�����ӳ�����
			//		�����������һ�ߵ��е㲻��polygon�ⲿ���ǾͿ�ʼ�ر����е���һ����
			//		ÿ����һ������Ҫ�Ѷ�����Ϊvisited�����ҽ�����ͻ���������Կ�����PushBack�������������
			//		������������OverlapVertex���Ǿ��л���OverlapVertex�����visited
			//		�������ű����У�ֱ������һ��������ʼ�Ķ�����ȫ��ͬ��ָ�����������ͬ��
			//		������������ֻ��������������ԣ�������Merge��result��

			// 4.	���ؼ����������һ��Ԫ��Pop���ظ�����3��ֱ���ؼ���������Ϊ��
		

			// �������Ч��Ԥ����
			if (listVertex.IsEmpty() || polygon.listVertex.IsEmpty()) 
			{
				// ����Ϊ��
				result = Polygon();
				return;
			}
		
			// ��ȫǶ�׵����Ԥ����
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

			// ���˳�ѭ��ʱ����ʼ���ǵ�һ����Ļ�����ʾthis�����е㶼���ڵ㣬��������this
			if (_vertex == _firstVertex)
			{
				// �������
				result = *this;
				return;
			}

			// ��ȫǶ�����Ԥ����2
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
				// �������
				result = polygon;
				return;
			}

			// �������������ε����н���
			// poly1���this������ƽ�������Ķ���Σ�poly2���polygon������ƽ�������Ķ����
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
				
					// ������һ�����ڲ��ſ���
					if (pos_this == InteriorPoint || pos_that == InteriorPoint)
					{
						// ���������߲��Ƕ��ڶԷ����ڲ������ϡ��ⲿ�����п����ǳ�����
						// ���󽻼��ĳ�����Ϊ�ڶԷ��ڲ��ı�
						Polygon intersectionPart;
						if (pos_that == InteriorPoint)
						{
							_IT_VERTEX_ = _VERTEX_.OverlapVertexIterator;
						}

						// �ȴ������
						_VERTEX_.Visited = true;
						VertexIterator justInserted = intersectionPart.InsertVertex(_VERTEX_);

						// ֻ�������������������ԣ���������������0������ͬ��
						justInserted->Data.Visited = false;
						justInserted->Data.OverlapVertexIterator = NULL;

						_IT_VERTEX_ = _IT_VERTEX_->Next;

						// ��ʼ˳�ű����У�����������ȵĵ�����н�����
						while (_VERTEX_.Coord != it_it_vertex->Data->Data.Coord)
						{
							_VERTEX_.Visited = true;
							justInserted = intersectionPart.InsertVertex(_VERTEX_);
							justInserted->Data.Visited = false;
							justInserted->Data.OverlapVertexIterator = NULL;

							if (_VERTEX_.OverlapVertexIterator)
							{
								// �л�����һ�������
								_IT_VERTEX_ = _VERTEX_.OverlapVertexIterator;
								_VERTEX_.Visited = true;
							}
							_IT_VERTEX_ = _IT_VERTEX_->Next;
						}

						// ���н������ж�intersectionPart��Ч��
						if (	!intersectionPart.listVertex.IsEmpty() 
							&&	intersectionPart.FirstVertex()->Next != intersectionPart.LastVertex())
						{
							// �պ϶����
							justInserted = intersectionPart.InsertVertex(intersectionPart.FirstVertex()->Data.Coord);
							justInserted->Data.IsStartPointOfInactiveEdge = true;
							// ���ڵ���3������ͺϲ������
							result.Merge(intersectionPart);
						}
					}
				}
				intersectionPointQuene.PopFront();
				it_it_vertex = intersectionPointQuene.First();
			}
		}



		/*
		 * @brief	���������
		 * @param	polygon �󲢵���һ�����
					result �ಢ��Ķ���ν�������
		 */
		void UnionWith(const Polygon& polygon,Polygon& result)
		{

		}



		/*
		 * @brief	������������
		 */
		static int descending(const void* a,const void* b)
		{
			return *(const CoordType*)a < *(const CoordType*)b;
		}
		void Fill(GraphicsDevice& dc, ColorType color)
		{
			if (listVertex.IsEmpty()) return;

			LineSegment<GraphicsDevice>* edge = new LineSegment<GraphicsDevice>[VertexCount];

			// ��ʼ���߱�   
			// ���������������ӣ�����yֵС��ŵ�P1����ķŵ�P2��ˮƽ�߿��Բ��������(��һ���̶�������ٶ�)
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


			//��ʼɨ��   
			CoordType xv[12] = { 0 };	// ���ɨ���������ߵĽ����x���������
			INT16 xv_index = 0;								// ���������±�
		
			for(INT16 y=Ymin; y<Ymax; ++y)   
			{   
				xv_index = 0;
				for(INT16 i=0; i<edgeCount; ++i)   
				{   
					// �����ı�
					// �ߵ�����ĵ����յ㴦������ĵ������㴦��
					// ˮƽ�߲�����뵽����ķ�֧�����ܱ��ܺõؽ����
					// ��Ϊˮƽ�ߵ����������ڱ����з�ˮƽ�ߣ���Щ��ˮƽ�ߵĶ����������ǿ��Ա�����һ���
					// �����ڲݸ�ֽ�ϻ��������������һ�¾������ˡ�
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
			//��������
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
		 * @brief	�����ͼ�����
		 * @param	img ͼ��
					offx ͼ��ƫ�����x(��GDI�е�x������Ϊͼ���ע���)
					offy ͼ��ƫ�����y
		 * @notice	����ζ�������ϵ��Ϊ�ο�ϵ������ͼ��Χ�Ĳ��ֲ����
		 */
		void Fill(GraphicsDevice& dc, IMAGE<ColorType>& img, CoordType offx = 0, CoordType offy = 0)
		{
			if (listVertex.IsEmpty()) return;

			LineSegment<GraphicsDevice>* edge = new LineSegment<GraphicsDevice>[VertexCount];

			// ��ʼ���߱�   
			// ���������������ӣ�����yֵС��ŵ�P1����ķŵ�P2��ˮƽ�߿��Բ��������(��һ���̶�������ٶ�)
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


			// ��ʼɨ��   
			typename PointType::CoordValue xv[12] = { 0 };	// ���ɨ���������ߵĽ����x���������
			INT16 xv_index = 0;								// ���������±�

			// y��Χ��Ҫ����ȷ������ͼ���������
			INT16 ymin = max(Ymin, offy);
			INT16 ymax = min((int)Ymax, offy+ img.Height);
			for(INT16 y=ymin; y<ymax; ++y)   
			{   
				xv_index = 0;
				for(INT16 i=0; i<edgeCount; ++i)   
				{   
					// �����ı�
					// �ߵ�����ĵ����յ㴦������ĵ������㴦��
					// ˮƽ�߲�����뵽����ķ�֧�����ܱ��ܺõؽ����
					// ��Ϊˮƽ�ߵ����������ڱ����з�ˮƽ�ߣ���Щ��ˮƽ�ߵĶ����������ǿ��Ա�����һ���
					// �����ڲݸ�ֽ�ϻ��������������һ�¾������ˡ�
					if (y >= edge[i].P1.y && y < edge[i].P2.y)
					{   
						xv[xv_index] = edge[i].P1.x + (y - edge[i].P1.y) * (edge[i].P2.x - edge[i].P1.x) / (edge[i].P2.y - edge[i].P1.y);
					
						// �޶�x�������Χ
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
			//��������
			delete [] edge;
		}

	

		/*
		 * @brief	��������������㰴����ʱ��˳��Ϊ����˳ʱ��Ϊ��
		 */
		INT32 Area() const
		{
			if (listVertex.IsEmpty()) return 0;

			VertexIterator v1 = listVertex.First();
			VertexIterator pv = v1;
			VertexIterator v2 = pv->Next;
			VertexIterator v3 = v2->Next;

			// �������ֻ��һ�����������㣬��ôv1 == v3����������Բ����жϱ���
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
		 * @brief	ʹ��������򣬸�������ʱ�����򣬻��߷��򣬸�����˳ʱ������
		 */
		void MakeForward()
		{
			if (Area() < 0)
			{
				listVertex.Reverse();
			
				// �����Ҫ���պ϶��㴦�Ļ״̬���Ųһ��
				FirstVertex()->Data.IsStartPointOfInactiveEdge = false;
				LastVertex()->Data.IsStartPointOfInactiveEdge = true;
			}
		}
		void MakeReverse()
		{
			// �������ж�������ͬ
			if (Area() > 0)
			{
				listVertex.Reverse();

				// �����Ҫ���պ϶��㴦�Ļ״̬���Ųһ��
				FirstVertex()->Data.IsStartPointOfInactiveEdge = false;
				LastVertex()->Data.IsStartPointOfInactiveEdge = true;
			}
		}


		/*
		 * @brief	��ȡ��һ�������һ���ڵ������
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
		 * @brief	��ĳ���ڵ�����һ���ڵ���߲��뵽���һ���ڵ�ĩβ
		 * @param	afterThis ������ڵ��
					withThis ��������ڵ�
		 * @notice	����Ķ����뱣֤�ǻ�����Ҳ�ʹ�ñ߱��غϣ�����״̬��ǰһ�㱣��һֱ
					��ע���������Զ��պϵģ�Ĭ�ϲ���ڵ㵽���β�ڵ����û����ʾЧ����
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
			// �˷�������Ķ��������һ����״̬
			return InsertVertex(Vertex(withThis,LastVertex()->Data.IsStartPointOfInactiveEdge));
		}
		VertexIterator InsertVertex(const VertexIterator afterThis,const PointType& withThis)
		{
			// �˷�������Ķ��������һ����״̬
			return InsertVertex(afterThis,Vertex(withThis,afterThis->Data.IsStartPointOfInactiveEdge));
		}



		/*
		 * @brief	����һ����κϲ��ɸ�һ���������С����εĶ����
		 * @param	poly ���һ����Σ�����ֵ������
		 * @notice	0��ȱ߻��Զ�����
		 */
		void Merge(const Polygon& poly)
		{
			// ��ʱ��ûʵ����������ƴ�ӣ��Ժ󿴿���û��ʲô�취��
			VertexIterator first = poly.FirstVertex();
			VertexIterator vertex = first;
			do 
			{
				// ����������룬��������״̬
				InsertVertex(vertex->Data);
				vertex = vertex->Next;
			} while (vertex != first);
		
			// �պϲ���
			InsertVertex(Vertex(FirstVertex()->Data.Coord,true));
		}



		enum PointPosition
		{
			ExternalPoint,
			InteriorPoint,
			BoundaryPoint
		};
		/*
		 * @brief	����һ�����Ƿ��ڶ������
		 * @retval	�ڵ㷵��InteriorPoint����㷵��ExternalPoint���߽�㷵��BoundaryPoint
		 */
		PointPosition TestPoint(const PointType& point) const
		{
			typename PointType::CoordValue xv[12] = { 0 };					// ���ɨ���������ߵĽ����x���������
			INT16 xv_index = 0;												// ���������±�
			VertexIterator firstVertex = listVertex.First();
			VertexIterator vertex = firstVertex;
			do 
			{
				if (!vertex->Data.IsStartPointOfInactiveEdge)
				{
					// �ߵ�Ļ���ˮƽ��Ҫ���ǽ���
					if (	vertex->Data.Coord.y == vertex->Next->Data.Coord.y 
						&&	vertex->Data.Coord.y == point.y)
					{
						// x��Χƥ��
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
		 * @brief	���Զ������Ƿ���ĳ����
		 * @param	point Ҫ���Եĵ㣨���������꣩
					vertex Ҫ���ԵĶ����Ƿ��Ƕ�����ϵĶ���
		 * @retval	�����򷵻�true
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
		 * @brief	ͼ���������
		 * @param	pos ���Ͻ�����(ע���)
		 *			img ͼ��Դ����
		 *			imgRef ͼ������������Ϊ�����ʼλ��
		 *			ref ͼ�����Ͻ�Ϊԭ����������յ���Ϊ�����ʼλ��
		 *			clientWidth/clientHeight ͼ���͸�
		 */
		void Out(GraphicsDevice& dc, PointType pos) const
		{
			if (pos.x >= GraphicsDevice::MAX_WIDTH || pos.y >= GraphicsDevice::MAX_HEIGHT ) return;

			// �������
			const ColorType* source = Source;
			SizeType OutputWidth = min(int(Width),(int)GraphicsDevice::MaxWidth-pos.x);
			SizeType OutputHeight = min(int(Height),(int)GraphicsDevice::MaxHeight-pos.y);
			for (SizeType i=0; i < OutputHeight; i++) 
			{
				dc.SequentialSetPixel(pos.x, pos.y + i, source, OutputWidth);	// �����һ��
				source += Width;												// ת����һ��
			}
		}
		void Out(GraphicsDevice& dc, 
				 PointType pos, 
				 PointType imgRef, 
				 const SizeType clientWidth, 
				 const SizeType clientHeight) const
		{
			if (pos.x >= GraphicsDevice::MAX_WIDTH || pos.y >= GraphicsDevice::MAX_HEIGHT ) return;

			// ���һ����
			const ColorType* source = Position(imgRef);
			UINT16 OutputWidth = min(int(clientWidth),(int)GraphicsDevice::MaxWidth-pos.x);
			UINT16 OutputHeight = min(int(clientHeight),(int)GraphicsDevice::MaxHeight-pos.y);
			for (UINT16 i=0; i<OutputHeight; i++) 
			{
				dc.SequentialSetPixel(pos.x, pos.y + i, source, OutputWidth);	// �����һ��
				source += Width;												// ת����һ��
			}
		}
		void Out(GraphicsDevice& dc, 
				 PointType pos, 
				 VectorType ref, 
				 const SizeType clientWidth, 
				 const SizeType clientHeight) const
		{
			if (pos.x >= GraphicsDevice::MAX_WIDTH || pos.y >= GraphicsDevice::MAX_HEIGHT ) return;

			// ���һ����
			const ColorType* source = Position(ref.x,ref.y);
			SizeType OutputWidth = min(int(clientWidth),(int)GraphicsDevice::MaxWidth-pos.x);
			SizeType OutputHeight = min(int(clientHeight),(int)GraphicsDevice::MaxHeight-pos.y);
			for (UINT16 i=0; i<OutputHeight; i++) 
			{
				dc.SequentialSetPixel(pos.x, pos.y + i, source, OutputWidth);	// �����һ��
				source += Width;												// ת����һ��
			}
		}

	};



}


#endif /* _GRAPHICAL_H_ */
