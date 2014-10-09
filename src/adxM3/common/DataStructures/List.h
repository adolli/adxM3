/*
 * List.h
 *
 *  Created on: 2013-10-4
 *      Author: Administrator
 */

#ifndef _LIST_H_
#define _LIST_H_

#include "../defs.h"

/*
 * 链表数据结构基类
 */ 
template <typename ValueType>
class List
{
public:

	// 数据结构说明与约定
	// 当链表空时，Head和Tail必为NULL，否则都不能为NULL
	// 链表只有一个节点时，Head和Tail都指向该节点
	// 链表有多个节点时，Head指向第一个节点，Tail指向最后一个节点
	// 建立时的节点插入顺序没有严格规定，由各个派生类自行实现


	// 链表长度类型
	typedef UINT16 size_t;

	// 节点类型
	class Node
	{
	public:
		Node(ValueType data)
			: Data(data), Next(NULL)
		{ }
		Node(ValueType data,Node* next)
			: Data(data), Next(next)
		{ }
		ValueType Data;
		Node* Next;
	};

	// 迭代子类型定义
	typedef Node* Iterator;

	// 构造函数与析构函数
	List()
		: Head(NULL), Tail(NULL)
	{ }
	virtual ~List()
	{
		// 析构留到派生类实现
	}
	Iterator Head, Tail;


	/*
	 * @brief	测试链表是否为空
	 * @retval	空则返回true
	 */
	bool IsEmpty() const
	{
		return Head == NULL;
	}


	/*
	 * @brief	测试链表是否只有一个元素
	 */
	bool OnlyOneElement() const
	{
		return Head == Tail;
	}


	/*
	 * @brief	交换头尾指针（一般用不到）
	 */
	void _SwapHeadAndTail()
	{
		Iterator temp = Head;
		Head = Tail;
		Tail = temp;
	}


	/*
	 * @brief	节点定位
	 */
	Iterator First() const
	{
		return Head;
	}
	Iterator Last() const
	{
		return Tail;
	}
	Iterator NextOf(Iterator node) const
	{
		if (node)
		{
			return node->Next;
		}
		return NULL;
	}


	/*
	 * @brief	求链表长度
	 * @retval	返回链表的长度
	 */
	virtual size_t Length() const = 0;


	/*
	 * @brief	链表插入操作，在末尾插入或者在某节点后插入
	 * @retval	指向刚插入的节点的指针
	 */
	virtual Iterator PushBack(const ValueType& Data) = 0;
	virtual Iterator PushFront(const ValueType& Data) = 0;
	virtual Iterator Insert(Iterator afterThis,const ValueType& Data) = 0;


	/*
	 * @brief	链表弹出操作，弹出Tail或者弹出Head指向的节点
	 * @notice	PopBack方法的时间复杂度为O(n)，尽量不使用
	 */
	virtual void PopBack() = 0;
	virtual void PopFront() = 0;



	/*
	 * @brief	清空整个链表，释放内存
	 */
	virtual void Clear() = 0;



	// 测试用输出函数
	virtual void ListOut() const = 0;
};


