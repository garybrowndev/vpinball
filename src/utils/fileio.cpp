#include "core/stdafx.h"

#include <mutex>
static std::mutex mtx; //!! only used for Wine multithreading bug workaround

#ifdef __STANDALONE__
#include <fstream>
#endif

bool DirExists(const string& dirPath)
{
#ifdef _MSC_VER
   const DWORD fileAtt = GetFileAttributesA(dirPath.c_str());

   return (fileAtt != INVALID_FILE_ATTRIBUTES && (fileAtt & FILE_ATTRIBUTE_DIRECTORY));
#else
   struct stat info;
   if (stat(dirPath.c_str(), &info) != 0)
      return false;
   return (info.st_mode & S_IFDIR);
#endif
}

bool FileExists(const string& filePath)
{
#ifdef _MSC_VER
   //This will get the file attributes bitlist of the file
   const DWORD fileAtt = GetFileAttributesA(filePath.c_str());

   //If an error occurred it will equal to INVALID_FILE_ATTRIBUTES
   if (fileAtt == INVALID_FILE_ATTRIBUTES)
      return false;

   //If the path refers to a directory it should also not exist.
   return ((fileAtt & FILE_ATTRIBUTE_DIRECTORY) == 0);
#else
   struct stat info;

   if (stat(filePath.c_str(), &info) != 0)
      return false;
   return !(info.st_mode & S_IFDIR);
#endif
}

string ExtensionFromFilename(const string& filename)
{
   const size_t pos = filename.find_last_of('.');
   return (pos == string::npos) ? string() : filename.substr(pos + 1);
}

string TitleFromFilename(const string& filename)
{
   // Find the last path separator
   size_t begin = filename.find_last_of(PATH_SEPARATOR_CHAR);
   if (begin == string::npos)
      begin = 0;
   else
      begin++;

   // Find the last dot after the last path separator
   size_t end = filename.find_last_of('.');
   if (end == string::npos || end < begin)
      end = filename.length();

   return filename.substr(begin, end - begin);
}

string PathFromFilename(const string &filename)
{
   const size_t pos = filename.find_last_of(PATH_SEPARATOR_CHAR);
   return (pos == string::npos) ? string() : filename.substr(0, pos + 1); // previously returned filename if no separator found, but i guess that just worked because filename was then also constantly ""
}

// same as removing the file extension
string TitleAndPathFromFilename(const string &filename)
{
   return filename.substr(0, filename.find_last_of('.')); // in case no '.' is found, will then copy full filename
}

bool ReplaceExtensionFromFilename(string& filename, const string& newextension)
{
   const size_t i = filename.find_last_of('.');

   if (i != string::npos)
   {
      filename.replace(i + 1, newextension.length(), newextension);
      return true;
   }

   return false;
}

BiffWriter::BiffWriter(IStream *pistream, const HCRYPTHASH hcrypthash)
{
   m_pistream = pistream;
   m_hcrypthash = hcrypthash;
}

HRESULT BiffWriter::WriteBytes(const void *pv, const ULONG count, ULONG *foo)
{
#ifndef __STANDALONE__
   if (m_hcrypthash)
      CryptHashData(m_hcrypthash, (BYTE *)pv, count, 0);
#endif

   return m_pistream->Write(pv, count, foo);
}

HRESULT BiffWriter::WriteRecordSize(const int size)
{
   ULONG writ = 0;
   const HRESULT hr = m_pistream->Write(&size, sizeof(size), &writ);
   return hr;
}

HRESULT BiffWriter::WriteInt(const int id, const int value)
{
   ULONG writ = 0;
   HRESULT hr;

   if (FAILED(hr = WriteRecordSize(sizeof(int) * 2)))
      return hr;

   if (FAILED(hr = WriteBytes(&id, sizeof(int), &writ)))
      return hr;

   hr = WriteBytes(&value, sizeof(int), &writ);

   return hr;
}

