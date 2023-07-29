//StringEx.cpp

#include "Stdafx.h"
#include "StringEx.h"

char* CStringEx::AsCharArray()
{
	return GetBuffer(0);		
}

int CStringEx::AsInt()
{
	if ( m_nType == CSTRINGEX_TYPE_INT )
		return m_nIntValue;
	if ( m_nType == CSTRINGEX_TYPE_LONG )
		return (int)m_lLongValue;
	if ( m_nType == CSTRINGEX_TYPE_FLOAT )
		return (int)m_fFloatValue;
	if ( m_nType == CSTRINGEX_TYPE_DOUBLE )
		return (int)m_dDoubleValue;

	return atoi ( GetBuffer(0) );
}

float CStringEx::AsFloat()
{
	if ( m_nType == CSTRINGEX_TYPE_INT )
		return (float)m_nIntValue;
	if ( m_nType == CSTRINGEX_TYPE_LONG )
		return (float)m_lLongValue;
	if ( m_nType == CSTRINGEX_TYPE_FLOAT )
		return m_fFloatValue;
	if ( m_nType == CSTRINGEX_TYPE_DOUBLE )
		return (float)m_dDoubleValue;

	return (float) atof ( GetBuffer(0) );
}

long CStringEx::AsLong()
{
	if ( m_nType == CSTRINGEX_TYPE_INT )
		return (long)m_nIntValue;
	if ( m_nType == CSTRINGEX_TYPE_LONG )
		return m_lLongValue;
	if ( m_nType == CSTRINGEX_TYPE_FLOAT )
		return (long)m_fFloatValue;
	if ( m_nType == CSTRINGEX_TYPE_DOUBLE )
		return (long)m_dDoubleValue;
	
	return atol ( GetBuffer(0) );
}

double CStringEx::AsDouble()
{
	if ( m_nType == CSTRINGEX_TYPE_INT )
		return (double)m_nIntValue;
	if ( m_nType == CSTRINGEX_TYPE_LONG )
		return (double)m_lLongValue;
	if ( m_nType == CSTRINGEX_TYPE_FLOAT )
		return (double)m_fFloatValue;
	if ( m_nType == CSTRINGEX_TYPE_DOUBLE )
		return m_dDoubleValue;

	return atof ( GetBuffer(0) );
}

// CStringEx strTest = "aaa,bbb,ddddd,";
// CStringEx strRet = strText.GetField ( ",", 1 );
// strRet is "bbb"
CStringEx CStringEx::GetField(LPCTSTR Delim, int FieldNum)
{
	if ( FieldNum < 0 )
		return GetField ( Delim, GetFieldCount(Delim)+1+FieldNum );

	LPTSTR lpsz, lpszRemainder = GetBuffer(0), lpszRet;
	int RetLen, LenDelim = lstrlen( Delim );

	while( FieldNum-- >= 0 )
	{
		lpszRet = lpszRemainder;
		lpsz = _tcsstr ( lpszRemainder, Delim);
		if ( lpsz )
		{
			// we did find the delim
			RetLen = lpsz - lpszRemainder;
			lpszRemainder = lpsz + LenDelim;
		}
		else
		{
			RetLen = lstrlen( lpszRemainder );
			lpszRemainder += RetLen;	// change to empty string
		}
	}
	return Mid( lpszRet - GetBuffer(0), RetLen );

}


// CStringEx strTest = "aaa(12) bbb(24) ddddd";
// CStringEx strRet = strText.GetDelimitedField ( "(", ")", 1 );
// strRet is "24"
CStringEx CStringEx::GetDelimitedField(LPCTSTR DelimStart, LPCTSTR DelimEnd, int FieldNum)
{
	LPTSTR lpsz, lpszEnd, lpszRet, lpszRemainder = GetBuffer(0) ;
	int LenDelimStart = lstrlen( DelimStart ), LenDelimEnd = lstrlen( DelimEnd );

	while( FieldNum-- >= 0 )
	{
		lpsz = _tcsstr ( lpszRemainder, DelimStart);
		if ( lpsz )
		{
			// we did find the start delim
			lpszRet = lpszRemainder = lpsz + LenDelimStart;
			lpszEnd = _tcsstr ( lpszRemainder, DelimEnd);
			if ( lpszEnd == NULL ) 
				return "";
			lpszRemainder = lpsz + LenDelimEnd;
		}
		else return "";
	}
	return Mid( lpszRet - GetBuffer(0), lpszEnd - lpszRet );

}

