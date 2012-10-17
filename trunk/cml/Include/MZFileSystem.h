#pragma once

#include <string>
#include <vector>
#include <list>
#include <map>
using namespace std;

#pragma warning( disable : 4251)


// File access mode
enum
{
	MFILEACCESS_GENERIC,							///< 일반 파일에 대한 접근 허용
	MFILEACCESS_ENCRYPTED							///< 암호화된 파일만 접근 허용
};


// Default properties of LZMA
static const unsigned char LZMA_PROPS[] = { 0x5D, 0x00, 0x00, 0x01, 0x00 };
void GetRefineFilename(char *szRefine, const char *szSource);




// MFileHeader
struct MFileHeader
{
	unsigned long			nFileSize;
	unsigned long			nSectorPos;
	unsigned long			nSectorSize;
	unsigned short			nMotherFileNameLen;
	unsigned short			nFileNameLen;
	unsigned short			nCRC;
};





/// struct : MFileInfo
struct MFileInfo
{
	bool					bExist;					///< Flag to exist file
	string					strMotherFileName;		///< Mother file name
	string					strFileName;			///< File name
	unsigned short			nCRC;					///< CRC
	size_t					nFileSize;				///< Size of file
	DWORD					nSectorPos;				///< Position of sector
	size_t					nSectorSize;			///< Size of sector
};





/// class : MCrypt
class MCrypt
{
protected:
	unsigned char*			m_pKeyTable;


public:
	MCrypt();
	virtual ~MCrypt();

	bool Create();
	bool Destroy();
	
	bool Encrypt( void* pBuff, DWORD nBuffLen);
	bool Decrypt( void* pBuff, DWORD nBuffLen);

	unsigned short GetCRC( void* pBuff, DWORD nBuffLen);

	bool Validate();
};





/// class : MFile
class MZFile
{
// Origin of file seek
public:
	enum MSEEKPOS			{ BEGIN = 0X00,  CURRENT = 0X01,  END = 0X02 };



// Member variables
protected:
	string					m_strFileName;			///< File name
	HANDLE					m_hFile;				///< Handle of file
	unsigned char*			m_pData;				///< Pointer of file data
	DWORD					m_nFilePos;				///< Position of file
	size_t					m_nFileSize;			///< Size of file
	DWORD					m_nSectorPos;			///< Position of sector
	size_t					m_nSectorSize;			///< Size of sector
	bool					m_bCompressed;			///< Flag of compressed file
	bool					m_bAllowAccUncompFile;	///< Flag to allow access uncompressed files



// Member functions
public:
	// Constructor/Destructor
	MZFile();
	virtual ~MZFile();


	// Open/Close file
	bool Open( const char* szFileName, bool bAllowAccessUncompFile =false);
	bool Close();

	// Get file size
	size_t GetLength() const;

	// Get file name
	const string& GetFileName() const;

	// Move file pointer
	DWORD Seek( long nOffset, int nOrigin);

	// Read file
	bool Read( void* pBuffer, size_t nBytesToRead, size_t* pnBytesRead =NULL);
	bool ReadAll( void* pBuffer, size_t nBufferSize, size_t* pnBytesRead =NULL);
	bool ReadBufferAll( char** pBuffer, size_t* pnBytesRead =NULL);
	bool ReadStringAll( char** pBuffer, size_t* pnBytesRead =NULL);
	bool ReadStringAll( string& strBuffer, size_t* pnBytesRead =NULL);
	void FileFindAndChangeState( const char* szInFileName, string& szOutFileName );

	// Allow access to uncompressed files
	void SetAllowAccessUncompFile( bool bAllow)	{ m_bAllowAccUncompFile = bAllow;	}
	bool IsAllowedAccessUncompFile() const		{ return m_bAllowAccUncompFile;		}
};





// Type of file index
typedef vector< string>						PATHINDEX;

// Type of file index
typedef multimap< string, MFileInfo*>		FILEINDEX;



static void ValidateFilename(char* szOut, const char* szSrc, char substitute)
{
	strcpy(szOut, szSrc);
	size_t len = strlen(szOut);
	for (size_t i=0; i<len; ++i)
	{
		if (szOut[i] == '<' ||
			szOut[i] == '>' ||
			szOut[i] == ':' ||
			szOut[i] == '\"' ||
			szOut[i] == '/' ||
			szOut[i] == '\\' ||
			szOut[i] == '|' ||
			szOut[i] == '?' ||
			szOut[i] == '*')
			szOut[i] = substitute;
	}
}

// Callback type of detected invalid file
typedef void ( *MCALLBACK_DETECTEDINVALIDFILE) ();





