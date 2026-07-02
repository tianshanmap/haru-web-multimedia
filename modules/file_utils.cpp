#include <filesystem>
#include <iostream>
#include <random>
#include <sstream>
#include <fstream>
#include "file_utils.h"

namespace fs = std::filesystem;
namespace haru {
    std::string getCurrentTime_as_string() {
        // 1. Get the current time point from the system clock
        auto now = std::chrono::system_clock::now();

        // 2. Get the duration since the Unix epoch (January 1, 1970)
        auto duration = now.time_since_epoch();

        // 3. Cast the duration to milliseconds and extract the integer count
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

        // 4. Convert the millisecond numeric value to a std::string
        std::string millis_str = std::to_string(millis);

        // Output the resulting string
        std::cout << "Milliseconds String: " << millis_str << std::endl;

        return millis_str;
    }
    void move_file(std::string source,std::string parent){
        fs::path filePath = source;
        fs::path destination;
        if (filePath.extension() == ".jpg") {
            destination = parent + "/" + filePath.stem().string() + ".jpeg";
        } else {
            destination = parent + "/" + filePath.filename().string();
        }
        std::cout << "move_file::" << source << " to " << destination << std::endl;
        fs::rename(source, destination);
    }
    void clear_folder(std::string path) {
        fs::path folderPath(path);
        for (const auto& entry : fs::directory_iterator(folderPath)) {
            fs::remove(entry.path());
        }  
    }
    void create_folder(std::string name){
        try {
            if (fs::create_directories(name)) {
                std::cout << "Directories created successfully\n";
            } else {
                // std::cout << "Directories already exist\n";
            }
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Error: " << e.what() << '\n';
        }
    }
    void create_folder(std::string parent,std::string name){
        create_folder(parent + "/" + name);
    }
    std::string create_folder_under(std::string pathname,std::string name){
        fs::path filePath(pathname);
        std::string target = filePath.parent_path().string() + "/" + name;
        clear_folder(target);
        create_folder(target);
        return target;
    }
    int process_filesystem_file(std::string path,std::string destination){
        // std::cout << "process_filesystem_file::" << path << std::endl;
        fs::path filePath(path);
        // Get the parent path
        fs::path parent = filePath.parent_path().filename();
        std::string target = destination + "/" + parent.string() + "/jpeg";
        if (path.ends_with(".jpeg") || path.ends_with(".jpg")){
            // std::cout << "process_filesystem_file::jpeg" << path << std::endl;
            std::string target = destination + "/" + parent.string() + "/jpeg";
            create_folder(target);
            move_file(filePath.string(),target);
        } else if (path.ends_with(".mov") || path.ends_with(".mp4")){
            std::cout << "process_filesystem_file::mov=>" << path << std::endl;
            std::string target = destination + "/" + parent.string() + "/mov";
            create_folder(target);
            move_file(filePath.string(),target);
        }
        return 1;
    }

    int process_filesystem_directory(std::string path,std::string destination) {
        // std::string path = "/Users/developer/T9/travels/2026-01-14_2026-03-25/2026-01-16-2026-01-23-singapore-maleka";
        std::cout << "process_filesystem_directory::" << path << std::endl;
        // Validate path

        std::error_code ec; // To handle errors without exceptions
         fs::path folderPath(path);
        std::cout << "\nContents of folder:\n";
        for (const auto& entry : fs::directory_iterator(folderPath, ec)) {
            if (ec) {
                std::cerr << "Error reading directory: " << ec.message() << "\n";
                return 1;
            }
            if (fs::is_directory(entry.path())){
                process_filesystem_directory(entry.path(),destination);
            } else {
                process_filesystem_file(entry.path(),destination);
            }
        }

        return 0;
    }

