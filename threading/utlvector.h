//====== Copyright � 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: 
//
// $NoKeywords: $
//
// A growable array class that maintains a free list and keeps elements
// in the same location
//=============================================================================//

#ifndef UTLVECTOR_H
#define UTLVECTOR_H

#ifdef _WIN32
#pragma once
#endif

#define  assert( _exp )										((void)0)
#define MEM_ALLOC_CREDIT_CLASS()

#include <string.h>
#include "platform.h"
//#include "tier0/dbg.h"
#include "threadtools.h"
#include "utlmemory.h"
#include "utlblockmemory.h"
#include <malloc.h>
#include "strtools.h"

#define FOR_EACH_VEC( vecName, iteratorName ) \
	for ( int iteratorName = 0; iteratorName < (vecName).Count(); iteratorName++ )
#define FOR_EACH_VEC_BACK( vecName, iteratorName ) \
	for ( int iteratorName = (vecName).Count()-1; iteratorName >= 0; iteratorName-- )

//-----------------------------------------------------------------------------
// The c_utl_vector class:
// A growable array class which doubles in size by default.
// It will always keep all elements consecutive in memory, and may move the
// elements around in memory (via a PvRealloc) when elements are inserted or
// removed. Clients should therefore refer to the elements of the vector
// by index (they should *never* maintain pointers to elements in the vector).
//-----------------------------------------------------------------------------
template< class T, class A = c_utl_memory<T> >
class c_utl_vector
{
	typedef A CAllocator;
public:
	typedef T ElemType_t;

	// constructor, destructor
	c_utl_vector( int growSize = 0, int initSize = 0 );
	c_utl_vector( T* pMemory, int allocationCount, int numElements = 0 );
	~c_utl_vector( );

	// Copy the array.
	c_utl_vector<T, A>& operator=( const c_utl_vector<T, A>& other );

	// element access
	T& operator[]( int i );
	const T& operator[]( int i ) const;
	T& Element( int i );
	const T& Element( int i ) const;
	T& Head( );
	const T& Head( ) const;
	T& Tail( );
	const T& Tail( ) const;

	// Gets the base address (can change when adding elements!)
	T* Base( ) { return m_Memory.Base( ); }
	const T* Base( ) const { return m_Memory.Base( ); }

	// Returns the number of elements in the vector
	int Count( ) const;

	// Is element index valid?
	bool IsValidIndex( int i ) const;
	static int InvalidIndex( );

	// Adds an element, uses default constructor
	int AddToHead( );
	int AddToTail( );
	int InsertBefore( int elem );
	int InsertAfter( int elem );

	// Adds an element, uses copy constructor
	int AddToHead( const T& src );
	int AddToTail( const T& src );
	int InsertBefore( int elem, const T& src );
	int InsertAfter( int elem, const T& src );

	// Adds multiple elements, uses default constructor
	int AddMultipleToHead( int num );
	int AddMultipleToTail( int num );
	int AddMultipleToTail( int num, const T* pToCopy );
	int InsertMultipleBefore( int elem, int num );
	int InsertMultipleBefore( int elem, int num, const T* pToCopy );
	int InsertMultipleAfter( int elem, int num );

	// Calls RemoveAll() then AddMultipleToTail.
	void SetSize( int size );
	void SetSizeDirect( int size ); //DYLAN ADDED DUE TO NOV 18, 2019 UPDATE, DO NOT USE THIS UNDER ANY CIRCUMSTANCE
	void SetCount( int count );
	void SetCountNondestructively( int count ); //sets count by adding or removing elements to tail TODO: This should probably be the default behavior for SetCount

											  // Calls SetSize and copies each element.
	void CopyArray( const T* pArray, int size );

	// Fast swap
	void Swap( c_utl_vector< T, A >& vec );

	// Add the specified array to the tail.
	int AddVectorToTail( c_utl_vector<T, A> const& src );

	// Finds an element (element needs operator== defined)
	int Find( const T& src ) const;
	void FillWithValue( const T& src );

	bool HasElement( const T& src ) const;

	// Makes sure we have enough memory allocated to store a requested # of elements
	void EnsureCapacity( int num );

	// Makes sure we have at least this many elements
	void EnsureCount( int num );

