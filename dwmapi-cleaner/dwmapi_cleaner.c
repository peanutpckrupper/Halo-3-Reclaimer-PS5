/* dwmapi_cleaner: a stand-in dwmapi.dll for running Project Reclaimer's
 * Halo 3 worker under prospero-win on the PS5.
 *
 * The worker creates "native-data" in its working directory and refuses to
 * start when it already exists ("File already exists (os error 183)"). On
 * Windows its supervisor gives it a fresh mcc-startup-<id> folder each run;
 * on the PS5 the profile has to name one fixed folder, so the previous
 * run's native-data is always still there.
 *
 * The worker imports dwmapi.dll, so Windows loads this DLL before the
 * worker's own code runs. When this DLL attaches, it deletes native-data
 * from the working directory, but only when that directory's name starts
 * with "mcc-startup-", so it never touches anything else. It also stands in
 * for the few dwmapi functions programs use; there is no compositor on the
 * PS5, so they only report success or "not available". */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wchar.h>

static void delete_tree(const WCHAR *dir, int depth)
{
    WCHAR pattern[MAX_PATH + 4], path[MAX_PATH];
    WIN32_FIND_DATAW fd;
    HANDLE find;

    if (depth > 32 || _snwprintf(pattern, MAX_PATH + 4, L"%ls\\*", dir) < 0) return;
    find = FindFirstFileW(pattern, &fd);
    if (find != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!wcscmp(fd.cFileName, L".") || !wcscmp(fd.cFileName, L"..")) continue;
            if (_snwprintf(path, MAX_PATH, L"%ls\\%ls", dir, fd.cFileName) < 0) continue;
            path[MAX_PATH - 1] = 0;
            if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                !(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))
                delete_tree(path, depth + 1);
            else
            {
                SetFileAttributesW(path, FILE_ATTRIBUTE_NORMAL);
                DeleteFileW(path);
            }
        } while (FindNextFileW(find, &fd));
        FindClose(find);
    }
    RemoveDirectoryW(dir);
}

static void clear_native_data(void)
{
    WCHAR cwd[MAX_PATH], target[MAX_PATH];
    const WCHAR *name;
    DWORD len = GetCurrentDirectoryW(MAX_PATH, cwd);

    if (!len || len >= MAX_PATH - 16) return;
    while (len > 3 && (cwd[len - 1] == L'\\' || cwd[len - 1] == L'/')) cwd[--len] = 0;
    name = wcsrchr(cwd, L'\\');
    name = name ? name + 1 : cwd;
    if (_wcsnicmp(name, L"mcc-startup-", 12)) return;
    _snwprintf(target, MAX_PATH, L"%ls\\native-data", cwd);
    target[MAX_PATH - 1] = 0;
    if (GetFileAttributesW(target) != INVALID_FILE_ATTRIBUTES) delete_tree(target, 0);
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(instance);
        clear_native_data();
    }
    return TRUE;
}

/* The dwmapi functions. No compositor: report success where a caller only
 * asks for an effect, and "not available" where it asks for information. */
typedef struct { DWORD dwFlags; BOOL fEnable; HRGN hRgnBlur; BOOL fTransitionOnMaximized; } blur_behind;
typedef struct { int left, right, top, bottom; } margins;

__declspec(dllexport) HRESULT WINAPI DwmSetWindowAttribute(HWND hwnd, DWORD attr, const void *data, DWORD size)
{ return S_OK; }
__declspec(dllexport) HRESULT WINAPI DwmGetWindowAttribute(HWND hwnd, DWORD attr, void *data, DWORD size)
{ return E_NOTIMPL; }
__declspec(dllexport) HRESULT WINAPI DwmEnableBlurBehindWindow(HWND hwnd, const blur_behind *bb)
{ return S_OK; }
__declspec(dllexport) HRESULT WINAPI DwmExtendFrameIntoClientArea(HWND hwnd, const margins *m)
{ return S_OK; }
__declspec(dllexport) HRESULT WINAPI DwmIsCompositionEnabled(BOOL *enabled)
{ if (!enabled) return E_INVALIDARG; *enabled = FALSE; return S_OK; }
__declspec(dllexport) HRESULT WINAPI DwmEnableComposition(UINT action)
{ return S_OK; }
__declspec(dllexport) HRESULT WINAPI DwmFlush(void)
{ return S_OK; }
__declspec(dllexport) HRESULT WINAPI DwmGetColorizationColor(DWORD *color, BOOL *opaque)
{ return E_NOTIMPL; }
__declspec(dllexport) BOOL WINAPI DwmDefWindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, LRESULT *result)
{ return FALSE; }
__declspec(dllexport) HRESULT WINAPI DwmEnableMMCSS(BOOL enable)
{ return S_OK; }
__declspec(dllexport) HRESULT WINAPI DwmGetCompositionTimingInfo(HWND hwnd, void *info)
{ return E_NOTIMPL; }
__declspec(dllexport) HRESULT WINAPI DwmInvalidateIconicBitmaps(HWND hwnd)
{ return S_OK; }
