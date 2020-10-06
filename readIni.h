//
// Generated on 2020/10/04 by inimoni 
//

/*
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Documentation
  Struct = Section name, Field = Key name.
  The function used is only a declaration, load(), and save().
  The constructor automatically reads initialization file [initFileName].
  Please rewrite [initFileName] in the source freely.
  It becomes Windows folder when there is not path. The relative path can be used.

Example
#include "SampleIni.h"
void CTestDlg::OnButton1()
{
    //initFileName = "./sample.ini";
    SampleIni data1;                   // Open ./sample.ini
    int v = data1.Sample.value;        // [Sample]-value Get
    data1.Sample.value = v+1;          // [Sample]-value Modify
    data1.save();                      // Save ./Sample.ini (Opened file)

    SampleIni data2("sample.ini");     // Open C:/WINDOWS/Sample.ini
    data2.load("./sample.ini");        // Open Specified file.
    data2.save();                      // Save Opened file (Making at not being)
}

Supplementation
  Reading and writing becomes the unit of the file.
  [initFileName] is the same as the argument of GetPrivateProfileString() of SDK.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/

#ifndef READINI_H
#define READINI_H

#include <string>
using namespace std;

#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#ifdef _UNICODE
typedef std::wstring string_t;
#else
typedef std::string string_t;
#endif

namespace inimoni {
    template<class T>
    static bool inirw(int is_read, string_t& fname, string_t sec, string_t key, T& val_t);
};

/**
    readIni.ini input output class.
*/
class readIni
{
public:

    readIni(string_t fname = _T(""))
    {
        initFileName = _T("./readIni.ini"); //Default file.    
        init();
        load(fname);
    }

    /**
        INI file is read.
        @param fname Filename
        When there is not a file, It becomes initFileName.
        When there is not a pass, It becomes Windows folder.
    */
    bool load(string_t fname = _T(""))
    {
        if (fname.empty()) {
            fname = initFileName;
        }
        loadFileName = fname;
        WIN32_FIND_DATA fd;
        HANDLE h = ::FindFirstFile(fname.c_str(), &fd);
        if (h != INVALID_HANDLE_VALUE) {
            iniRW(fname, 1);
        }
        return (h != INVALID_HANDLE_VALUE);
    }

    /**
        It writes it in the INI file.
        @param fname Filename
        When there is not a file, It becomes open file.
        When there is not a pass, It becomes Windows folder.
    */
    bool save(string_t fname = _T(""))
    {
        if (fname.empty()) {
            fname = loadFileName;
        }
        iniRW(fname, 0);
        return true;
    }

public:

    // PANEL
    struct _PANEL
    {
        int      index0;
        int      index1;
        int      index2;
        int      index3;
        int      index4;
        int      index5;
        int      index6;
        int      index7;
        int      index8;
        int      index9;
        int      index10;
        int      index11;
        int      index12;
        int      index13;
        int      index14;
        int      index15;
        int      index16;
        int      index17;
        int      index18;
        int      index19;
    } PANEL;

    // BRAKE
    struct _BRAKE
    {
        double   EBN;
        double   B8N;
        double   B7N;
        double   B4N;
    } BRAKE;

    // BEACON
    struct _BEACON
    {
        int type0;
        int type1;
        int type2;
        int type3;
        int type4;
    } BEACON;

    // OPTION
    struct OPTION
    {
        int brake;
        int split;
        int margin;
    } OPTION;

protected:

    string_t initFileName;
    string_t loadFileName;

    bool iniRW(string_t f, int r)
    {
        string_t s;

        s = _T("PANEL");
        inimoni::inirw(r, f, s, _T("index0           "), PANEL.index0);
        inimoni::inirw(r, f, s, _T("index1           "), PANEL.index1);
        inimoni::inirw(r, f, s, _T("index2           "), PANEL.index2);
        inimoni::inirw(r, f, s, _T("index3           "), PANEL.index3);
        inimoni::inirw(r, f, s, _T("index4           "), PANEL.index4);
        inimoni::inirw(r, f, s, _T("index5           "), PANEL.index5);
        inimoni::inirw(r, f, s, _T("index6           "), PANEL.index6);
        inimoni::inirw(r, f, s, _T("index7           "), PANEL.index7);
        inimoni::inirw(r, f, s, _T("index8           "), PANEL.index8);
        inimoni::inirw(r, f, s, _T("index9           "), PANEL.index9);
        inimoni::inirw(r, f, s, _T("index10          "), PANEL.index10);
        inimoni::inirw(r, f, s, _T("index11          "), PANEL.index11);
        inimoni::inirw(r, f, s, _T("index12          "), PANEL.index12);
        inimoni::inirw(r, f, s, _T("index13          "), PANEL.index13);
        inimoni::inirw(r, f, s, _T("index14          "), PANEL.index14);
        inimoni::inirw(r, f, s, _T("index15          "), PANEL.index15);
        inimoni::inirw(r, f, s, _T("index16          "), PANEL.index16);
        inimoni::inirw(r, f, s, _T("index17          "), PANEL.index17);
        inimoni::inirw(r, f, s, _T("index18          "), PANEL.index18);
        inimoni::inirw(r, f, s, _T("index19          "), PANEL.index19);

        s = _T("BRAKE");
        inimoni::inirw(r, f, s, _T("EBN              "), BRAKE.EBN);
        inimoni::inirw(r, f, s, _T("B8N              "), BRAKE.B8N);
        inimoni::inirw(r, f, s, _T("B7N              "), BRAKE.B7N);
        inimoni::inirw(r, f, s, _T("B4N              "), BRAKE.B4N);

        s = _T("BEACON");
        inimoni::inirw(r, f, s, _T("type0            "), BEACON.type0);
        inimoni::inirw(r, f, s, _T("type1            "), BEACON.type1);
        inimoni::inirw(r, f, s, _T("type2            "), BEACON.type2);
        inimoni::inirw(r, f, s, _T("type3            "), BEACON.type3);
        inimoni::inirw(r, f, s, _T("type4            "), BEACON.type4);

        s = _T("OPTION");
        inimoni::inirw(r, f, s, _T("brake            "), OPTION.brake);
        inimoni::inirw(r, f, s, _T("split            "), OPTION.split);
        inimoni::inirw(r, f, s, _T("margin           "), OPTION.margin);
        return true;
    }

