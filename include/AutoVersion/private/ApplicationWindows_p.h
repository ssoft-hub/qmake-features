#pragma once
#ifndef AUTO_VERSION_APPLICATION_WINDOWS_IMPL
#define AUTO_VERSION_APPLICATION_WINDOWS_IMPL

#include <AutoVersion/Module.h>

#include <cstdarg>
#include <string>
#include <vector>

#include <libloaderapi.h>
#include <tchar.h>
#include <winver.h>

namespace AutoVersion
{
    template <size_t>
    struct TypeHelper;

    extern::AutoVersion::Info info ();

    template <>
    struct TypeHelper<sizeof(char)>
    {
        typedef typename ::std::string String;
        static int print(char * buffer, size_t len, char const * format, va_list args)
        {
            return vsnprintf(buffer, len, format, args);
            va_end(args);
        }
        static char const * parameterFormat() { return "\\StringFileInfo\\%04x%04x\\%s"; }
    };

    template <>
    struct TypeHelper<sizeof(wchar_t)>
    {
        typedef typename ::std::wstring String;
        static int print(wchar_t * buffer, size_t len, wchar_t const * format, va_list args)
        {
            return vswprintf(buffer, len, format, args);
            va_end(args);
        }
        static wchar_t const * parameterFormat() { return L"\\StringFileInfo\\%04x%04x\\%ls"; }
    };

    struct Application
    {
        typedef ::AutoVersion::RuntimeVersion::Module Module;
        typedef ::AutoVersion::RuntimeVersion::Modules Modules;
        typedef ::std::map<HMODULE, Module *> ModuleForHandle;
        typedef TypeHelper<sizeof(TCHAR)>::String TSTRING;

        RuntimeVersion m_runtime;
        ModuleForHandle m_module_for_handle;

        static int print(TCHAR * buffer, size_t len, const TCHAR * format, ...)
        {
            va_list args;
            va_start(args, format);
            int result = ::AutoVersion::TypeHelper<sizeof(TCHAR)>::print(buffer, len, format, args);
            va_end(args);
            return result;
        }
        static const TCHAR * parameterFormat()
        {
            return ::AutoVersion::TypeHelper<sizeof(TCHAR)>::parameterFormat();
        }

        Application() { init(); }