	// Element removal
	void FastRemove( int elem );	// doesn't preserve order
	void Remove( int elem );		// preserves order, shifts elements
	bool FindAndRemove( const T& src );	// removes first occurrence of src, preserves order, shifts elements
	bool FindAndFastRemove( const T& src );	// removes first occurrence of src, doesn't preserve order
	void RemoveMultiple( int elem, int num );	// preserves order, shifts elements
	void RemoveMultipleFromHead( int num ); // removes num elements from tail
	void RemoveMultipleFromTail( int num ); // removes num elements from tail
	void RemoveAll( );				// doesn't deallocate memory

									// Memory deallocation
	void Purge( );

	// Purges the list and calls delete on each element in it.
	void PurgeAndDeleteElements( );

	// Compacts the vector to the number of elements actually in use 
	void Compact( );

	// Set the size by which it grows when it needs to allocate more memory.
	void SetGrowSize( int size ) { m_Memory.SetGrowSize( size ); }

	int NumAllocated( ) const;	// Only use this if you really know what you're doing!

	void Sort( int( __cdecl* pfnCompare )( const T*, const T* ) );

#ifdef DBGFLAG_VALIDATE
	void Validate( CValidator& validator, char* pchName );		// Validate our internal structures
#endif // DBGFLAG_VALIDATE

protected:
	// Can't copy this unless we explicitly do it!
	c_utl_vector( c_utl_vector const& vec ) { /*assert(0);*/ }

	// Grows the vector
	void GrowVector( int num = 1 );

	// Shifts elements....
	void ShiftElementsRight( int elem, int num = 1 );
	void ShiftElementsLeft( int elem, int num = 1 );

	CAllocator m_Memory;
	int m_Size;

#ifndef _X360
	// For easier access to the elements through the debugger
	// it's in release builds so this can be used in libraries correctly
	T* m_pElements;

	inline void ResetDbgInfo( )
	{
		m_pElements = Base( );
	}
#else
	inline void ResetDbgInfo( ) {}
#endif
};


// this is kind of ugly, but until C++ gets templatized typedefs in C++0x, it's our only choice
template < class T >
class CUtlBlockVector : public c_utl_vector< T, CUtlBlockMemory< T, int > >
{
public:
	CUtlBlockVector( int growSize = 0, int initSize = 0 )
		: c_utl_vector< T, CUtlBlockMemory< T, int > >( growSize, initSize ) {}
};

//-----------------------------------------------------------------------------
// The c_utl_vectorMT class:
// A array class with some sort of mutex protection. Not sure which operations are protected from
// which others.
//-----------------------------------------------------------------------------

template< class BASE_UTLVECTOR, class MUTEX_TYPE = CThreadFastMutex >
class c_utl_vectorMT : public BASE_UTLVECTOR, public MUTEX_TYPE
{
	typedef BASE_UTLVECTOR BaseClass;
public:
	MUTEX_TYPE Mutex_t;

	// constructor, destructor
	c_utl_vectorMT( int growSize = 0, int initSize = 0 ) : BaseClass( growSize, initSize ) {}
	c_utl_vectorMT( typename BaseClass::ElemType_t* pMemory, int numElements ) : BaseClass( pMemory, numElements ) {}
};


//-----------------------------------------------------------------------------
// The c_utl_vectorFixed class:
// A array class with a fixed allocation scheme
//-----------------------------------------------------------------------------
template< class T, size_t MAX_SIZE >
class c_utl_vectorFixed : public c_utl_vector< T, c_utl_memoryFixed<T, MAX_SIZE > >
{
	typedef c_utl_vector< T, c_utl_memoryFixed<T, MAX_SIZE > > BaseClass;
public:

	// constructor, destructor
	c_utl_vectorFixed( int growSize = 0, int initSize = 0 ) : BaseClass( growSize, initSize ) {}
	c_utl_vectorFixed( T* pMemory, int numElements ) : BaseClass( pMemory, numElements ) {}
};


//-----------------------------------------------------------------------------
// The c_utl_vectorFixedGrowable class:
// A array class with a fixed allocation scheme backed by a dynamic one
//-----------------------------------------------------------------------------
template< class T, size_t MAX_SIZE >
class c_utl_vectorFixedGrowable : public c_utl_vector< T, c_utl_memoryFixedGrowable<T, MAX_SIZE > >
{
	typedef c_utl_vector< T, c_utl_memoryFixedGrowable<T, MAX_SIZE > > BaseClass;

public:
	// constructor, destructor
	c_utl_vectorFixedGrowable( int growSize = 0 ) : BaseClass( growSize, MAX_SIZE ) {}
};


