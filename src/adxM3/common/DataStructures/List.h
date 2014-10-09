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
 * �������ݽṹ����
 */ 
template <typename ValueType>
class List
{
public:

	// ���ݽṹ˵����Լ��
	// �������ʱ��Head��Tail��ΪNULL�����򶼲���ΪNULL
	// ����ֻ��һ���ڵ�ʱ��Head��Tail��ָ��ýڵ�
	// �����ж���ڵ�ʱ��Headָ���һ���ڵ㣬Tailָ�����һ���ڵ�
	// ����ʱ�Ľڵ����˳��û���ϸ�涨���ɸ�������������ʵ��


	// ����������
	typedef UINT16 size_t;

	// �ڵ�����
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

	// ���������Ͷ���
	typedef Node* Iterator;

	// ���캯������������
	List()
		: Head(NULL), Tail(NULL)
	{ }
	virtual ~List()
	{
		// ��������������ʵ��
	}
	Iterator Head, Tail;


	/*
	 * @brief	���������Ƿ�Ϊ��
	 * @retval	���򷵻�true
	 */
	bool IsEmpty() const
	{
		return Head == NULL;
	}


	/*
	 * @brief	���������Ƿ�ֻ��һ��Ԫ��
	 */
	bool OnlyOneElement() const
	{
		return Head == Tail;
	}


	/*
	 * @brief	����ͷβָ�루һ���ò�����
	 */
	void _SwapHeadAndTail()
	{
		Iterator temp = Head;
		Head = Tail;
		Tail = temp;
	}


	/*
	 * @brief	�ڵ㶨λ
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
	 * @brief	��������
	 * @retval	��������ĳ���
	 */
	virtual size_t Length() const = 0;


	/*
	 * @brief	��������������ĩβ���������ĳ�ڵ�����
	 * @retval	ָ��ղ���Ľڵ��ָ��
	 */
	virtual Iterator PushBack(const ValueType& Data) = 0;
	virtual Iterator PushFront(const ValueType& Data) = 0;
	virtual Iterator Insert(Iterator afterThis,const ValueType& Data) = 0;


	/*
	 * @brief	����������������Tail���ߵ���Headָ��Ľڵ�
	 * @notice	PopBack������ʱ�临�Ӷ�ΪO(n)��������ʹ��
	 */
	virtual void PopBack() = 0;
	virtual void PopFront() = 0;



	/*
	 * @brief	������������ͷ��ڴ�
	 */
	virtual void Clear() = 0;



	// �������������
	virtual void ListOut() const = 0;
};


// ���������ݽṹ
template <typename ValueType>
class SingleList
	: public List<ValueType>
{
public:

	// ���ݽṹ˵����Լ��
	// Tail->Next == NULL �����
	// ������ʱ����β�巨˳������


	// �����õ�������������������Ҫ����˵��һ��
	typedef typename List<ValueType>::size_t 	size_t;
	typedef typename List<ValueType>::Node 		Node;
	typedef typename List<ValueType>::Iterator 	Iterator;

	// �������ݳ�Ա��ģ����������ҲҪ����˵��һ��
	using List<ValueType>::Head;
	using List<ValueType>::Tail;

	SingleList()
		: List<ValueType>()
	{ }
	SingleList(ValueType pData[],size_t count)
		: List<ValueType>()
	{
		// ��β�巨��������˳������
		if (count > 0)
		{
			// Ԥ�Ȱ�Headȷ����
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
	 * @brief	����ڵ�
	 * @param	Data Ҫ���������
	 * @retval	����ָ��ղ���Ľڵ��ָ��
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
	 * @brief	����������������Tail���ߵ���Headָ��Ľڵ�
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
	 * @brief	��������
	 * @retval	��������ĳ���
	 * @notice	���ڸ��Ӷ�ΪO(n)��������ʹ�ø÷���
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
	 * @brief	ɾ���ڵ�
	 * @param	afterThis Ҫɾ���Ľڵ��ǰһ���ڵ�
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
	 * @brief	������������
	 * @notice	Head��Tail��ָ��Ľڵ㻥������Ϊ�����ˣ�
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
	 * @brief	������������ͷ��ڴ�
	 * @notice	��պ�Head��Tailָ��NULL
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


	// ����Ļ�����������
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

// ѭ���������ݽṹ
template <typename ValueType>
class CircularList
	: public List<ValueType>
{
public:

	// ���ݽṹ˵����Լ��
	// ѭ��������ʱ��Tail->Next == Head �����
	// ������ʱ����β�巨˳������


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
		// ��β�巨��������˳�����У������
		if (count > 0)
		{
			// Ԥ�Ȱ�Headȷ����
			Head = new Node(pData[0]);

			Iterator q = Head;
			for (size_t i=1; i<count; ++i)
			{
				Iterator p = new Node(pData[i]);
				q->Next = p;
				q = p;
			}
			Tail = q;

			// �������
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

		// β�ڵ�պ�
		Tail = thisIt;
		Tail->Next = Head;

		return *this;
	}
	virtual ~CircularList()
	{
		Clear();
	}


	/*
	 * @brief	����ڵ�
	 * @param	Data Ҫ���������
	 * @retval	����ָ��ղ���Ľڵ��ָ��
	 * @notice	ѭ��������PushBack��PushFront�ǵȼ۵�
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
	 * @brief	����������������Tail���ߵ���Headָ��Ľڵ�
	 * @notice	PopBack������ʱ�临�Ӷ�ΪO(n)��������ʹ��
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
	 * @brief	��������
	 * @retval	��������ĳ���
	 * @notice	���ڸ��Ӷ�ΪO(n)��������ʹ�ø÷���
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
	 * @brief	������������
	 * @notice	Head��Tail��ָ��Ľڵ㻥������Ϊ�����ˣ�
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