        void init(const TSTRING & name = TSTRING())
        {
            HMODULE handle = GetModuleHandle(name.empty() ? LPCTSTR() : name.c_str());
            if (!handle)
            {
                return;
            }

                   // product
            Info::Attribute product;
            ModuleForHandle::iterator module_hiter = m_module_for_handle.find(handle);
            if (module_hiter == m_module_for_handle.end())
            {
                Module module;

                       // case this application
                if (name.empty())
                {
                    module.m_info = ::AutoVersion::info();
                    product = module.m_info[Info::key("product")];
                }
                else
                {
                    module.m_aliases.insert(Info::attribute(name));
                }

                typedef void * void_ptr;

                       // case runtime info method
                size_t dot_position = name.find_last_of(TEXT("."));
                ::std::string module_name = (dot_position != ::std::string::npos)
                    ? ::std::string(name.begin(), name.begin() + dot_position)
                    : ::std::string(name.begin(), name.end());
                ::std::string version_function_name = ::std::string("runtimeInfoAbout_") + module_name;
                typedef ::AutoVersion::Info (*InfoMethod)();
                InfoMethod info_method = InfoMethod(void_ptr(GetProcAddress(handle,
                    version_function_name.c_str())));
                if (info_method)
                {
                    module.m_info = info_method();
                    product = module.m_info[Info::key("product")];
                }
                else if (module_name.find("Qt") == 0)
                {
                    // case Qt version method
                    typedef char const * (*QtVersionMethod)();
                    QtVersionMethod qt_version_method =
                        QtVersionMethod(void_ptr(GetProcAddress(handle, "qVersion")));
                    if (qt_version_method)
                    {
                        if (module.m_info[Info::key("revision")].empty())
                            module.m_info[Info::key("revision")] = Info::attribute(qt_version_method());
                        if (module.m_info[Info::key("version")].empty())
                            module.m_info[Info::key("version")] = Info::attribute(qt_version_method());
                    }
                }

                       // path
                TSTRING path;
                DWORD path_buffer_size = DWORD();
                DWORD path_size = MAX_PATH;
                while (path_size && path_size >= path_buffer_size)
                {
                    path_buffer_size += MAX_PATH;
                    path.resize(path_buffer_size);
                    path_size = GetModuleFileName(handle, &path.front(), path_buffer_size);
                }
                path.resize(path_size);
                module.m_info[Info::key("path")] = Info::attribute(path);

                       // file version
                DWORD version_info_size = GetFileVersionInfoSize(path.c_str(), LPDWORD());
                if (version_info_size)
                {
                    typedef ::std::vector<BYTE> Bytes;
                    Bytes version_buffer = Bytes(version_info_size);
                    if (GetFileVersionInfo(path.c_str(), DWORD(), version_info_size,
                            LPVOID(version_buffer.data())))
                    {
                        LPVOID version_info = &version_buffer.front(); // Must be LPCVOID but it is
                                                                       // issue for some compilers.
                        VS_FIXEDFILEINFO * file_info = 0;

                               // Version
                               // File info
                        UINT file_info_size = UINT();
                        if (VerQueryValue(version_info, TEXT("\\"), (LPVOID *)&file_info, &file_info_size))
                        {
                            if (file_info)
                            {
                                TCHAR buffer[24];

                                if (module.m_info[Info::key("file.version")].empty())
                                {
                                    TCHAR format[] = TEXT("%d.%d.%d.%d");
                                    print(buffer, 24, format, int((file_info->dwFileVersionMS >> 16) & 0xFFFF),
                                        int((file_info->dwFileVersionMS) & 0xFFFF),
                                        int((file_info->dwFileVersionLS >> 16) & 0xFFFF),
                                        int((file_info->dwFileVersionLS) & 0xFFFF));
                                    module.m_info[Info::key("file.version")] = Info::attribute(buffer);
                                }

                                if (module.m_info[Info::key("version")].empty())
                                {
                                    TCHAR format[] = TEXT("%d.%d.%d");
                                    print(buffer, 24, format, int((file_info->dwFileVersionMS >> 16) & 0xFFFF),
                                        int((file_info->dwFileVersionMS) & 0xFFFF),
                                        int((file_info->dwFileVersionLS >> 16) & 0xFFFF));
                                    module.m_info[Info::key("version")] = Info::attribute(buffer);
                                }

                                       // TCHAR value_format [] = TEXT( "%d" );
                                       // print( buffer, 24, value_format, int( (
                                       // file_info->dwFileVersionMS >> 16 ) & 0xFFFF ) ); module.m_info[
                                       // file_version + Info::key( ".major" ) ] = Info::attribute( buffer
                                       // ); print( buffer, 24, value_format, int( (
                                       // file_info->dwFileVersionMS ) & 0xFFFF ) ); module.m_info[
                                       // file_version + Info::key( ".minor" ) ] = Info::attribute( buffer
                                       // ); print( buffer, 24, value_format, int( (
                                       // file_info->dwFileVersionLS >> 16 ) & 0xFFFF ) ); module.m_info[
                                       // file_version + Info::key( ".patch" ) ] = Info::attribute( buffer
                                       // ); print( buffer, 24, value_format, int( (
                                       // file_info->dwFileVersionLS ) & 0xFFFF ) ); module.m_info[
                                       // file_version + Info::key( ".other" ) ] = Info::attribute( buffer
                                       // );
                            }
                        }

                               // Translation
                        struct LANGANDCODEPAGE
                        {
                            WORD m_language;
                            WORD m_code_page;
                        } * translation;
                        UINT translation_count = UINT();
                        if (VerQueryValue(version_info, TEXT("VarFileInfo\\Translation"),
                                (LPVOID *)&translation, &translation_count))
                        {
                            static const TCHAR * parameter_names[] = {TEXT("Author"), TEXT("Comments"),
                                TEXT("CompanyName"), TEXT("FileDescription"), TEXT("FileVersion"),
                                TEXT("InternalName"), TEXT("LegalCopyright"), TEXT("LegalTradeMarks"),
                                TEXT("OriginalFilename"), TEXT("PrivateBuild"), TEXT("ProductName"),
                                TEXT("ProductVersion"), TEXT("SpecialBuild")};

                            static Info::Key const parameter_keys[] = {Info::key("author"),
                                Info::key("comments"), Info::key("vendor"), Info::key("description"),
                                Info::key("version"), Info::key("product"), Info::key("copyright"),
                                Info::key("trademark"), Info::key("file.name"),
                                Info::key("private.build"), Info::key("product.name"),
                                Info::key("product.version"), Info::key("special.build")};

                                   // перебираем все кодовые страницы
                            for (size_t index = size_t();
                                index < (translation_count / sizeof(LANGANDCODEPAGE)); ++index)
                            {
                                // перебираем имена параметров
                                for (size_t i = 0; i < sizeof(parameter_names) / sizeof(const TCHAR *); ++i)
                                {
                                    // формируем имя параметра (
                                    // \StringFileInfo\кодовая_страница\имя_параметра )
                                    TCHAR parameter_name_buffer[64];
                                    print(parameter_name_buffer, 64, parameterFormat(),
                                        translation[index].m_language,
                                        translation[index].m_code_page, parameter_names[i]);

                                           // получаем значение параметра
                                    TCHAR * parameter_value = 0;
                                    UINT parameter_size = UINT();
                                    if (VerQueryValue(version_info, parameter_name_buffer,
                                            (LPVOID *)&parameter_value, &parameter_size) &&
                                        parameter_size)
                                    {
                                        if (module.m_info[parameter_keys[i]].empty())
                                        {
                                            module.m_info[parameter_keys[i]] = Info::attribute(parameter_value);
                                        }
                                        else
                                        {
                                            module.m_info[Info::key("file.") + parameter_keys[i]] =
                                                Info::attribute(parameter_value);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                if (product.empty())
                {
                    product = module.m_info[Info::key("product")];
                }

                if (product.empty())
                {
                    product = module.m_info[Info::key("product")] = Info::attribute(module_name);
                }

                m_runtime.m_modules[product] = module;
                m_module_for_handle[handle] = &m_runtime.m_modules[product];

                PIMAGE_DOS_HEADER dos_header = PIMAGE_DOS_HEADER(handle);
                if (dos_header->e_magic != IMAGE_DOS_SIGNATURE)
                {
                    return;
                }

                PIMAGE_NT_HEADERS nt_headers = PIMAGE_NT_HEADERS(LPBYTE(dos_header) + dos_header->e_lfanew);
                if (nt_headers->Signature != IMAGE_NT_SIGNATURE)
                {
                    return;
                }

                DWORD virtual_address = nt_headers->OptionalHeader.DataDirectory[1].VirtualAddress;
                if (virtual_address != DWORD())
                {
                    PIMAGE_IMPORT_DESCRIPTOR image_import_descriptor =
                        PIMAGE_IMPORT_DESCRIPTOR(LPBYTE(dos_header) + virtual_address);
                    for (int i = 0; image_import_descriptor[i].Characteristics != DWORD(); ++i)
                    {
                        LPCSTR submodule_name = LPCSTR(LPBYTE(dos_header) +
                            image_import_descriptor[i].Name);
                        init(TSTRING(submodule_name, submodule_name + strlen(submodule_name)));
                    }
                }
            }
            else
            {
                module_hiter->second->m_aliases.insert(Info::attribute(name));
            }
        }

        ::AutoVersion::RuntimeVersion runtimeVersion() { return m_runtime; }
    };
} // namespace AutoVersion

#endif