HRESULT BiffWriter::WriteString(const int id, const char * const szvalue)
{
   ULONG writ = 0;
   HRESULT hr;
   const int len = (int)strlen(szvalue);

   if (FAILED(hr = WriteRecordSize((int)sizeof(int) * 2 + len)))
      return hr;

   if (FAILED(hr = WriteBytes(&id, sizeof(int), &writ)))
      return hr;

   if (FAILED(hr = WriteBytes(&len, sizeof(int), &writ)))
      return hr;

   hr = WriteBytes(szvalue, len, &writ);

   return hr;
}

HRESULT BiffWriter::WriteString(const int id, const string &szvalue)
{
   ULONG writ = 0;
   HRESULT hr;
   const int len = (int)szvalue.length();

   if (FAILED(hr = WriteRecordSize((int)sizeof(int) * 2 + len)))
      return hr;

   if (FAILED(hr = WriteBytes(&id, sizeof(int), &writ)))
      return hr;

   if (FAILED(hr = WriteBytes(&len, sizeof(int), &writ)))
      return hr;

   hr = WriteBytes(szvalue.data(), len, &writ);

   return hr;
}

HRESULT BiffWriter::WriteWideString(const int id, const WCHAR * const wzvalue)
{
   ULONG writ = 0;
   HRESULT hr;
   const int len = (int)wcslen(wzvalue) * (int)sizeof(WCHAR);

   if (FAILED(hr = WriteRecordSize((int)sizeof(int) * 2 + len)))
      return hr;

   if (FAILED(hr = WriteBytes(&id, sizeof(int), &writ)))
      return hr;

   if (FAILED(hr = WriteBytes(&len, sizeof(int), &writ)))
      return hr;

   hr = WriteBytes(wzvalue, len, &writ);

   return hr;
}

HRESULT BiffWriter::WriteWideString(const int id, const std::basic_string<WCHAR>& wzvalue)
{
   ULONG writ = 0;
   HRESULT hr;
   const int len = (int)wzvalue.length() * (int)sizeof(WCHAR);

   if (FAILED(hr = WriteRecordSize((int)sizeof(int) * 2 + len)))
      return hr;

   if (FAILED(hr = WriteBytes(&id, sizeof(int), &writ)))
      return hr;

   if (FAILED(hr = WriteBytes(&len, sizeof(int), &writ)))
      return hr;

   hr = WriteBytes(wzvalue.data(), len, &writ);

   return hr;
}

HRESULT BiffWriter::WriteBool(const int id, const BOOL value)
{
   ULONG writ = 0;
   HRESULT hr;

   if (FAILED(hr = WriteRecordSize(sizeof(int) * 2)))
      return hr;

   if (FAILED(hr = WriteBytes(&id, sizeof(int), &writ)))
      return hr;

   hr = WriteBytes(&value, sizeof(BOOL), &writ);

   return hr;
}

HRESULT BiffWriter::WriteFloat(const int id, const float value)
{
   ULONG writ = 0;
   HRESULT hr;

   if (FAILED(hr = WriteRecordSize(sizeof(int) + sizeof(float))))
      return hr;

   if (FAILED(hr = WriteBytes(&id, sizeof(int), &writ)))
      return hr;

   hr = WriteBytes(&value, sizeof(float), &writ);

   return hr;
}

HRESULT BiffWriter::WriteStruct(const int id, const void * const pvalue, const int size)
{
   ULONG writ = 0;
   HRESULT hr;

   if (FAILED(hr = WriteRecordSize((int)sizeof(int) + size)))
      return hr;

   if (FAILED(hr = WriteBytes(&id, sizeof(int), &writ)))
      return hr;

   hr = WriteBytes(pvalue, size, &writ);

   return hr;
}

HRESULT BiffWriter::WriteVector2(const int id, const Vertex2D& vec)
{
   return WriteStruct(id, &vec.x, 2 * sizeof(float));
}

HRESULT BiffWriter::WriteVector3(const int id, const Vertex3Ds& vec)
{
   return WriteStruct(id, &vec.x, 3 * sizeof(float));
}

HRESULT BiffWriter::WriteVector3Padded(const int id, const Vertex3Ds& vec)
{
   const float data[4] = { vec.x,vec.y,vec.z,0.0f };
   return WriteStruct(id, data, 4 * sizeof(float));
}

