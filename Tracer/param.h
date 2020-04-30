//**************************************************************
//     Interface module to interlanguage communication. WARNING: CODE IS UNSAFE!!!!!!!!!!!!!!!!
//**************************************************************

#include <istream>
#include <sstream>
#include <fstream>

using namespace std;

//------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------

#define MAXPARNAMELEN 64
#define MAXLINELEN 1024

//------------------------------------------------------------------------
// Export functions declaration
//------------------------------------------------------------------------
void ParseStream(istream&);

extern "C"
{
  void SetExpandMacros(bool);
  void ParseBuf(char*);
  void ParseFile(char*);
  int GetBufSize(char*, bool);
  int SaveBuf(char*, char*, bool);
  void RemovePar(char*);
  void CopyPar(char*, char*);
  void MovePar(char*, char*);
  bool ExistsPar(const char *name);
  int GetIntPar(char const*);
  double GetDblPar(const char *name);
  void GetStrPar(const char *name, char *value);
  void SetIntPar(char*, int);
  void SetDblPar(char*, double);
  void SetStrPar(char*, char*);
  void SetBoolPar(char*, bool);
  bool TestPar(char*, char*);
  bool GetBoolPar(char*);
}

//------------------------------------------------------------------------
// CList abstract class declaration
//------------------------------------------------------------------------
class CList
{
public:
  CList *m_next, *m_last;
  char m_name[MAXPARNAMELEN];

  explicit CList(char* name = nullptr);
  virtual ~CList();
  
  CList* Find(char*);
  bool FindPlace(CList*&);
  CList* Insert(CList*);
  void virtual Remove();

  static std::string prefixstr;
  static int prefixlen;
};


//------------------------------------------------------------------------
// CParameter class declaration
//------------------------------------------------------------------------
class CParameter : public CList
{
public:
  char *m_value;
  static bool m_expandmacros;

  explicit CParameter(char* name = nullptr, char* value = nullptr);
  ~CParameter() override;
  
  void Set(char*);
  CParameter* next() { return (CParameter*)m_next; }
  CParameter* Find(char*);
  CParameter* Insert(CParameter*);
  int Save(char*&);
  void GetMacroName(char*, char*);
};


//------------------------------------------------------------------------
// CSection class declaration
//------------------------------------------------------------------------
class CSection : public CList
{
public:
  CParameter m_plist;

  explicit CSection(char* name = nullptr);
  ~CSection();
  
  CSection* next() { return (CSection*)m_next; }
  CSection* Find(char* parname) { return (CSection*) CList::Find(parname); }
  CSection* Insert(CSection*);
  CParameter* LocatePar(const char *name);
  CParameter* CreatePar(char*);
  void Clear();
  static void SplitName(const char *name, char *secname, char *parname);
  bool isEmpty() { return !m_plist.m_next; }
  int SaveTitle(char*&);
  int Save(char*&);
};