//-----------------------------------------------------------------------------
// The c_utl_vectorConservative class:
// A array class with a conservative allocation scheme
//-----------------------------------------------------------------------------
template< class T >
class c_utl_vectorConservative : public c_utl_vector< T, c_utl_memoryConservative<T> >
{
	typedef c_utl_vector< T, c_utl_memoryConservative<T> > BaseClass;
public:

	// constructor, destructor
	c_utl_vectorConservative( int growSize = 0, int initSize = 0 ) : BaseClass( growSize, initSize ) {}
	c_utl_vectorConservative( T* pMemory, int numElements ) : BaseClass( pMemory, numElements ) {}
};


//-----------------------------------------------------------------------------
// The c_utl_vectorUltra Conservative class:
// A array class with a very conservative allocation scheme, with customizable allocator
// Especialy useful if you have a lot of vectors that are sparse, or if you're
// carefully packing holders of vectors
//-----------------------------------------------------------------------------
#pragma warning(push)
#pragma warning(disable : 4200) // warning C4200: nonstandard extension used : zero-sized array in struct/union
#pragma warning(disable : 4815 ) // warning C4815: 'staticData' : zero-sized array in stack object will have no elements

class c_utl_vectorUltraConservativeAllocator
{
public:
	static void* Alloc( size_t nSize )
	{
		return malloc( nSize );
	}

	static void* Realloc( void* pMem, size_t nSize )
	{
		return realloc( pMem, nSize );
	}

	static void Free( void* pMem )
	{
		free( pMem );
	}

	static size_t GetSize( void* pMem )
	{
		return mallocsize( pMem );
	}

};

template <typename T, typename A = c_utl_vectorUltraConservativeAllocator >
class c_utl_vectorUltraConservative : private A
{
public:
	c_utl_vectorUltraConservative( )
	{
		m_pData = StaticData( );
	}

	~c_utl_vectorUltraConservative( )
	{
		RemoveAll( );
	}

	int Count( ) const
	{
		return m_pData->m_Size;
	}

	static int InvalidIndex( )
	{
		return -1;
	}

	inline bool IsValidIndex( int i ) const
	{
		return ( i >= 0 ) && ( i < Count( ) );
	}

	T& operator[]( int i )
	{
		//assert(IsValidIndex(i));
		return m_pData->m_Elements[ i ];
	}

	const T& operator[]( int i ) const
	{
		//assert(IsValidIndex(i));
		return m_pData->m_Elements[ i ];
	}

	T& Element( int i )
	{
		//assert(IsValidIndex(i));
		return m_pData->m_Elements[ i ];
	}

	const T& Element( int i ) const
	{
		//assert(IsValidIndex(i));
		return m_pData->m_Elements[ i ];
	}

	void EnsureCapacity( int num )
	{
		int nCurCount = Count( );
		if ( num <= nCurCount )
		{
			return;
		}
		if ( m_pData == StaticData( ) )
		{
			m_pData = ( Data_t* )A::Alloc( sizeof( int ) + ( num * sizeof( T ) ) );
			m_pData->m_Size = 0;
		}
		else
		{
			int nNeeded = sizeof( int ) + ( num * sizeof( T ) );
			int nHave = A::GetSize( m_pData );
			if ( nNeeded > nHave )
			{
				m_pData = ( Data_t* )A::Realloc( m_pData, nNeeded );
			}
		}
	}

	int AddToTail( const T& src )
	{
		int iNew = Count( );
		EnsureCapacity( Count( ) + 1 );
		m_pData->m_Elements[ iNew ] = src;
		m_pData->m_Size++;
		return iNew;
	}

	void RemoveAll( )
	{
		if ( Count( ) )
		{
			for ( int i = m_pData->m_Size; --i >= 0; )
			{
				destruct( &m_pData->m_Elements[ i ] );
			}
		}
		if ( m_pData != StaticData( ) )
		{
			A::Free( m_pData );
			m_pData = StaticData( );

		}
	}

	void PurgeAndDeleteElements( )
	{
		if ( m_pData != StaticData( ) )
		{
			for ( int i = 0; i < m_pData->m_Size; i++ )
			{
				delete Element( i );
			}
			RemoveAll( );
		}
	}

