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
    inline void debug_log(std::string_view msg) { //riped from another project of mine because im lazy
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
    /*
    some functions have aliases like create_file as touch.
    this is for convenience.
    */
    inline void create_file(const fs::path& filepath, std::string_view content) {
        std::ofstream outFile(filepath, std::ios::out | std::ios::binary);
        if (!outFile) {
            debug_log(std::format("File could not be created. File name: {}", filepath.string())); 
            // im using std::format cuz like it better then normal strings with +
            return;
        }
        outFile.write(content.data(), content.size());
    } 
    inline void touch(const fs::path& filepath, std::string_view content) { create_file(filepath, content); }

    inline void delete_file(const fs::path& filepath) {
        std::error_code ec;
        fs::remove(filepath, ec);
        if (ec) {
            debug_log(std::format("Failed to delete file. File name: {} Error: {}", filepath.string(), ec.message()));
            return;
        }
        return;
    }
    inline void rm(const fs::path& filepath) {
        return delete_file(filepath);
    }
    inline void delete_dir(const fs::path& dirpath) {
        std::error_code ec;
        fs::remove(dirpath, ec);
        if (ec) {
            debug_log(std::format("Failed to delete directory. Directory: {} Error: {}", dirpath.string(), ec.message()));
        }
    }
    inline void rmdir(const fs::path& dirpath) { delete_dir(dirpath); }

    inline void delete_dir_recursive(const fs::path& dirpath) {
        std::error_code ec;
        fs::remove_all(dirpath, ec);
        if (ec) {
            debug_log(std::format("Failed to delete directory. Directory: {} Error: {}", dirpath.string(), ec.message()));
        }
    }
    inline void rm_rf(const fs::path& dirpath) { delete_dir_recursive(dirpath); }

    inline void create_folder(const fs::path& folderPath) {
        if (fs::exists(folderPath)) {
            return;
        }
        std::error_code ec;
        if (fs::create_directories(folderPath, ec)) {
            return;
        }
        if (ec) {
            debug_log(std::format("Directory could not be made. Folder name: {} Error: {}", folderPath.string(), ec.message()));
        }
        else {
            debug_log(std::format("Directory already exists. Folder name: {}", folderPath.string()));
        }
    }
    inline void mkdir(const fs::path& folderPath) { create_folder(folderPath); }

    inline void change_directory(const fs::path& directory_path) {
        std::error_code ec;
        fs::current_path(directory_path, ec);
        if (ec) {
            debug_log(std::format("Failed to change directory. Directory name: {}. Error: {}", directory_path.string(), ec.message()));
        }
    }
    inline void cd(const fs::path& directory_path) { change_directory(directory_path); }
    inline void cddotdot() { change_directory(".."); }

    inline std::string get_file_contents(const fs::path& filepath) {
        std::ifstream inFile(filepath, std::ios::in | std::ios::binary);
        if (!inFile) {
            debug_log(std::format("File could not be opened. File name: {}", filepath.string()));
            return "";
        }
        return std::string((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    }
    inline std::string cat(const fs::path& filepath) { return get_file_contents(filepath); }

    inline void append_at_point_file(const fs::path& filepath, std::string_view find, std::string_view appended_text) {
        std::string contents = get_file_contents(filepath);
        if (contents.empty() && !fs::exists(filepath)) {
            return;
        }
        size_t pos = contents.find(find);
        if (pos == std::string::npos) {
            debug_log(std::format("Target text not found in file. File name: {}", filepath.string()));
            return;
        }
        contents.insert(pos + find.length(), appended_text);
        std::ofstream outFile(filepath, std::ios::out | std::ios::binary);
        if (!outFile) {
            debug_log(std::format("Failed to write updates to file. File name: {}", filepath.string()));
            return;
        }
        outFile.write(contents.data(), contents.size());
    }
    inline void prepend_at_point_file(const fs::path& filepath, std::string_view find, std::string_view prepended_text) {
        std::string contents = get_file_contents(filepath);
        if (contents.empty() && !fs::exists(filepath)) {
            return;
        }
        size_t pos = contents.find(find);
        if (pos == std::string::npos) {
            debug_log(std::format("Target text not found in file. File name: {}", filepath.string()));
            return;
        }
        contents.insert(pos, prepended_text);
        std::ofstream outFile(filepath, std::ios::out | std::ios::binary);
        if (!outFile) {
            debug_log(std::format("Failed to write updates to file. File name: {}", filepath.string()));
            return;
        }
        outFile.write(contents.data(), contents.size());
    }
    inline void find_and_replace_file(const fs::path& filepath, std::string_view find, std::string_view replace_with) {
        std::string contents = get_file_contents(filepath);
        if (contents.empty() && !fs::exists(filepath)) {
            return;
        }
        size_t pos = contents.find(find);
        if (pos == std::string::npos) {
            debug_log(std::format("Target text not found in file. File name: {}", filepath.string()));
            return;
        }
        while (pos != std::string::npos) {
            contents.replace(pos, find.length(), replace_with);
            pos = contents.find(find, pos + replace_with.length());
        }

        std::ofstream outFile(filepath, std::ios::out | std::ios::binary);
        if (!outFile) {
            debug_log(std::format("Failed to write updates to file. File name: {}", filepath.string()));
            return;
        }
        outFile.write(contents.data(), contents.size());
    }
    inline void sed(const fs::path& filepath, std::string_view find, std::string_view replace_with) {
        find_and_replace_file(filepath, find, replace_with);
    }//THESE ARE SHIIIIIIT pre/appeed and find and replace. All of the file data is loaded into memory so use this for like 5KB files
     //and not 5GB log files(why whould you change a log file like this?).

    inline void append_file(const fs::path& filepath, std::string_view content) {
        std::ofstream outFile(filepath, std::ios::out | std::ios::binary | std::ios::app);
        if (!outFile) {
            debug_log(std::format("File could not be opened for appending. File name: {}", filepath.string()));
            return;
        }
        outFile.write(content.data(), content.size());
    }
    inline void prepend_file(const fs::path& filepath, std::string_view content) { // aswell this is ass but it does the job ig
        std::string contents = get_file_contents(filepath);
        if (contents.empty() && !fs::exists(filepath)) {
            create_file(filepath, content);
            return;
        }
        contents.insert(0, content);
        std::ofstream outFile(filepath, std::ios::out | std::ios::binary);
        if (!outFile) {
            debug_log(std::format("Failed to write prepend updates to file. File name: {}", filepath.string()));
            return;
        }
        outFile.write(contents.data(), contents.size());
    }

    inline void clear_file(const std::filesystem::path& filepath) {
        std::ofstream file(filepath, std::ios::out | std::ios::trunc);
        if (!file) {
            debug_log(std::format("Could not open/clear file. File name: {}", filepath.string()));
        }
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

} // namespace fflib

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
