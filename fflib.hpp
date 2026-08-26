/*
             ahh.txt 1kb
              ,_____.
              |     |
              | >:( |
              | ahh |
._________.-----------.
| FFlib.hpp           |
| Friendly(or fuckin) |
| File                |
| Library             |
|                     |
|                     |
`---------------------'
Why are files so bad my fucking god.
What the hell is a stream, and why are we using it for files?
Why is it not for just console.
Streams are so much worse for files.
ahhhhhhhhhhhhhhhhkjldasfhjksdafhuieoh lfngbm,.

F***, what? why did you censor me?
The advertizers will kill me if you continue acting like this.
Fine...
Heres my way of abstracting the horid C++ streams.

Copyright (c) 2026 https://github.com/ladsm

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <string_view>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <format>

namespace fflib {
    namespace fs = std::filesystem;

    inline void debug_log(std::string_view msg) {
        const auto now = std::chrono::system_clock::now();
        const auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm time_info{};
#ifdef _WIN32
        localtime_s(&time_info, &in_time_t);
#else
        localtime_r(&in_time_t, &time_info);
#endif
        std::cerr << "[" << std::put_time(&time_info, "%Y-%m-%d %H:%M:%S") << "] [FFLIB] " << msg << "\n" << std::flush;
    }

    inline bool exists(const fs::path& p) {
        return fs::exists(p);
    }

    inline bool create_file(const fs::path& filepath, std::string_view content) {
        std::ofstream outFile(filepath, std::ios::out | std::ios::binary);
        if (!outFile) {
            debug_log(std::format("File could not be created. File name: {}", filepath.string()));
            return false;
        }
        outFile.write(content.data(), content.size());
        return outFile.good();
    }

    inline bool touch(const fs::path& filepath, std::string_view content) {
        return create_file(filepath, content);
    }

    inline bool delete_file(const fs::path& filepath) {
        std::error_code ec;
        bool result = fs::remove(filepath, ec);
        if (ec) {
            debug_log(std::format("Failed to delete file. File name: {} Error: {}", filepath.string(), ec.message()));
            return false;
        }
        return result;
    }

    inline bool rm(const fs::path& filepath) {
        return delete_file(filepath);
    }

    inline bool delete_dir(const fs::path& dirpath) {
        std::error_code ec;
        bool result = fs::remove(dirpath, ec);
        if (ec) {
            debug_log(std::format("Failed to delete directory. Directory: {} Error: {}", dirpath.string(), ec.message()));
            return false;
        }
        return result;
    }

    inline bool rmdir(const fs::path& dirpath) {
        return delete_dir(dirpath);
    }

    inline bool delete_dir_recursive(const fs::path& dirpath) {
        std::error_code ec;
        std::uintmax_t count = fs::remove_all(dirpath, ec);
        if (ec) {
            debug_log(std::format("Failed to delete directory. Directory: {} Error: {}", dirpath.string(), ec.message()));
            return false;
        }
        return count > 0;
    }

    inline bool rm_rf(const fs::path& dirpath) {
        return delete_dir_recursive(dirpath);
    }

    inline bool create_folder(const fs::path& folderPath) {
        if (fs::exists(folderPath)) {
            return true;
        }
        std::error_code ec;
        bool created = fs::create_directories(folderPath, ec);
        if (ec) {
            debug_log(std::format("Directory could not be made. Folder name: {} Error: {}", folderPath.string(), ec.message()));
            return false;
        }
        return created;
    }

    inline bool mkdir(const fs::path& folderPath) {
        return create_folder(folderPath);
    }

    inline bool change_directory(const fs::path& directory_path) {
        std::error_code ec;
        fs::current_path(directory_path, ec);
        if (ec) {
            debug_log(std::format("Failed to change directory. Directory name: {}. Error: {}", directory_path.string(), ec.message()));
            return false;
        }
        return true;
    }

    inline bool cd(const fs::path& directory_path) {
        return change_directory(directory_path);
    }

    inline bool cddotdot() {
        return change_directory("..");
    }

    inline std::string get_file_contents(const fs::path& filepath) {
        std::ifstream inFile(filepath, std::ios::in | std::ios::binary);
        if (!inFile) {
            debug_log(std::format("File could not be opened. File name: {}", filepath.string()));
            return "";
        }
        return std::string((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    }

    inline std::string cat(const fs::path& filepath) {
        return get_file_contents(filepath);
    }

    inline std::vector<std::string> list_dir(const fs::path& dirpath = ".") {
        std::vector<std::string> entries;
        std::error_code ec;

        if (!fs::exists(dirpath, ec) || !fs::is_directory(dirpath, ec)) {
            debug_log(std::format("Failed to list directory. Directory: {} Error: {}", dirpath.string(), ec ? ec.message() : "Not a directory"));
            return entries;
        }

        for (const auto& entry : fs::directory_iterator(dirpath, ec)) {
            if (ec) {
                debug_log(std::format("Error reading directory entry in: {} Error: {}", dirpath.string(), ec.message()));
                break;
            }
            entries.push_back(entry.path().filename().string());
        }

        return entries;
    }

    inline std::vector<std::string> ls(const fs::path& dirpath = ".") {
        return list_dir(dirpath);
    }

    inline bool append_at_point_file(const fs::path& filepath, std::string_view find, std::string_view appended_text) {
        std::string contents = get_file_contents(filepath);
        if (contents.empty() && !fs::exists(filepath)) {
            return false;
        }
        size_t pos = contents.find(find);
        if (pos == std::string::npos) {
            debug_log(std::format("Target text not found in file. File name: {}", filepath.string()));
            return false;
        }
        contents.insert(pos + find.length(), appended_text);
        std::ofstream outFile(filepath, std::ios::out | std::ios::binary);
        if (!outFile) {
            debug_log(std::format("Failed to write updates to file. File name: {}", filepath.string()));
            return false;
        }
        outFile.write(contents.data(), contents.size());
        return outFile.good();
    }

    inline bool prepend_at_point_file(const fs::path& filepath, std::string_view find, std::string_view prepended_text) {
        std::string contents = get_file_contents(filepath);
        if (contents.empty() && !fs::exists(filepath)) {
            return false;
        }
        size_t pos = contents.find(find);
        if (pos == std::string::npos) {
            debug_log(std::format("Target text not found in file. File name: {}", filepath.string()));
            return false;
        }
        contents.insert(pos, prepended_text);
        std::ofstream outFile(filepath, std::ios::out | std::ios::binary);
        if (!outFile) {
            debug_log(std::format("Failed to write updates to file. File name: {}", filepath.string()));
            return false;
        }
        outFile.write(contents.data(), contents.size());
        return outFile.good();
    }

    inline bool find_and_replace_file(const fs::path& filepath, std::string_view find, std::string_view replace_with) {
        std::string contents = get_file_contents(filepath);
        if (contents.empty() && !fs::exists(filepath)) {
            return false;
        }
        size_t pos = contents.find(find);
        if (pos == std::string::npos) {
            debug_log(std::format("Target text not found in file. File name: {}", filepath.string()));
            return false;
        }
        while (pos != std::string::npos) {
            contents.replace(pos, find.length(), replace_with);
            pos = contents.find(find, pos + replace_with.length());
        }

        std::ofstream outFile(filepath, std::ios::out | std::ios::binary);
        if (!outFile) {
            debug_log(std::format("Failed to write updates to file. File name: {}", filepath.string()));
            return false;
        }
        outFile.write(contents.data(), contents.size());
        return outFile.good();
    }

    inline bool sed(const fs::path& filepath, std::string_view find, std::string_view replace_with) {
        return find_and_replace_file(filepath, find, replace_with);
    }

    inline bool append_file(const fs::path& filepath, std::string_view content) {
        std::ofstream outFile(filepath, std::ios::out | std::ios::binary | std::ios::app);
        if (!outFile) {
            debug_log(std::format("File could not be opened for appending. File name: {}", filepath.string()));
            return false;
        }
        outFile.write(content.data(), content.size());
        return outFile.good();
    }

    inline bool prepend_file(const fs::path& filepath, std::string_view content) {
        std::string contents = get_file_contents(filepath);
        if (contents.empty() && !fs::exists(filepath)) {
            return create_file(filepath, content);
        }
        contents.insert(0, content);
        std::ofstream outFile(filepath, std::ios::out | std::ios::binary);
        if (!outFile) {
            debug_log(std::format("Failed to write prepend updates to file. File name: {}", filepath.string()));
            return false;
        }
        outFile.write(contents.data(), contents.size());
        return outFile.good();
    }

    inline bool clear_file(const std::filesystem::path& filepath) {
        std::ofstream file(filepath, std::ios::out | std::ios::trunc);
        if (!file) {
            debug_log(std::format("Could not open/clear file. File name: {}", filepath.string()));
            return false;
        }
        return file.good();
    }

    inline std::string current_path() {
        std::error_code ec;
        auto p = fs::current_path(ec);
        if (ec) {
            debug_log(std::format("Failed to get current path. Error: {}", ec.message()));
            return "";
        }
        return p.string();
    }

}

/*
hey








hey





me





over here











put me in your precompiled header so your whatever compiles faster.
ie CMake: target_precompile_headers(${PROJECT_NAME} PRIVATE include/pch.h)

put me as an include, yeah, yeah you're following along.

then compile and then make every file that uses me to include pch.h insted of fflib.hpp
*/