// 单链表数据结构
template <typename ValueType>
class SingleList
	: public List<ValueType>
{
public:

	// 数据结构说明与约定
	// Tail->Next == NULL 恒成立
	// 建立的时按照尾插法顺次排列


	// 基类用到的类型在派生类中需要重新说明一次
	typedef typename List<ValueType>::size_t 	size_t;
	typedef typename List<ValueType>::Node 		Node;
	typedef typename List<ValueType>::Iterator 	Iterator;

	// 基类数据成员在模版派生类中也要重新说明一次
	using List<ValueType>::Head;
	using List<ValueType>::Tail;

	SingleList()
		: List<ValueType>()
	{ }
	SingleList(ValueType pData[],size_t count)
		: List<ValueType>()
	{
		// 用尾插法建立链表，顺次排列
		if (count > 0)
		{
			// 预先把Head确定好
			Head = new Node(pData[0]);

			Iterator q = Head;
			for (size_t i=1; i<count; ++i)
			{
				Iterator p = new Node(pData[i]);
				q->Next = p;
				q = p;
			}
			Tail = q;
		}
	}
	SingleList(const SingleList& rhs)
	{
		operator=(rhs);
	}
	SingleList& operator=(const SingleList& rhs)
	{
		Clear();

		if (!rhs.Head)
		{
			return *this;
		}

		Head = new Node(rhs.Head->Data);
		Iterator thisIt = Head;		
		Iterator p = rhs.Head->Next;
		while (p)
		{
			thisIt->Next = new Node(p->Data);
			thisIt = thisIt->Next;
			p = p->Next;
		} 

		Tail = thisIt;

		return *this;
	}
	virtual ~SingleList()
	{
		Clear();
	}

	/*
	 * @brief	插入节点
	 * @param	Data 要插入的数据
	 * @retval	返回指向刚插入的节点的指针
	 */
	Iterator PushFront(const ValueType& Data)
	{
		Iterator p = Head;
		Head = new Node(Data);
		Head->Next = p;
		if (!Tail)
		{
			Tail = Head;
		}
		return Head;
	}
	Iterator PushBack(const ValueType& Data)
	{
		if (!Head)
		{
			Tail = Head = new Node(Data);
		}
		else
		{
			Tail->Next = new Node(Data);
			Tail = Tail->Next;
		}
		return Tail;
	}
	Iterator PushNewBack()
	{
		if (!Head) 
		{
			Tail = Head = new Node();
		} 
		else
		{
			Tail->Next = new Node();
			Tail = Tail->Next;
		}
		return Tail;
	}
	Iterator Insert(Iterator afterThis,const ValueType& Data)
	{
		Iterator p = new Node(Data,afterThis->Next);
		afterThis->Next = p;
		return p;
	}


	
	/*
	 * @brief	链表弹出操作，弹出Tail或者弹出Head指向的节点
	 */
	void PopBack()
	{
		if (!Head) return;
		if (Head == Tail)
		{
			delete Head;
			Head = Tail = NULL;
			return;
		}

		Iterator p = Head;
		while(p->Next != Tail)
		{
			p = p->Next;
		}
		
		delete Tail;
		Tail = p;
		Tail->Next = NULL;
	}
	void PopFront()
	{
		if (!Head) return;
		if (Head == Tail)
		{
			delete Head;
			Head = Tail = NULL;
			return;
		}

		Iterator p = Head;
		Head = Head->Next;
		delete p;
	}
	


	/*
	 * @brief	求链表长度
	 * @retval	返回链表的长度
	 * @notice	由于复杂度为O(n)，尽量不使用该方法
	 */
	size_t Length() const
	{
		size_t len = 0;
		for (Iterator p = Head; p; p = p->Next)
		{
			++len;
		}
		return len;
	}


	/*
	 * @brief	删除节点
	 * @param	afterThis 要删除的节点的前一个节点
	 */
	void RemoveAfter(Iterator afterThis)
	{
		if (afterThis && afterThis->Next)
		{
			Iterator del = afterThis->Next;
			afterThis->Next = del->Next;
			delete del;
		}
	}



	/*
	 * @brief	单向链表逆序
	 * @notice	Head和Tail所指向的节点互换（因为逆序了）
	 */
	void Reverse()
	{
		if (this->IsEmpty() || Head == Tail) return;

		Iterator p = Head, q = p->Next, r = q->Next;
		while (r) 
		{
			q->Next = p;
			p = q;
			q = r;
			r = r->Next;
		}
		q->Next = p;
		
		// swap Head and Tail
		Iterator temp = Head;
		Head = Tail;
		Tail = temp;
		Tail->Next = NULL;
	}



	/*
	 * @brief	清空整个链表，释放内存
	 * @notice	清空后Head和Tail指向NULL
	 */
	void Clear()
	{
		Iterator d = Head;
		while (d)
		{
			Head = Head->Next;
			delete d;
			d = Head;
		}
		Head = Tail = NULL;
	}


	// 向屏幕输出，测试用
	void ListOut() const
	{
#ifdef SCREEN_TRACEOUT
		Iterator p = Head;
		while (p)
		{
			cout<<p->Data<<" ";
			p = p->Next;
		}
		cout<<endl;
#endif
	}
};