	void FastRemove( int elem )
	{
		//assert(IsValidIndex(elem));

		destruct( &Element( elem ) );
		if ( Count( ) > 0 )
		{
			if ( elem != m_pData->m_Size - 1 )
				memcpy( &Element( elem ), &Element( m_pData->m_Size - 1 ), sizeof( T ) );
			--m_pData->m_Size;
		}
		if ( !m_pData->m_Size )
		{
			A::Free( m_pData );
			m_pData = StaticData( );
		}
	}

	void Remove( int elem )
	{
		destruct( &Element( elem ) );
		ShiftElementsLeft( elem );
		--m_pData->m_Size;
		if ( !m_pData->m_Size )
		{
			A::Free( m_pData );
			m_pData = StaticData( );
		}
	}

	int Find( const T& src ) const
	{
		int nCount = Count( );
		for ( int i = 0; i < nCount; ++i )
		{
			if ( Element( i ) == src )
				return i;
		}
		return -1;
	}

	bool FindAndRemove( const T& src )
	{
		int elem = Find( src );
		if ( elem != -1 )
		{
			Remove( elem );
			return true;
		}
		return false;
	}


	bool FindAndFastRemove( const T& src )
	{
		int elem = Find( src );
		if ( elem != -1 )
		{
			FastRemove( elem );
			return true;
		}
		return false;
	}

	struct Data_t
	{
		int m_Size;
		T m_Elements[ ];
	};

	Data_t* m_pData;
private:
	void ShiftElementsLeft( int elem, int num = 1 )
	{
		int Size = Count( );
		//assert(IsValidIndex(elem) || (Size == 0) || (num == 0));
		int numToMove = Size - elem - num;
		if ( ( numToMove > 0 ) && ( num > 0 ) )
		{
			memmove( &Element( elem ), &Element( elem + num ), numToMove * sizeof( T ) );

#ifdef _DEBUG
			memset( &Element( Size - num ), 0xDD, num * sizeof( T ) );
#endif
		}
	}



	static Data_t* StaticData( )
	{
		static Data_t staticData;
		//assert(staticData.m_Size == 0);
		return &staticData;
	}
};

#pragma warning(pop)


//-----------------------------------------------------------------------------
// The CCopyableUtlVector class:
// A array class that allows copy construction (so you can nest a c_utl_vector inside of another one of our containers)
//  WARNING - this class lets you copy construct which can be an expensive operation if you don't carefully control when it happens
// Only use this when nesting a c_utl_vector() inside of another one of our container classes (i.e a CUtlMap)
//-----------------------------------------------------------------------------
template< class T >
class CCopyableUtlVector : public c_utl_vector< T, c_utl_memory<T> >
{
	typedef c_utl_vector< T, c_utl_memory<T> > BaseClass;
public:
	CCopyableUtlVector( int growSize = 0, int initSize = 0 ) : BaseClass( growSize, initSize ) {}
	CCopyableUtlVector( T* pMemory, int numElements ) : BaseClass( pMemory, numElements ) {}
	virtual ~CCopyableUtlVector( ) {}
	CCopyableUtlVector( CCopyableUtlVector const& vec ) { CopyArray( vec.Base( ), vec.Count( ) ); }
};

// TODO (Ilya): It seems like all the functions in c_utl_vector are simple enough that they should be inlined.

//-----------------------------------------------------------------------------
// constructor, destructor
//-----------------------------------------------------------------------------
template< typename T, class A >
inline c_utl_vector<T, A>::c_utl_vector( int growSize, int initSize ) :
	m_Memory( growSize, initSize ), m_Size( 0 )
{
	ResetDbgInfo( );
}

template< typename T, class A >
inline c_utl_vector<T, A>::c_utl_vector( T* pMemory, int allocationCount, int numElements ) :
	m_Memory( pMemory, allocationCount ), m_Size( numElements )
{
	ResetDbgInfo( );
}

template< typename T, class A >
inline c_utl_vector<T, A>::~c_utl_vector( )
{
	Purge( );
}

template< typename T, class A >
inline c_utl_vector<T, A>& c_utl_vector<T, A>::operator=( const c_utl_vector<T, A>& other )
{
	int nCount = other.Count( );
	SetSize( nCount );
	for ( int i = 0; i < nCount; i++ )
	{
		( *this )[ i ] = other[ i ];
	}
	return *this;
}


//-----------------------------------------------------------------------------
// element access
//-----------------------------------------------------------------------------
template< typename T, class A >
inline T& c_utl_vector<T, A>::operator[]( int i )
{
	//assert(i < m_Size);
	return m_Memory[ i ];
}