HRESULT BiffWriter::WriteTag(const int id)
{
   ULONG writ = 0;
   HRESULT hr;

   if (FAILED(hr = WriteRecordSize(sizeof(int))))
      return hr;

   hr = WriteBytes(&id, sizeof(int), &writ);

   return hr;
}

BiffReader::BiffReader(IStream *pistream, ILoadable *piloadable, void *ppassdata, const int version, const HCRYPTHASH hcrypthash, const HCRYPTKEY hcryptkey)
{
   m_pistream = pistream;
   m_piloadable = piloadable;
   m_pdata = ppassdata;
   m_version = version;

   m_bytesinrecordremaining = 0;

   m_hcrypthash = hcrypthash;
   m_hcryptkey = hcryptkey;
}

HRESULT BiffReader::ReadBytes(void * const pv, const ULONG count, ULONG * const foo)
{
   const bool iow = IsOnWine();
   if (iow)
      mtx.lock();
   const HRESULT hr = m_pistream->Read(pv, count, foo);
   if (iow)
      mtx.unlock();

#ifndef __STANDALONE__
   if (m_hcrypthash)
      CryptHashData(m_hcrypthash, (BYTE *)pv, count, 0);
#endif

   return hr;
}

HRESULT BiffReader::GetIntNoHash(int &value)
{
   m_bytesinrecordremaining -= sizeof(int);

   ULONG read = 0;
   const bool iow = IsOnWine();
   if (iow)
      mtx.lock();
   const HRESULT hr = m_pistream->Read(&value, sizeof(int), &read);
   if (iow)
      mtx.unlock();
   return hr;
}

HRESULT BiffReader::GetInt(void * const value)
{
   m_bytesinrecordremaining -= sizeof(int);

   ULONG read = 0;
   return ReadBytes(value, sizeof(int), &read);
}

HRESULT BiffReader::GetInt(int &value)
{
   m_bytesinrecordremaining -= sizeof(int);

   ULONG read = 0;
   return ReadBytes(&value, sizeof(int), &read);
}

HRESULT BiffReader::GetString(char *const szvalue, const size_t szvalue_maxlength)
{
   ULONG read = 0;
   HRESULT hr;
   int len;

   if (FAILED(hr = ReadBytes(&len, sizeof(int), &read)))
   {
      szvalue[0] = '\0';
      return hr;
   }

   m_bytesinrecordremaining -= len + (int)sizeof(int);

   char *tmp = new char[len+1];
   hr = ReadBytes(tmp, len, &read);
   tmp[len] = '\0';
   strncpy_s(szvalue, szvalue_maxlength, tmp, len);
   delete[] tmp;
   return hr;
}

HRESULT BiffReader::GetString(string &szvalue)
{
   ULONG read = 0;
   HRESULT hr;
   int len;

   if (FAILED(hr = ReadBytes(&len, sizeof(int), &read)))
   {
      szvalue.clear();
      return hr;
   }

   m_bytesinrecordremaining -= len + (int)sizeof(int);

   char *tmp = new char[len+1];
   hr = ReadBytes(tmp, len, &read);
   tmp[len] = 0;
   szvalue = tmp;
   delete[] tmp;
   return hr;
}

HRESULT BiffReader::GetWideString(WCHAR *wzvalue, const size_t wzvalue_maxlength)
{
   ULONG read = 0;
   HRESULT hr;
   int len;

   if (FAILED(hr = ReadBytes(&len, sizeof(int), &read)))
   {
      wzvalue[0] = L'\0';
      return hr;
   }

   m_bytesinrecordremaining -= len + (int)sizeof(int);

#ifndef __STANDALONE__
   WCHAR * tmp = new WCHAR[len/sizeof(WCHAR)+1];
   hr = ReadBytes(tmp, len, &read);
   tmp[len/sizeof(WCHAR)] = L'\0';
#else
   WCHAR * tmp = new WCHAR[len/2+1];
   memset(tmp, 0, (len/2+1) * sizeof(WCHAR));
   char* ptr = (char*)tmp;
   for (int index = 0; index < len/2; index++) {
      hr = ReadBytes(ptr, 2, &read);
      ptr += sizeof(WCHAR);
   }
#endif
   wcscpy_s(wzvalue, wzvalue_maxlength, tmp);
   delete[] tmp;
   return hr;
}