    void init()
    {
        PANEL.index0 = -1;
        PANEL.index1 = -1;
        PANEL.index2 = -1;
        PANEL.index3 = -1;
        PANEL.index4 = -1;
        PANEL.index5 = -1;
        PANEL.index6 = -1;
        PANEL.index7 = -1;
        PANEL.index8 = -1;
        PANEL.index9 = -1;
        PANEL.index10 = -1;
        PANEL.index11 = -1;
        PANEL.index12 = -1;
        PANEL.index13 = -1;
        PANEL.index14 = -1;
        PANEL.index15 = -1;
        PANEL.index16 = -1;
        PANEL.index17 = -1;
        PANEL.index18 = -1;
        PANEL.index18 = -1;
        BRAKE.EBN = 4.0;
        BRAKE.B8N = 3.5;
        BRAKE.B7N = 3.0;
        BRAKE.B4N = 2.0;
        BEACON.type0 = 0;
        BEACON.type1 = 1;
        BEACON.type2 = 2;
        BEACON.type3 = 3;
        BEACON.type4 = 4;
        OPTION.brake = 1;
        OPTION.split = 1;
        OPTION.margin = 0;
    }
};
typedef readIni ReadIniFile; //êVãåå›ä∑

//---------------------------------------------------------------------------
// Common method                                                             
//---------------------------------------------------------------------------
#ifndef INIMONI_INIRW                                                        
#define INIMONI_INIRW                                                        
namespace inimoni
{
    /*
    Read and Write INI file
      int     is_read  1=Read mode, 0=Write mode
      string  fname    Filename (The Windows folder when there is not path)
      string  sec      Section name
      string  key      Key name
      T       val_t    [Read]Init+Output, [Write]Input
    */
    template<class T>
    bool inirw(int is_read, string_t& fname, string_t sec, string_t key, T& val_t)
    {
        if (is_read) {
            inimoni::read(fname.c_str(), sec.c_str(), key.c_str(), val_t);
        }
        else {
            inimoni::write(fname.c_str(), sec.c_str(), key.c_str(), val_t);
        }
        return true;
    }

    bool read(string_t ifn, string_t sec, string_t key, int& dst)
    {
        dst = GetPrivateProfileInt(sec.c_str(), key.c_str(), dst, ifn.c_str());
        return true;
    }

    bool read(string_t ifn, string_t sec, string_t key, basic_string<TCHAR>& dst)
    {
        TCHAR buf[256];
        GetPrivateProfileString(
            sec.c_str(),
            key.c_str(),
            dst.c_str(),
            buf,
            sizeof(buf),
            ifn.c_str());
        dst = buf;
        return true;
    }

    bool read(string_t ifn, string_t sec, string_t key, double& dst)
    {
        string_t s;
        inimoni::read(ifn, sec, key, s);

        TCHAR* e;
        double x = _tcstod(s.c_str(), &e);

        if (!*e && s != "") {
            dst = x;
        }
        return true;
    }

    template<class T>
    bool write(string_t ifn, string_t sec, string_t key, T val_t)
    {
        TCHAR val[1024];
        inimoni::to_string(val, val_t);
        WritePrivateProfileString(sec.c_str(), key.c_str(), val, ifn.c_str());
        return true;
    }

    void to_string(TCHAR* str, int val)
    {
        _stprintf(str, _T("%d"), val);
    }

    void to_string(TCHAR* str, double val)
    {
        _stprintf(str, _T("%f"), val);
    }

    void to_string(TCHAR* str, basic_string<TCHAR> val)
    {
        _stprintf(str, _T("%s"), val.c_str());
    }
};

#endif                                                                  
#endif                                                                  