// CStringEx strTest = "aaa(12)bbb(24)bbbddddd";
// CStringEx strRet = strText.DeleteAllString ( "bbb" );
// strRet is "aaa(12)(24)ddddd"
CStringEx CStringEx::DeleteAllString(CString strToBeDelete)
{
	CString strRet;

	int i = 0;
	CString strTemp;

	do
	{
		strTemp = GetField(strToBeDelete, i++ );
		strRet += strTemp;
	}while ( strTemp != "" );

	return strRet;
}

// CStringEx strTest = "aaa(12)bbb(24)bbbddddd";
// CStringEx strRet = strText.EraseLeft ( 4 );
// strRet is "12)(24)ddddd"
CStringEx CStringEx::EraseLeft(int nCount)
{
	return Right ( GetLength() - nCount );	
}

// CStringEx strTest = "aaa(12)bbb(24)bbbddddd";
// CStringEx strRet = strText.EraseRight ( 4 );
// strRet is "1aaa(12)bbb(24)bbbd"
CStringEx CStringEx::EraseRight(int nCount)
{
	return Left ( GetLength() - nCount );
}

// CStringEx strTest = "   \taaa(12)bbb(24)bbbddddd\t   ";
// CStringEx strRet = strText.EraseBlank();
// strRet is "1aaa(12)bbb(24)bbbd"
CStringEx CStringEx::EraseBlank(BOOL bEraseHead, BOOL bEraseTail)
{
	int nLength = GetLength();
	int nStart = 0, nEnd = nLength-1;

	if ( bEraseHead )
		for ( ; nStart < nLength; nStart ++ )
		{
			char ch = GetAt ( nStart );
			if ( ch != ' ' && ch != '\t' && ch != 13 && ch != 10 )
				break;
		}

	if ( bEraseTail )
		for ( ; nEnd >= 0; nEnd -- )
		{
			char ch = GetAt ( nEnd );
			if ( ch != ' ' && ch != '\t' && ch != 13 && ch != 10 )
				break;
		}

	return Mid ( nStart, nEnd - nStart + 1 );
}

// CStringEx strTest = "aaa(12)bbb(24)bbbddddd";
// int nCount = strText.GetDelimitedFieldCount ( "(", ")" );
// nCount is 2
int CStringEx::GetDelimitedFieldCount(LPCTSTR DelimStart, LPCTSTR DelimEnd)
{
	LPTSTR lpsz, lpszEnd, lpszRemainder = GetBuffer(0) ;
	int LenDelimStart = lstrlen( DelimStart ), LenDelimEnd = lstrlen( DelimEnd );
	int nCount = 0;

	do
	{
		lpsz = _tcsstr ( lpszRemainder, DelimStart);
		if ( lpsz )
		{
			lpszRemainder = lpsz + LenDelimStart;
			lpszEnd = _tcsstr ( lpszRemainder, DelimEnd);
			if ( lpszEnd != NULL ) 
			{
				lpszRemainder = lpszEnd + LenDelimEnd;
				nCount ++;
			}
		}
	} while (lpsz && lpszEnd);

	return nCount;
}

int CStringEx::GetFieldCount(LPCTSTR Delim)
{
	LPTSTR lpsz = NULL, lpszRemainder = GetBuffer(0) ;
	int LenDelim = lstrlen( Delim );
	int nCount = 0;

	do
	{
		lpsz = _tcsstr ( lpszRemainder, Delim);
		if ( lpsz )
		{
			lpszRemainder = lpsz + LenDelim;
			nCount ++;
		}
	} while (lpsz);
	
	return nCount;
}

void CStringEx::StartTraverse()
{
	m_lpszTraverseRemainder = GetBuffer(0);

}

CStringEx CStringEx::GetNextField(LPCTSTR Delim)
{
	int nLenDelim = lstrlen( Delim );
	LPTSTR lpsz = _tcsstr ( m_lpszTraverseRemainder, Delim);
	int nRetLen = lpsz - m_lpszTraverseRemainder;

	CStringEx strRet = Mid ( m_lpszTraverseRemainder - GetBuffer(0), nRetLen );

	m_lpszTraverseRemainder = lpsz + nLenDelim;

	return strRet;
}