HRESULT BiffReader::GetWideString(std::basic_string<WCHAR>& wzvalue)
{
   ULONG read = 0;
   HRESULT hr;
   int len;

   if (FAILED(hr = ReadBytes(&len, sizeof(int), &read)))
   {
      wzvalue.clear();
      return hr;
   }

   m_bytesinrecordremaining -= len + (int)sizeof(int);

#ifndef __STANDALONE__
   WCHAR * tmp = new WCHAR[len/sizeof(WCHAR)+1];
   hr = ReadBytes(tmp, len, &read);
   tmp[len/sizeof(WCHAR)] = 0;
#else
   WCHAR * tmp = new WCHAR[len/2+1];
   memset(tmp, 0, (len/2+1) * sizeof(WCHAR));
   char* ptr = (char*)tmp;
   for (int index = 0; index < len/2; index++) {
      hr = ReadBytes(ptr, 2, &read);
      ptr += sizeof(WCHAR);
   }
   tmp[len/2] = 0;
#endif
   wzvalue = tmp;
   delete[] tmp;
   return hr;
}

HRESULT BiffReader::GetFloat(float &value)
{
   m_bytesinrecordremaining -= sizeof(float);

   ULONG read = 0;
   return ReadBytes(&value, sizeof(float), &read);
}

HRESULT BiffReader::GetBool(BOOL &value)
{
   m_bytesinrecordremaining -= sizeof(BOOL);

   ULONG read = 0;
   //return m_pistream->Read(&value, sizeof(BOOL), &read);
   return ReadBytes(&value, sizeof(BOOL), &read);
}

HRESULT BiffReader::GetStruct(void *pvalue, const int size)
{
   m_bytesinrecordremaining -= size;

   ULONG read = 0;
   return ReadBytes(pvalue, size, &read);
}

HRESULT BiffReader::GetVector2(Vertex2D& vec)
{
   assert(sizeof(Vertex2D) == 2 * sizeof(float));     // fields need to be contiguous
   return GetStruct(&vec.x, 2 * sizeof(float));
}

HRESULT BiffReader::GetVector3(Vertex3Ds& vec)
{
   assert(sizeof(Vertex3Ds) == 3 * sizeof(float));     // fields need to be contiguous
   return GetStruct(&vec.x, 3 * sizeof(float));
}

HRESULT BiffReader::GetVector3Padded(Vertex3Ds& vec)
{
   float data[4];
   const HRESULT hr = GetStruct(data, 4 * sizeof(float));
   if (SUCCEEDED(hr))
   {
      vec.x = data[0];
      vec.y = data[1];
      vec.z = data[2];
   }
   return hr;
}

HRESULT BiffReader::Load(std::function<bool(const int id, BiffReader *const pbr)> processToken)
{
   int tag = 0;
   while (tag != FID(ENDB))
   {
      if (m_version > 30)
      {
         /*const HRESULT hr =*/ GetIntNoHash(m_bytesinrecordremaining);
      }

      const HRESULT hr = GetInt(tag);

      bool cont = false;
      if (hr == S_OK)
         cont = processToken ? processToken(tag, this) : m_piloadable->LoadToken(tag, this);

      if (!cont)
         return E_FAIL;

      if (m_version > 30)
      {
         assert(m_bytesinrecordremaining >= 0);

         if (m_bytesinrecordremaining > 0)
         {
            BYTE * const szT = new BYTE[m_bytesinrecordremaining];
            /*const HRESULT hr =*/ GetStruct(szT, m_bytesinrecordremaining);
            delete[] szT;
         }
      }
   }

   return S_OK;
}

FastIStorage::FastIStorage()
   : m_cref(0)
   , m_wzName(nullptr)
{
}

