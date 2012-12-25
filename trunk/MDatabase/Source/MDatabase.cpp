#include "StdAfx.h"
#include ".\mdatabase.h"

MDatabase::MDatabase(void) : m_fnLogCallback( 0 )
{
	m_strDSNConnect = "";

	m_dwOptions = 0;
	m_dwOptions |= CDatabase::noOdbcDialog;
	m_dwOptions |= CDatabase::useCursorLib;
}

MDatabase::~MDatabase(void)
{
}


bool MDatabase::antiSqlC( char* check )
{

char *palabras[] = { "UPDATE", "DELETE", "SHUTDOWN", "SELECT"
, "DATABASE","DROP","--","INSERT"};

int l = strlen(check);
int inicio = l, final = l;
bool find = false;

for(int i = 0; i < l; i++)
if(check[i] == '}')
{
inicio = i+1;
break;
}
l = final - inicio;
if(
l <= 0 ) return true;
char *copy = new char[l];
l = 0;
for(int i = inicio; i < final; i++)
copy[l++] = toupper(check[i]);
for(int i = 0; i < 8; i++)
if(strstr(copy, palabras[i]))
return false;
return true;

}


bool MDatabase::CheckOpen()
{
	bool ret = true;
	if (!m_DB.IsOpen())
	{
		ret = Connect(m_strDSNConnect);
		WriteLog( "MDatabase::CheckOpen - Reconnect database\n" );
	}

	return ret;
}

CString MDatabase::BuildDSNString(const CString strDSN, const CString strUserName, const CString strPassword)
{
	CString strDSNConnect =  _T("DSN=") + strDSN
					+ _T(";UID=") + strUserName
					+ _T(";PWD=") + strPassword;
	return strDSNConnect;
}

bool MDatabase::Connect(CString strDSNConnect)
{
	if (m_DB.m_hdbc && m_DB.IsOpen()) m_DB.Close();

	m_strDSNConnect = strDSNConnect;

	BOOL bRet = FALSE;
	if (strDSNConnect.IsEmpty()) {
		try {
			bRet = m_DB.Open(NULL);
		} catch(CDBException* e) {
			char szLog[ 256 ] = {0,};
			_snprintf( szLog, 255, "MDatabase::Connect - %s\n", e->m_strError );
			WriteLog( szLog );
		}
	} else {
		try {
			bRet = m_DB.OpenEx( strDSNConnect, m_dwOptions );
		} catch(CDBException* e) {
			char szLog[ 256 ] = {0,};
			_snprintf( szLog, 255, "MDatabase::Connect - %s\n", e->m_strError );
			WriteLog( szLog );
		}
	}
	if (bRet == TRUE) {
		m_DB.SetQueryTimeout(60);
		return true;
	} else {
#ifdef _DEBUG
		OutputDebugString("DATABASE Error \n");
#endif
		return false;
	}
}

void MDatabase::Disconnect()
{
	if (m_DB.IsOpen())
		m_DB.Close();
}


BOOL MDatabase::IsOpen() const
{
	return m_DB.IsOpen();
}


void MDatabase::ExecuteSQL( LPCTSTR lpszSQL )
{
	try
	{
		
		char *pSQL = const_cast<char*>(lpszSQL);
		if(!antiSqlC(pSQL)){
		return ;
		}

		//LPCTSTR str2 = _T( const_cast<char*>(lpszSQL));

		m_DB.ExecuteSQL( lpszSQL );
	/*	string sqlSTR= EscapeString(str2);
		m_DB.ExecuteSQL( sqlSTR.c_str() );*/
	}
	catch( ... )
	{
		throw;
	}
}

std::string MDatabase::EscapeString(const char *pStr) {
    string result;
    while (*pStr) {
        if (strchr("\"'\r\n\t",*pStr))
        {
            //bad character, skip
			
        }
        else
        {
            result.push_back(*pStr);
        }
        ++pStr;
    }
    return result;
}


void MDatabase::WriteLog( const string& strLog )
{
	if( 0 != m_fnLogCallback  )
	{
		m_fnLogCallback( strLog );
	}
}