#pragma once
#ifndef AUTO_VERSION_APPLICATION_UNIX_IMPL
#define AUTO_VERSION_APPLICATION_UNIX_IMPL

#include <AutoVersion/Module.h>

#include <set>
#include <string>

#include <dlfcn.h>
#include <fcntl.h>
#include <link.h>
#include <sys/stat.h>
#include <unistd.h>

namespace AutoVersion
{
    extern ::AutoVersion::Info info();

    inline std::wstring widen(std::string const & s) { return std::wstring(s.begin(), s.end()); }
    inline std::string narrow(std::wstring const & ws) { return std::string(ws.begin(), ws.end()); }

    inline std::wstring fileNameFromPath(std::wstring const & path)
    {
        if (path.empty())
            return std::wstring();
        size_t pos = path.find_last_of(L'/');
        return (pos == std::wstring::npos) ? path : path.substr(pos + 1);
    }

    inline std::wstring moduleNameFromPath(std::wstring const & path)
    {
        if (path.empty())
            return std::wstring();
        size_t pos = path.find_last_of(L'/');
        std::wstring result = (pos == std::wstring::npos) ? path : path.substr(pos + 1);
        pos = result.find_first_of(L'.');
        result = (pos == std::wstring::npos) ? result : result.substr(0, pos);
        if (result.find(L"lib") == 0)
            result = result.substr(3);

        return result;
    }

    inline std::wstring parseVersionFromFilename(std::wstring const & path)
    {
        // Look for ".so." pattern
        size_t pos = path.find(L".so.");
        if (pos == std::wstring::npos)
            return {};
        std::wstring ver = path.substr(pos + 4);
        // Trim directory if path includes directories
        size_t slashPos = ver.find_last_of(L'/');
        if (slashPos != std::wstring::npos)
            ver = ver.substr(slashPos + 1);
        // Keep numeric/dots only simple heuristic
        std::wstring filtered;
        for (wchar_t c : ver)
            if ((c >= L'0' && c <= L'9') || c == L'.')
                filtered.push_back(c);
            else
                break;
        return filtered;
    }

    struct Application
    {
        RuntimeVersion m_runtime;
        std::set<std::wstring> m_seen_paths;

        Application() { init(); }
        RuntimeVersion runtimeVersion() { return m_runtime; }

        void init() { dl_iterate_phdr(&phdrCallback, this); }

        static int phdrCallback(struct dl_phdr_info * info, size_t /*size*/, void * data)
        {
            Application * self = reinterpret_cast<Application *>(data);
            Module module;
            Info::Attribute product;

            std::string path = info->dlpi_name ? info->dlpi_name : "";
            std::wstring wpath = widen(path);
            if (path.empty())
            {
                char exe[4096];
                ssize_t r = ::readlink("/proc/self/exe", exe, sizeof(exe) - 1);
                if (r > 0)
                {
                    exe[r] = '\0';
                    path = exe;
                    wpath = widen(path);
                }

                module.m_info = ::AutoVersion::info();
                product = module.m_info[Info::key("product")];
            }
            else
            {
                module.m_aliases.insert(Info::attribute(fileNameFromPath(wpath)));
            }

            if (self->m_seen_paths.count(wpath))
                return 0; // already processed

            self->m_seen_paths.insert(wpath);

            std::wstring module_name = moduleNameFromPath(wpath);

            void * handle = dlopen(path.c_str(), RTLD_NOW | RTLD_NOLOAD);
            if (!handle)
                handle = dlopen(path.c_str(), RTLD_NOW);

            if (handle)
            {
                std::string version_function_name = "runtimeInfoAbout_" + narrow(module_name);
                typedef ::AutoVersion::Info (*InfoMethod)();

                InfoMethod info_method = reinterpret_cast<InfoMethod>(dlsym(handle,
                    version_function_name.c_str()));

                if (info_method)
                {
                    module.m_info = info_method();
                    product = module.m_info[Info::key("product")];
                }
                else if (module_name.find(L"Qt") == 0)
                {
                    typedef char const * (*QtVersionMethod)();
                    QtVersionMethod qt_version_method = reinterpret_cast<QtVersionMethod>(dlsym(handle,
                        "qVersion"));
                    if (qt_version_method)
                    {
                        if (module.m_info[Info::key("revision")].empty())
                            module.m_info[Info::key("revision")] = Info::attribute(qt_version_method());
                        if (module.m_info[Info::key("version")].empty())
                            module.m_info[Info::key("version")] = Info::attribute(qt_version_method());
                    }
                }
            }

            if (product.empty())
                product = module.m_info[Info::key("product")];
            if (product.empty())
                product = module.m_info[Info::key("product")] = Info::attribute(module_name);

            if (!wpath.empty())
                module.m_info[Info::key("path")] = Info::attribute(wpath);

                   // version from filename
            if (module.m_info[Info::key("version")].empty())
            {
                std::wstring ver = parseVersionFromFilename(wpath);
                if (!ver.empty())
                    module.m_info[Info::key("version")] = Info::attribute(ver);
            }

            self->m_runtime.m_modules[product] = module;
            return 0;
        }
    };

} // namespace AutoVersion

#endif // AUTO_VERSION_APPLICATION_UNIX_IMPL