// 循环链表数据结构
template <typename ValueType>
class CircularList
	: public List<ValueType>
{
public:

	// 数据结构说明与约定
	// 循环链表不空时，Tail->Next == Head 恒成立
	// 建立的时按照尾插法顺次排列


	typedef typename List<ValueType>::size_t 	size_t;
	typedef typename List<ValueType>::Node 		Node;
	typedef typename List<ValueType>::Iterator 	Iterator;

	using List<ValueType>::Head;
	using List<ValueType>::Tail;

	CircularList()
		: List<ValueType>()
	{ }
	CircularList(ValueType pData[],size_t count)
		: List<ValueType>()
	{
		// 用尾插法建立链表，顺次排列，最后封闭
		if (count > 0)
		{
			// 预先把Head确定好
			Head = new Node(pData[0]);

			Iterator q = Head;
			for (size_t i=1; i<count; ++i)
			{
				Iterator p = new Node(pData[i]);
				q->Next = p;
				q = p;
			}
			Tail = q;

			// 封闭链表
			Tail->Next = Head;
		}
	}
	CircularList(const CircularList& rhs)
	{
		operator=(rhs);
	}
	CircularList& operator=(const CircularList& rhs)
	{
		Clear();

		if (!rhs.Head)
		{
			return *this;
		}

		Head = new Node(rhs.Head->Data);
		Iterator thisIt = Head;
		Iterator first = rhs.Head;
		Iterator it = first->Next;
		while (it != first)
		{
			thisIt->Next = new Node(it->Data);
			thisIt = thisIt->Next;
			it = it->Next;
		} 

		// 尾节点闭合
		Tail = thisIt;
		Tail->Next = Head;

		return *this;
	}
	virtual ~CircularList()
	{
		Clear();
	}


	/*
	 * @brief	插入节点
	 * @param	Data 要插入的数据
	 * @retval	返回指向刚插入的节点的指针
	 * @notice	循环链表中PushBack和PushFront是等价的
	 */
	virtual Iterator PushBack(const ValueType& Data)
	{
		if (!Tail)
		{
			Head = Tail = new Node(Data);
			Tail->Next = Head;
		}
		else
		{
			Iterator p = new Node(Data,Head);
			Tail->Next = p;
			Tail = p;
		}
		return Tail;
	}
	virtual Iterator PushFront(const ValueType& Data)
	{
		return PushBack(Data);
	}
	virtual Iterator Insert(Iterator afterThis,const ValueType& Data)
	{
		Iterator p = new Node(Data,afterThis->Next);
		afterThis->Next = p;
		return p;
	}


	/*
	 * @brief	链表弹出操作，弹出Tail或者弹出Head指向的节点
	 * @notice	PopBack方法的时间复杂度为O(n)，尽量不使用
	 */
	virtual void PopBack()
	{
		if (!Head) return;
		if (Head == Tail) 
		{
			delete Head;
			Head = Tail = NULL;
			return;
		}

		Iterator p = Head;
		while(p->Next != Tail)
		{
			p = p->Next;
		}
		
		delete Tail;
		Tail = p;
		Tail->Next = Head;
	}
	virtual void PopFront()
	{
		if (!Head) return;
		if (Head == Tail) 
		{
			delete Head;
			Head = Tail = NULL;
			return;
		}

		Iterator p = Head;
		Head = Head->Next;
		delete p;
		Tail->Next = Head;
	}


	/*
	 * @brief	求链表长度
	 * @retval	返回链表的长度
	 * @notice	由于复杂度为O(n)，尽量不使用该方法
	 */
	virtual size_t Length() const
	{
		size_t len = 0;
		if (Head)
		{
			Iterator p = Head;
			do
			{
				++len;
				p = p->Next;
			} while (p != Head);
		}
		return len;
	}



	/*
	 * @brief	单向链表逆序
	 * @notice	Head和Tail所指向的节点互换（因为逆序了）
	 */
	void Reverse()
	{
		if (this->IsEmpty() || Head == Tail) return;

		Iterator p = Head, q = p->Next, r = q->Next;
		while (r != Head)
		{
			q->Next = p;
			p = q;
			q = r;
			r = r->Next;
		}
		q->Next = p;

		// swap Head and Tail
		Iterator temp = Head;
		Head = Tail;
		Tail = temp;
		Tail->Next = Head;
	}



	virtual void Clear()
	{
		if (Head)
		{
			Iterator d = Head;
			const Iterator h = Head;
			do
			{
				Head = Head->Next;
				delete d;
				d = Head;
			} while (d != h);

			Head = Tail = NULL;
		}
	}


	virtual void ListOut() const
	{
#ifdef SCREEN_TRACEOUT
		if (Head)
		{
			Iterator p = Head;
			do
			{
				cout<<p->Data<<" ";
				p = p->Next;
			} while (p != Head);
			cout<<endl;
		}
#endif
	}

};


#endif /* _LIST_H_ */