FastIStorage::~FastIStorage()
{
   for (size_t i = 0; i < m_vstg.size(); i++)
      m_vstg[i]->Release();

   for (size_t i = 0; i < m_vstm.size(); i++)
      m_vstm[i]->Release();

   SAFE_VECTOR_DELETE(m_wzName);
}

HRESULT __stdcall FastIStorage::QueryInterface(const struct _GUID &, void **)
{
   return S_OK;
}

ULONG __stdcall FastIStorage::AddRef()
{
   m_cref++;

   return S_OK;
}

ULONG __stdcall FastIStorage::Release()
{
   m_cref--;

   if (m_cref == 0)
   {
      delete this; // legal, but meh
   }

   return S_OK;
}

HRESULT __stdcall FastIStorage::CreateStream(const WCHAR *wzName, ULONG, ULONG, ULONG, struct IStream **ppstm)
{
   FastIStream * const pfs = new FastIStream();
   const size_t wzNameLen = wcslen(wzName) + 1; // incl. zero terminator
   pfs->AddRef(); // AddRef once for us, and once for the caller
   pfs->AddRef();
   pfs->m_wzName = new WCHAR[wzNameLen];
   wcscpy_s(pfs->m_wzName, wzNameLen, wzName);

   *ppstm = pfs;

   m_vstm.push_back(pfs);

   return S_OK;
}

HRESULT __stdcall FastIStorage::OpenStream(const WCHAR *, void *, ULONG, ULONG, struct IStream **)
{
   return S_OK;
}

HRESULT __stdcall FastIStorage::CreateStorage(const WCHAR *wzName, ULONG, ULONG, ULONG, struct IStorage **ppstg)
{
   FastIStorage * const pfs = new FastIStorage();
   const size_t wzNameLen = wcslen(wzName) + 1; // incl. zero terminator
   pfs->AddRef(); // AddRef once for us, and once for the caller
   pfs->AddRef();
   pfs->m_wzName = new WCHAR[wzNameLen];
   wcscpy_s(pfs->m_wzName, wzNameLen, wzName);

   *ppstg = pfs;

   m_vstg.push_back(pfs);

   return S_OK;
}

HRESULT __stdcall FastIStorage::OpenStorage(const WCHAR *, struct IStorage *, ULONG, WCHAR **, ULONG, struct IStorage **)
{
   return S_OK;
}

HRESULT __stdcall FastIStorage::CopyTo(ULONG, const struct _GUID *, WCHAR **, struct IStorage *pstgNew)
{
   HRESULT hr;
   IStorage *pstgT;
   IStream *pstmT;

   for (size_t i = 0; i < m_vstg.size(); i++)
   {
      FastIStorage * const pstgCur = m_vstg[i];
      if (SUCCEEDED(hr = pstgNew->CreateStorage(pstgCur->m_wzName, STGM_DIRECT | STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE, 0, 0, &pstgT)))
      {
         pstgCur->CopyTo(0, nullptr, nullptr, pstgT);
         pstgT->Release();
      }
   }

   for (size_t i = 0; i < m_vstm.size(); i++)
   {
      const FastIStream * const pstmCur = m_vstm[i];
      if (SUCCEEDED(hr = pstgNew->CreateStream(pstmCur->m_wzName, STGM_DIRECT | STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE, 0, 0, &pstmT)))
      {
         ULONG writ;
         //pstmCur->CopyTo(0, nullptr, nullptr, pstmT);
         pstmT->Write(pstmCur->m_rg, pstmCur->m_cSize, &writ);
         pstmT->Release();
      }
   }

   return S_OK;
}

HRESULT __stdcall FastIStorage::MoveElementTo(const WCHAR *, struct IStorage *, const WCHAR *, ULONG)
{
   return S_OK;
}

HRESULT __stdcall FastIStorage::Commit(ULONG)
{
   return S_OK;
}

HRESULT __stdcall FastIStorage::Revert()
{
   return S_OK;
}

HRESULT __stdcall FastIStorage::EnumElements(ULONG, void *, ULONG, struct IEnumSTATSTG **)
{
   return S_OK;
}

HRESULT __stdcall FastIStorage::DestroyElement(const WCHAR *)
{
   return S_OK;
}