template< typename T, class A >
inline const T& c_utl_vector<T, A>::operator[]( int i ) const
{
	//assert(i < m_Size);
	return m_Memory[ i ];
}

template< typename T, class A >
inline T& c_utl_vector<T, A>::Element( int i )
{
	//assert(i < m_Size);
	return m_Memory[ i ];
}

template< typename T, class A >
inline const T& c_utl_vector<T, A>::Element( int i ) const
{
	//assert(i < m_Size);
	return m_Memory[ i ];
}

template< typename T, class A >
inline T& c_utl_vector<T, A>::Head( )
{
	//assert(m_Size > 0);
	return m_Memory[ 0 ];
}

template< typename T, class A >
inline const T& c_utl_vector<T, A>::Head( ) const
{
	//assert(m_Size > 0);
	return m_Memory[ 0 ];
}

template< typename T, class A >
inline T& c_utl_vector<T, A>::Tail( )
{
	//assert(m_Size > 0);
	return m_Memory[ m_Size - 1 ];
}

template< typename T, class A >
inline const T& c_utl_vector<T, A>::Tail( ) const
{
	//assert(m_Size > 0);
	return m_Memory[ m_Size - 1 ];
}


//-----------------------------------------------------------------------------
// Count
//-----------------------------------------------------------------------------
template< typename T, class A >
inline int c_utl_vector<T, A>::Count( ) const
{
	return m_Size;
}


//-----------------------------------------------------------------------------
// Is element index valid?
//-----------------------------------------------------------------------------
template< typename T, class A >
inline bool c_utl_vector<T, A>::IsValidIndex( int i ) const
{
	return ( i >= 0 ) && ( i < m_Size );
}


//-----------------------------------------------------------------------------
// Returns in invalid index
//-----------------------------------------------------------------------------
template< typename T, class A >
inline int c_utl_vector<T, A>::InvalidIndex( )
{
	return -1;
}


//-----------------------------------------------------------------------------
// Grows the vector
//-----------------------------------------------------------------------------
template< typename T, class A >
void c_utl_vector<T, A>::GrowVector( int num )
{
	if ( m_Size + num > m_Memory.NumAllocated( ) )
	{
		MEM_ALLOC_CREDIT_CLASS( );
		m_Memory.Grow( m_Size + num - m_Memory.NumAllocated( ) );
	}

	m_Size += num;
	ResetDbgInfo( );
}


