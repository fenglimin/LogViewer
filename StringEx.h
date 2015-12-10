//StringEx.h

#ifndef _STRINGEX_H_
#define _STRINGEX_H_

#define	CSTRINGEX_TYPE_STRING	0
#define	CSTRINGEX_TYPE_INT		1
#define	CSTRINGEX_TYPE_LONG		2
#define CSTRINGEX_TYPE_FLOAT	3
#define CSTRINGEX_TYPE_DOUBLE	4


class CStringEx : public CString 
{
public:
	CStringEx() : CString()
	{ 
		m_nType = CSTRINGEX_TYPE_STRING;
	}

	CStringEx( const CString& stringsrc) : CString( stringsrc )
	{
		m_nType = CSTRINGEX_TYPE_STRING;
	}

	CStringEx( const CStringEx& stringsrc) : CString( stringsrc )
	{
		m_nType = CSTRINGEX_TYPE_STRING;
	}

	CStringEx( TCHAR ch, int nrepeat = 1 ) : CString( ch, nrepeat )
	{
		m_nType = CSTRINGEX_TYPE_STRING;
	}

	CStringEx( LPCTSTR lpch, int nlength ) : CString( lpch, nlength )
	{
		m_nType = CSTRINGEX_TYPE_STRING;
	}

	CStringEx( const unsigned char* psz ) : CString( psz )
	{
		m_nType = CSTRINGEX_TYPE_STRING;
	}

	CStringEx( LPCWSTR lpsz ) : CString( lpsz )
	{
		m_nType = CSTRINGEX_TYPE_STRING;
	}

	CStringEx( LPCSTR lpsz ) : CString( lpsz )
	{
		m_nType = CSTRINGEX_TYPE_STRING;
	}

	CStringEx( int IntValue ) : CString()
	{ 	
		m_nType = CSTRINGEX_TYPE_INT;
		m_nIntValue = IntValue;
		Format ( "%d", IntValue );
	};

	CStringEx( float FloatValue ) : CString()
	{ 	
		m_nType = CSTRINGEX_TYPE_FLOAT;
		m_fFloatValue = FloatValue;
		Format ( "%f", FloatValue );
	};

	CStringEx( double DoubleValue ) : CString()
	{ 	
		m_nType = CSTRINGEX_TYPE_DOUBLE;
		m_dDoubleValue = DoubleValue;
		Format ( "%u", DoubleValue );
	};

	CStringEx( long LongValue ) : CString()
	{ 	
		m_nType = CSTRINGEX_TYPE_LONG;
		m_lLongValue = LongValue;
		Format ( "%ld", LongValue );
	};

public:
	CStringEx GetNextField ( LPCTSTR Delim );
	void StartTraverse();
	int GetFieldCount ( LPCTSTR Delim );
	int GetDelimitedFieldCount ( LPCTSTR DelimStart, LPCTSTR DelimEnd );
	CStringEx EraseBlank ( BOOL bEraseHead = TRUE, BOOL bEraseTail = TRUE );
	CStringEx	EraseRight ( int nCount );
	CStringEx	EraseLeft ( int nCount );
	CStringEx	DeleteAllString ( CString strToBeDelete );
	CStringEx	GetDelimitedField ( LPCTSTR DelimStart, LPCTSTR DelimEnd, int FieldNum );
	CStringEx	GetField ( LPCTSTR Delim, int FieldNum);

	float		AsFloat();
	int			AsInt();
	double		AsDouble();
	long		AsLong();
	char*		AsCharArray();

	int			m_nType;
	int			m_nIntValue;
	long		m_lLongValue;
	float		m_fFloatValue;
	double		m_dDoubleValue;

	LPTSTR		m_lpszTraverseRemainder;
};

#endif