HRESULT __stdcall FastIStorage::RenameElement(const WCHAR *, const WCHAR *)
{
   return S_OK;
}

HRESULT __stdcall FastIStorage::SetElementTimes(const WCHAR *, const struct _FILETIME *, const struct _FILETIME *, const struct _FILETIME *)
{
   return S_OK;
}

HRESULT __stdcall FastIStorage::SetClass(const struct _GUID &)
{
   return S_OK;
}

HRESULT __stdcall FastIStorage::SetStateBits(ULONG, ULONG)
{
   return S_OK;
}

HRESULT __stdcall FastIStorage::Stat(struct tagSTATSTG *, ULONG)
{
   return S_OK;
}

FastIStream::FastIStream()
   : m_rg(nullptr)
   , m_wzName(nullptr)
   , m_cSize(0)
   , m_cref(0)
   , m_cMax(0)
   , m_cSeek(0)
{
}

FastIStream::~FastIStream()
{
   free(m_rg);
   SAFE_VECTOR_DELETE(m_wzName);
}

void FastIStream::SetSize(const unsigned int i)
{
   if (i > m_cMax)
   {
      void *m_rgNew;

      if (m_rg)
         m_rgNew = realloc((void *)m_rg, i);
      else
         m_rgNew = malloc(i);

      m_rg = (char *)m_rgNew;
      m_cMax = i;
   }
}

HRESULT __stdcall FastIStream::QueryInterface(const struct _GUID &, void **)
{
   return S_OK;
}

ULONG __stdcall FastIStream::AddRef()
{
   m_cref++;

   return S_OK;
}

ULONG __stdcall FastIStream::Release()
{
   m_cref--;

   if (m_cref == 0)
   {
      delete this; //!! legal, but meh
   }

   return S_OK;
}

HRESULT __stdcall FastIStream::Read(void *pv, const ULONG count, ULONG *foo)
{
   memcpy(pv, m_rg + m_cSeek, count);
   m_cSeek += count;

   if (foo != nullptr)
      *foo = count;

   return S_OK;
}

HRESULT __stdcall FastIStream::Write(const void *pv, const ULONG count, ULONG *foo)
{
   if ((m_cSeek + (unsigned int)count) > m_cMax)
      SetSize(max(m_cSeek * 2, m_cSeek + (unsigned int)count));

   memcpy(m_rg + m_cSeek, pv, count);
   m_cSeek += (unsigned int)count;

   m_cSize = max(m_cSize, m_cSeek);

   if (foo != nullptr)
      *foo = count;

   return S_OK;
}

HRESULT __stdcall FastIStream::Seek(union _LARGE_INTEGER li, const ULONG origin, union _ULARGE_INTEGER *puiOut)
{
   switch (origin)
   {
   case STREAM_SEEK_SET:
      m_cSeek = li.LowPart;
      break;

   case STREAM_SEEK_CUR:
      m_cSeek += li.LowPart;
      break;
   }

   if (puiOut)
      puiOut->QuadPart = m_cSeek;

   return S_OK;
}

HRESULT __stdcall FastIStream::SetSize(union _ULARGE_INTEGER)
{
   return S_OK;
}

HRESULT __stdcall FastIStream::CopyTo(struct IStream *, union _ULARGE_INTEGER, union _ULARGE_INTEGER *, union _ULARGE_INTEGER *)
{
   return S_OK;
}

HRESULT __stdcall FastIStream::Commit(ULONG)
{
   return S_OK;
}

HRESULT __stdcall FastIStream::Revert()
{
   return S_OK;
}

HRESULT __stdcall FastIStream::LockRegion(union _ULARGE_INTEGER, union _ULARGE_INTEGER, ULONG)
{
   return S_OK;
}

HRESULT __stdcall FastIStream::UnlockRegion(union _ULARGE_INTEGER, union _ULARGE_INTEGER, ULONG)
{
   return S_OK;
}

HRESULT __stdcall FastIStream::Stat(struct tagSTATSTG *, ULONG)
{
   return S_OK;
}

HRESULT __stdcall FastIStream::Clone(struct IStream **)
{
   return S_OK;
}