/// class : MZFileSystem
class MZFileSystem
{
	friend class MFile;


// Member variables
protected:
	bool					m_bCreate;				///< Flag of create file system
	PATHINDEX				m_PathIndex;			///< Path index
	FILEINDEX				m_FileIndex;			///< File index
	list< const MFileInfo*>		m_CheckFileList;		///< List to check CRC file
	MCrypt					m_Crypt;				///< Crypt
	CRITICAL_SECTION		m_csLock;				///< Critical section
	string					m_strIndexFName;		///< Index file name
	unsigned char			m_nAccessMode;			///< Access mode
	bool					m_bAllowAccUncompFile;	///< Flag to allow access uncompressed files
	bool					m_bDetectedInvalidFile;	///< Flag to detected invalid file


// Thread
protected:
	struct
	{
		HANDLE				hThread;				///< Handle of thread
		HANDLE				hEventDestroy;			///< Event handle of destroy thread
		bool				bExit;					///< Flag to exit thread
	} m_Thread;


// Callback
protected:
	MCALLBACK_DETECTEDINVALIDFILE			m_cbDetectedInvalidFile;



// Member functions
public:
	// Constructor/Destructor
	MZFileSystem();
	virtual ~MZFileSystem();


	// Create/Destroy
	bool Create( const char* szCurDirectory, int nAccessMode =MFILEACCESS_GENERIC);
	bool Destroy();


	// Get file system instance
	static MZFileSystem* GetInstance();


	// Read file index
	bool ReadFileIndex( const char* szFileName);


	// Allow access to uncompressed files
	bool SetAllowAccessUncompFile( bool bAllow);
	bool IsAllowedAccessUncompFile() const;
	MCrypt GetCrypt() { return m_Crypt; }


public:
	// Enter/Leave critical section
	void _lock();
	void _unlock();

	// Read/Write cached file index from disk
	bool _read_cached_fileindex( const char* szFileName);
	bool _write_cached_fileindex( const char* szFileName);

	// Find file info
	bool _find_fileinfo( const string& strFileName, const MFileInfo** pinfo);
	bool _find_fileinfo_from_disk( const string& strFileName, const MFileInfo** pInfo);

	// Find file from disk
	bool _find_file_from_disk( const string& strFileName, const MFileInfo** pInfo);

	bool _find_file_and_change_state( const string& _strOutFileName );


// Get/Set functions
public:
	// Get file index
	const FILEINDEX* GetFileIndex();

	// Set path
	void SetPath( const char* szPath, bool bIncludeSubDir =true);
	void AddPath( const char* szPath, bool bIncludeSubDir =true);

	// Is exist file
	bool IsExistFile( const char* szFileName);

	// Set file access mode
	int GetAccessMode() const;


// Thread
public:
	static unsigned long WINAPI _thread_filesystem( LPVOID lpParam);
	bool _is_exit_thread() const;
	void _on_create_thread();
	void _on_destroy_thread();
	bool _on_run_thread();


// Watch dog
public:
	void SetDetectInvalidFileCallback( MCALLBACK_DETECTEDINVALIDFILE _callback);


// Event handler
public:
	// Create/Destroy
	virtual void OnCreate();
	virtual void OnDestroy();

	// Open/Close file
	virtual void OnOpenFile( const char* szFileName, const MFileInfo* pInfo);
	virtual void OnCloseFile( const char* szFileName);

	// Detected invalid file
	virtual void OnDetectedInvalidFile();
};



// Get file system instance
MZFileSystem* MGetFileSystem();


// Create file system
MZFileSystem* MCreateFileSystem( const char* szCurDirectory, int nAccessMode, const char* szPath =NULL);
MZFileSystem* MCreateFileSystem( int nAccessMode, const char* szPath =NULL);
















// '경로+파일명+확장자' 중 '파일명+확장자'만 얻는다
string MGetPureFileNameExt( const string& strFileName);

// '경로+파일명+확장자' 중 '파일명'만 얻는다
string MGetPureFileName( const string& strFileName);

// '경로+파일명+확장자' 중 '확장자'만 얻는다
string MGetPureExtension( const string& szFileName);

// '경로+파일명+확장자' 중 경로를 제거한 '파일명+확장자'를 얻는다
string MGetRemovePathName( const string& strPathName);

// \ 를 / 로 대체한 경로를 얻는다
string MGetPathReplacedBackSlashToSlash( const string& strPath);

// 경로를 얻는다
string MGetPurePath( const string& strPath);
string MGetPathFileNameFromRelativePath( const string& strBasePath, const string& strRelative);	// 기준 경로에서 상대 경로에 있는 파일의 경로를 구한다 (예: "C:/Data/Model/" , "../Texture/file.dat" -> "C:/Data/Texture/file.dat")
string MGetRelativePathFileName( const string& strBasePath, const string& strFileName);			// 기준 경로에서 파일의 상대 경로를 구한다 (예: "C:/Data/Model/" , "C:/Data/Texture/file.dat" -> "../Texture/file.dat")

// 현재 경로를 얻는다
wstring MGetCurrentDirectoryW();
string MGetCurrentDirectoryA();
#ifdef _UNICODE	
#define MGetCurrentDirectory	MGetCurrentDirectoryW
#else
#define MGetCurrentDirectory	MGetCurrentDirectoryA
#endif

// 파일 존재 여부를 얻는다
bool MIsExistFileW( const wchar_t* szFileName);
bool MIsExistFileA( const char* szFileName);
#ifdef _UNICODE	
#define MIsExistFile	MIsExistFileW
#else
#define MIsExistFile	MIsExistFileA
#endif