//-----------------------------------------------------------------------------
// Sorts the vector
//-----------------------------------------------------------------------------
template< typename T, class A >
void c_utl_vector<T, A>::Sort( int( __cdecl* pfnCompare )( const T*, const T* ) )
{
	typedef int( __cdecl* QSortCompareFunc_t )( const void*, const void* );
	if ( Count( ) <= 1 )
		return;

	if ( Base( ) )
	{
		qsort( Base( ), Count( ), sizeof( T ), ( QSortCompareFunc_t )( pfnCompare ) );
	}
	else
	{
		//assert(0);
		// this path is untested
		// if you want to sort vectors that use a non-sequential memory allocator,
		// you'll probably want to patch in a quicksort algorithm here
		// I just threw in this bubble sort to have something just in case...

		for ( int i = m_Size - 1; i >= 0; --i )
		{
			for ( int j = 1; j <= i; ++j )
			{
				if ( pfnCompare( &Element( j - 1 ), &Element( j ) ) < 0 )
				{
					V_swap( Element( j - 1 ), Element( j ) );
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Makes sure we have enough memory allocated to store a requested # of elements
//-----------------------------------------------------------------------------
template< typename T, class A >
void c_utl_vector<T, A>::EnsureCapacity( int num )
{
	MEM_ALLOC_CREDIT_CLASS( );
	m_Memory.EnsureCapacity( num );
	ResetDbgInfo( );
}


//-----------------------------------------------------------------------------
// Makes sure we have at least this many elements
//-----------------------------------------------------------------------------
template< typename T, class A >
void c_utl_vector<T, A>::EnsureCount( int num )
{
	if ( Count( ) < num )
	{
		AddMultipleToTail( num - Count( ) );
	}
}


//-----------------------------------------------------------------------------
// Shifts elements
//-----------------------------------------------------------------------------
template< typename T, class A >
void c_utl_vector<T, A>::ShiftElementsRight( int elem, int num )
{
	//assert(IsValidIndex(elem) || (m_Size == 0) || (num == 0));
	int numToMove = m_Size - elem - num;
	if ( ( numToMove > 0 ) && ( num > 0 ) )
		memmove( &Element( elem + num ), &Element( elem ), numToMove * sizeof( T ) );
}

template< typename T, class A >
void c_utl_vector<T, A>::ShiftElementsLeft( int elem, int num )
{
	//assert(IsValidIndex(elem) || (m_Size == 0) || (num == 0));
	int numToMove = m_Size - elem - num;
	if ( ( numToMove > 0 ) && ( num > 0 ) )
	{
		memmove( &Element( elem ), &Element( elem + num ), numToMove * sizeof( T ) );

#ifdef _DEBUG
		//memmove(&Element(m_Size - num), 0xDD, num * sizeof(T));
#endif
	}
}


//-----------------------------------------------------------------------------
// Adds an element, uses default constructor
//-----------------------------------------------------------------------------
template< typename T, class A >
inline int c_utl_vector<T, A>::AddToHead( )
{
	return InsertBefore( 0 );
}

template< typename T, class A >
inline int c_utl_vector<T, A>::AddToTail( )
{
	return InsertBefore( m_Size );
}

template< typename T, class A >
inline int c_utl_vector<T, A>::InsertAfter( int elem )
{
	return InsertBefore( elem + 1 );
}

template< typename T, class A >
int c_utl_vector<T, A>::InsertBefore( int elem )
{
	// Can insert at the end
	//assert((elem == Count()) || IsValidIndex(elem));

	GrowVector( );
	ShiftElementsRight( elem );
	construct( &Element( elem ) );
	return elem;
}


//-----------------------------------------------------------------------------
// Adds an element, uses copy constructor
//-----------------------------------------------------------------------------
template< typename T, class A >
inline int c_utl_vector<T, A>::AddToHead( const T& src )
{
	// Can't insert something that's in the list... reallocation may hose us
	//assert((Base() == NULL) || (&src < Base()) || (&src >= (Base() + Count())));
	return InsertBefore( 0, src );
}

template< typename T, class A >
inline int c_utl_vector<T, A>::AddToTail( const T& src )
{
	// Can't insert something that's in the list... reallocation may hose us
	//assert((Base() == NULL) || (&src < Base()) || (&src >= (Base() + Count())));
	return InsertBefore( m_Size, src );
}

template< typename T, class A >
inline int c_utl_vector<T, A>::InsertAfter( int elem, const T& src )
{
	// Can't insert something that's in the list... reallocation may hose us
	//assert((Base() == NULL) || (&src < Base()) || (&src >= (Base() + Count())));
	return InsertBefore( elem + 1, src );
}

template< typename T, class A >
int c_utl_vector<T, A>::InsertBefore( int elem, const T& src )
{
	// Can't insert something that's in the list... reallocation may hose us
	//assert((Base() == NULL) || (&src < Base()) || (&src >= (Base() + Count())));

	// Can insert at the end
	//assert((elem == Count()) || IsValidIndex(elem));

	GrowVector( );
	ShiftElementsRight( elem );
	Copyconstruct( &Element( elem ), src );
	return elem;
}


//-----------------------------------------------------------------------------
// Adds multiple elements, uses default constructor
//-----------------------------------------------------------------------------
template< typename T, class A >
inline int c_utl_vector<T, A>::AddMultipleToHead( int num )
{
	return InsertMultipleBefore( 0, num );
}

template< typename T, class A >
inline int c_utl_vector<T, A>::AddMultipleToTail( int num )
{
	return InsertMultipleBefore( m_Size, num );
}

template< typename T, class A >
inline int c_utl_vector<T, A>::AddMultipleToTail( int num, const T* pToCopy )
{
	// Can't insert something that's in the list... reallocation may hose us
	//assert((Base() == NULL) || !pToCopy || (pToCopy + num <= Base()) || (pToCopy >= (Base() + Count())));

	return InsertMultipleBefore( m_Size, num, pToCopy );
}

template< typename T, class A >
int c_utl_vector<T, A>::InsertMultipleAfter( int elem, int num )
{
	return InsertMultipleBefore( elem + 1, num );
}


template< typename T, class A >
void c_utl_vector<T, A>::SetCount( int count )
{
	RemoveAll( );
	AddMultipleToTail( count );
}

template< typename T, class A >
inline void c_utl_vector<T, A>::SetSize( int size )
{
	SetCount( size );
}

template< typename T, class A >
inline void c_utl_vector<T, A>::SetSizeDirect( int size )
{
	m_Size = 0;
}

template< typename T, class A >
void c_utl_vector<T, A>::SetCountNondestructively( int count )
{
	int delta = count - m_Size;
	if ( delta > 0 ) AddMultipleToTail( delta );
	else if ( delta < 0 ) RemoveMultipleFromTail( -delta );
}

template< typename T, class A >
void c_utl_vector<T, A>::CopyArray( const T* pArray, int size )
{
	// Can't insert something that's in the list... reallocation may hose us
	//assert((Base() == NULL) || !pArray || (Base() >= (pArray + size)) || (pArray >= (Base() + Count())));

	SetSize( size );
	for ( int i = 0; i < size; i++ )
	{
		( *this )[ i ] = pArray[ i ];
	}
}

template< typename T, class A >
void c_utl_vector<T, A>::Swap( c_utl_vector< T, A >& vec )
{
	m_Memory.Swap( vec.m_Memory );
	V_swap( m_Size, vec.m_Size );
#ifndef _X360
	V_swap( m_pElements, vec.m_pElements );
#endif
}

template< typename T, class A >
int c_utl_vector<T, A>::AddVectorToTail( c_utl_vector const& src )
{
	//assert(&src != this);

	int base = Count( );

	// Make space.
	int nSrcCount = src.Count( );
	EnsureCapacity( base + nSrcCount );

	// Copy the elements.	
	m_Size += nSrcCount;
	for ( int i = 0; i < nSrcCount; i++ )
	{
		Copyconstruct( &Element( base + i ), src[ i ] );
	}
	return base;
}

template< typename T, class A >
inline int c_utl_vector<T, A>::InsertMultipleBefore( int elem, int num )
{
	if ( num == 0 )
		return elem;

	// Can insert at the end
	//assert((elem == Count()) || IsValidIndex(elem));

	GrowVector( num );
	ShiftElementsRight( elem, num );

	// Invoke default constructors
	for ( int i = 0; i < num; ++i )
	{
		construct( &Element( elem + i ) );
	}

	return elem;
}

template< typename T, class A >
inline int c_utl_vector<T, A>::InsertMultipleBefore( int elem, int num, const T* pToInsert )
{
	if ( num == 0 )
		return elem;

	// Can insert at the end
	//assert((elem == Count()) || IsValidIndex(elem));

	GrowVector( num );
	ShiftElementsRight( elem, num );

	// Invoke default constructors
	if ( !pToInsert )
	{
		for ( int i = 0; i < num; ++i )
		{
			construct( &Element( elem + i ) );
		}
	}
	else
	{
		for ( int i = 0; i < num; i++ )
		{
			Copyconstruct( &Element( elem + i ), pToInsert[ i ] );
		}
	}

	return elem;
}


//-----------------------------------------------------------------------------
// Finds an element (element needs operator== defined)
//-----------------------------------------------------------------------------
template< typename T, class A >
int c_utl_vector<T, A>::Find( const T& src ) const
{
	for ( int i = 0; i < Count( ); ++i )
	{
		if ( Element( i ) == src )
			return i;
	}
	return -1;
}

template< typename T, class A >
void c_utl_vector<T, A>::FillWithValue( const T& src )
{
	for ( int i = 0; i < Count( ); i++ )
	{
		Element( i ) = src;
	}
}

template< typename T, class A >
bool c_utl_vector<T, A>::HasElement( const T& src ) const
{
	return ( Find( src ) >= 0 );
}


//-----------------------------------------------------------------------------
// Element removal
//-----------------------------------------------------------------------------
template< typename T, class A >
void c_utl_vector<T, A>::FastRemove( int elem )
{
	//assert(IsValidIndex(elem));

	destruct( &Element( elem ) );
	if ( m_Size > 0 )
	{
		if ( elem != m_Size - 1 )
			memcpy( &Element( elem ), &Element( m_Size - 1 ), sizeof( T ) );
		--m_Size;
	}
}

template< typename T, class A >
void c_utl_vector<T, A>::Remove( int elem )
{
	destruct( &Element( elem ) );
	ShiftElementsLeft( elem );
	--m_Size;
}

template< typename T, class A >
bool c_utl_vector<T, A>::FindAndRemove( const T& src )
{
	int elem = Find( src );
	if ( elem != -1 )
	{
		Remove( elem );
		return true;
	}
	return false;
}

template< typename T, class A >
bool c_utl_vector<T, A>::FindAndFastRemove( const T& src )
{
	int elem = Find( src );
	if ( elem != -1 )
	{
		FastRemove( elem );
		return true;
	}
	return false;
}

template< typename T, class A >
void c_utl_vector<T, A>::RemoveMultiple( int elem, int num )
{
	//assert(elem >= 0);
	//assert(elem + num <= Count());

	for ( int i = elem + num; --i >= elem; )
		destruct( &Element( i ) );

	ShiftElementsLeft( elem, num );
	m_Size -= num;
}

template< typename T, class A >
void c_utl_vector<T, A>::RemoveMultipleFromHead( int num )
{
	//assert(num <= Count());

	for ( int i = num; --i >= 0; )
		destruct( &Element( i ) );

	ShiftElementsLeft( 0, num );
	m_Size -= num;
}

template< typename T, class A >
void c_utl_vector<T, A>::RemoveMultipleFromTail( int num )
{
	//assert(num <= Count());

	for ( int i = m_Size - num; i < m_Size; i++ )
		destruct( &Element( i ) );

	m_Size -= num;
}

template< typename T, class A >
void c_utl_vector<T, A>::RemoveAll( )
{
	for ( int i = m_Size; --i >= 0; )
	{
		destruct( &Element( i ) );
	}

	m_Size = 0;
}


//-----------------------------------------------------------------------------
// Memory deallocation
//-----------------------------------------------------------------------------

template< typename T, class A >
inline void c_utl_vector<T, A>::Purge( )
{
	RemoveAll( );
	m_Memory.Purge( );
	ResetDbgInfo( );
}


template< typename T, class A >
inline void c_utl_vector<T, A>::PurgeAndDeleteElements( )
{
	for ( int i = 0; i < m_Size; i++ )
	{
		delete Element( i );
	}
	Purge( );
}

template< typename T, class A >
inline void c_utl_vector<T, A>::Compact( )
{
	m_Memory.Purge( m_Size );
}

template< typename T, class A >
inline int c_utl_vector<T, A>::NumAllocated( ) const
{
	return m_Memory.NumAllocated( );
}


//-----------------------------------------------------------------------------
// Data and memory validation
//-----------------------------------------------------------------------------
#ifdef DBGFLAG_VALIDATE
template< typename T, class A >
void c_utl_vector<T, A>::Validate( CValidator& validator, char* pchName )
{
	validator.Push( typeid( *this ).name( ), this, pchName );

	m_Memory.Validate( validator, "m_Memory" );

	validator.Pop( );
}
#endif // DBGFLAG_VALIDATE

// A vector class for storing pointers, so that the elements pointed to by the pointers are deleted
// on exit.
template<class T> class c_utl_vectorAutoPurge : public c_utl_vector< T, c_utl_memory< T, int> >
{
public:
	~c_utl_vectorAutoPurge( void )
	{
		this->PurgeAndDeleteElements( );
	}

};

// easy string list class with dynamically allocated strings. For use with V_SplitString, etc.
// Frees the dynamic strings in destructor.
class CUtlStringList : public c_utl_vectorAutoPurge< char*>
{
public:
	void CopyAndAddToTail( char const* pString )			// clone the string and add to the end
	{
		char* pNewStr = new char[ 1 + strlen( pString ) ];
		strcpy( pNewStr, pString );
		AddToTail( pNewStr );
	}

	static int __cdecl SortFunc( char* const* sz1, char* const* sz2 )
	{
		return strcmp( *sz1, *sz2 );
	}

};



// <Sergiy> placing it here a few days before Cert to minimize disruption to the rest of codebase
class CSplitString : public c_utl_vector<char*, c_utl_memory<char*, int> >
{
public:
	CSplitString( const char* pString, const char* pSeparator );
	CSplitString( const char* pString, const char** pSeparators, int nSeparators );
	~CSplitString( );
	//
	// NOTE: If you want to make construct() public and implement Purge() here, you'll have to free m_szBuffer there
	//
private:
	void construct( const char* pString, const char** pSeparators, int nSeparators );
	void PurgeAndDeleteElements( );
private:
	char* m_szBuffer; // a copy of original string, with '\0' instead of separators
};


#endif // CCVECTOR_H