    int process_filesystem(std::string path,std::string destination){
        // Validate path
        create_folder(destination);
        fs::path folderPath(path);
        if (!fs::exists(folderPath)) {
            std::cerr << "Error: Path does not exist.\n";
            return 1;
        }
        if (fs::is_directory(folderPath)) {
            process_filesystem_directory(path,destination);
        } else {
            process_filesystem_file(path,destination);
        }
        return 1;
    }
    std::vector<std::string> getFiles(std::string path,std::string extension) {
        std::vector<std::string> files;
        for (const auto & entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file() && entry.path().extension() == extension) {
                files.push_back(entry.path().string());
            }
        }
        return files;
    }
    void scanFiles(std::string &path,std::vector<std::string> &files) {
        std::filesystem::path filePath(path);
        for (const auto & entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().string());
            } else if (entry.is_directory()) {
                std::string folder_path = entry.path().string();
                scanFiles(folder_path,files);
            }
        }
    }
    void find_image_directory(std::string path,std::vector<std::string> &files) {
        for (const auto & entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_directory() && entry.path().filename() == "jpeg") {
                files.push_back(entry.path().string());
            } else if (entry.is_directory()){
                find_image_directory(entry.path().string(),files);
            }
        }
    }
    int scan_directory(const std::string path,std::vector<std::string> &files,const std::string extension) {
        // std::string path = "/Users/developer/T9/travels/2026-01-14_2026-03-25/2026-01-16-2026-01-23-singapore-maleka";
        std::cout << "scan_filesystem_directory::" << path << std::endl;
        // Validate path

        std::error_code ec; // To handle errors without exceptions
        fs::path folderPath(path);
        std::cout << "\nContents of folder:\n";
        for (const auto& entry : fs::directory_iterator(folderPath, ec)) {
            if (ec) {
                std::cerr << "Error reading directory: " << ec.message() << "\n";
                return 1;
            }
            if (fs::is_directory(entry.path())){
                scan_directory(entry.path(),files,extension);
            } else {
                if (entry.path().extension() == extension) {
                    files.push_back(entry.path().string());
                }
            }
        }
        return 0;
    }
    void write_to_file(std::string &path,std::ostringstream &buffer) {
        std::ofstream myfile;
        myfile.open (path + "/mov.txt");
        myfile << buffer.str();
        myfile.close();
        buffer.str("");
        buffer.clear();
        std::cout << "write_to_file::" << path << std::endl;
    }
    std::vector<std::string> create_textfile_for_merge(std::vector<std::string> &files) {
        std::vector<std::string> files_parent;
        std::vector<std::string> files_with_same_parent;
        std::filesystem::path currentFilePath(files[0]);
        std::string current_parent = currentFilePath.parent_path().string();
        std::ostringstream buffer;
        for (int i = 0; i < files.size(); i++) {
            std::filesystem::path filePath(files[i]);
            std::string parent = filePath.parent_path().string();
            if (parent != current_parent) {
                files_parent.push_back(parent);
                if (!buffer.str().empty()) {
                    write_to_file(current_parent,buffer);
                }
                current_parent = parent;
            }
            buffer << "file " << "'" << files[i] << "'" << std::endl;
        }
        if (!buffer.str().empty()) {
            write_to_file(current_parent,buffer);
        }
        return files_parent;
    }
    std::string getLastWriteTimeStr(const std::filesystem::path& filePath) {
        try {
            auto fileTime = std::filesystem::last_write_time(filePath);

            // Convert file_time_type to system_clock time_point
            auto systemTime = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                fileTime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
            );

            // Convert to time_t for old C-style time parsing
            std::time_t cTime = std::chrono::system_clock::to_time_t(systemTime);

            // Thread-safe conversion to local time structure
            std::tm localTime;
#if defined(_MSC_VER)
            localtime_s(&localTime, &cTime); // Windows
#else
            localtime_r(&cTime, &localTime); // POSIX/Linux
#endif

            // Format the time structure via a stringstream
            std::ostringstream oss;
            oss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
            return oss.str();

        } catch (...) {
            return "";
        }
    }
    unsigned long get_file_size(const std::filesystem::directory_entry& entry) {
        try {
            return entry.file_size();
        } catch (...) {
            return 0;
        }
    }
    unsigned long get_file_time(const std::filesystem::directory_entry& entry) {
        try {
            return entry.file_size();
        } catch (...) {
            return 0;
        }
    }
    HaruFolder get_folder(std::string path) {
        std::filesystem::path filePath(path);
        HaruFolder folder;
        folder.name = filePath.filename().string();
        folder.parent = filePath.parent_path().string();
        std::vector<HaruFileEntry> folders;
        std::vector<HaruFileEntry> files;
        try {
            for (const auto& entry : fs::directory_iterator(path)) {
                HaruFileEntry harufile;
                harufile.path = entry.path().string();
                // std::cout << "entry.path().filename().string(): " << entry.path().filename().string() << '\n';
                harufile.name = entry.path().filename().string();
                harufile.parent_path = entry.path().parent_path().string();
                if (entry.is_directory()) {
                    harufile.kind = "folder";
                    harufile.size = 0;
                    folders.push_back(harufile);
                } else if (entry.is_regular_file()) {
                    harufile.kind = "file";
                    harufile.size = get_file_size(entry);
                    harufile.last_update = getLastWriteTimeStr(entry);
                    files.push_back(harufile);
                }
            }
            std::sort(folders.begin(), folders.end(), [](const auto& a, const auto& b) {
                return a.name > b.name;
            });
            std::sort(files.begin(), files.end(), [](const auto& a, const auto& b) {
                return a.name > b.name;
            });
            folder.files.insert(folder.files.end(), folders.begin(), folders.end());
            folder.files.insert(folder.files.end(), files.begin(), files.end());
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Error: " << e.what() << '\n';
        }
        return folder;
    }
    std::string get_folder_as_json(std::string path) {
        HaruFolder files = get_folder(path);
        nlohmann::json j = files;
        return j.dump();
    }
    std::string get_files_as_json(std::string path,std::string &&ext) {
        HaruFolder files = get_folder(path);
        std::vector<HaruFileEntry> audio_files = files.files;
        files.files.clear();
        auto audio_files_filter = audio_files | std::views::filter([ext](const auto& file) { return file.kind == "file" && file.name.ends_with(ext); });
        files.files.insert(files.files.end(), audio_files_filter.begin(), audio_files_filter.end());
        nlohmann::json j = files;
        return j.dump();
    }
    std::string get_audio_as_json(std::string path) {
        return get_files_as_json(path,".mp3");
    }
    std::string get_video_as_json(std::string path) {
        return get_files_as_json(path,".mp4");
    }
    std::string get_image_as_json(std::string path) {
        HaruFolder files = get_folder(path);
        std::vector<HaruFileEntry> audio_files = files.files;
        files.files.clear();
        auto audio_files_filter = audio_files | std::views::filter([](const auto& file) { return file.kind == "file" && (file.name.ends_with(".jpeg") || file.name.ends_with(".jpg")); });
        files.files.insert(files.files.end(), audio_files_filter.begin(), audio_files_filter.end());
        nlohmann::json j = files;
        return j.dump();
    }
    std::string read_binary_file_content(std::ifstream &file) {
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::string buffer(size, '\0');
        if (file.read(&buffer[0], size)) {
            return buffer;
        }
        return "";
    }
    std::string read_binary_file(const std::string& file_path_name) {
        fs::path file_path(file_path_name);
        std::ifstream file(file_path, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file: " << file_path << std::endl;
            return "";
        }
        return read_binary_file_content(file);
    }
    std::string get_content_type(std::string &path) {
        std::filesystem::path filePath(path);
        std::cout << "get_content_type::" << path << ": ext=" << filePath.extension() << std::endl;
        if (filePath.extension() == ".jpeg") {
            return "image/jpeg";
        } else if (filePath.extension() == ".mov") {
            return "video/quicktime";
        } else if (filePath.extension() == ".mp4") {
            return "video/mp4";
        } else if (filePath.extension() == ".mp3") {
            return "audio/mpeg";
        } else if (filePath.extension() == ".html") {
            return "text/html";
        } else if (filePath.extension() == ".txt") {
            return "text/html";
        } else if (filePath.extension() == ".json") {
            return "application/json";
        } else if (filePath.extension() == ".xml") {
            return "application/xml";
        } else if (filePath.extension() == ".pdf") {
            return "application/pdf";
        } else if (filePath.extension() == ".zip") {
            return "application/zip";
        } else {
            return "application/octet-stream";
        }
    }
    void create_folder(std::string &name,std::string &path) {
        fs::path folderPath(path + "/" + name);
        if (fs::exists(folderPath)) {
            std::cout << "Folder already exists: " << folderPath << std::endl;
        } else {
            if (fs::create_directories(folderPath)) {
                std::cout << "Folder created successfully: " << folderPath << std::endl;
            } else {
                std::cout << "Failed to create folder: " << folderPath << std::endl;
            }
        }
    }

    void delete_file(std::string &path) {
        fs::path filePath(path);
        try {
            if (fs::remove(filePath)) {
                std::cout << "File " << path << " successfully deleted.\n";
            } else {
                std::cout << "File did not exist.\n";
            }
        } catch (const fs::filesystem_error& err) {
            std::cerr << "Filesystem error: " << err.what() << "\n";
        }
    }
    void delete_folder(std::string &path) {
        fs::path dirPath(path);
        try {
            std::uintmax_t deletedCount = fs::remove_all(dirPath);
            std::cout << "Deleted " << deletedCount << " files/folders.\n";
        } catch (const fs::filesystem_error& err) {
            std::cerr << "Deletion failed: " << err.what() << "\n";
        }
    }
    void delete_folder_recursive(std::string path) {
        fs::path folderPath(path);
        for (const auto& entry : fs::recursive_directory_iterator(folderPath)) {
            fs::remove(entry.path());
        }
    }
    void copy(std::string &path,std::string &target) {
        fs::path src(path);
        try {
            // Move and/or rename the file
            const auto copyOptions = fs::copy_options::recursive
                                   | fs::copy_options::overwrite_existing;
            fs::path targetDir(target);
            targetDir = targetDir / src.filename();
            fs::copy(src, targetDir, copyOptions);
            std::cout << "File moved successfully!\n";
        }
        catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Error moving file: " << e.what() << '\n';
        }
    }
    void move(std::string &path,std::string &target) {
        fs::path filePath(path);
        fs::path targetPath(target + "/" + filePath.filename().string());
        try {
            // Move and/or rename the file
            fs::rename(filePath, targetPath);
            std::cout << "File moved successfully!\n";
        }
        catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Error moving file: " << e.what() << '\n';
            copy(path,target);
            if (is_regular_file(filePath)) {
                delete_file(path);
            } else if (is_directory(filePath)) {
                delete_folder(path);
            }
        }
    }
    void rename(std::string &file1,std::string &file2) {
        fs::path filePath1(file1);
        fs::path filePath2(file2);
        fs::rename(filePath1, filePath2);
    }
    std::string get_file_name(std::string &parent_path,std::string file_path) {
        return file_path.substr(parent_path.size() + 1);
    }
    void write_file(std::string &file_path,std::string &content) {
        std::ofstream outputFile(file_path);

        // 2. Check if the file opened successfully
        if (!outputFile.is_open()) {
            std::cerr << "Error: Could not open the file!" << std::endl;
            return;
        }

        // 3. Write text to the file using the stream insertion operator (<<)
        outputFile << content << std::endl;

        // 4. Close the file (optional but good practice)
        outputFile.close();

    }
    std::string read_file(std::string &file_path) {
        std::ifstream inputFile(file_path);

        // 2. Check if the file opened successfully
        if (!inputFile.is_open()) {
            std::cerr << "Error: Could not open the file!" << std::endl;
            return "";
        }

        std::stringstream buffer;

        // Read the entire file buffer into the stream
        buffer << inputFile.rdbuf();

        std::string file_contents = buffer.str();
        inputFile.close();
        return file_contents;
    }
}
