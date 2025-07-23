#include "File_manager.hpp"
#include <filesystem>

using namespace std;

bool FileManager::file_existance(const string& path)
{
    if (ifstream(path))
    {
        cout << "Файл с таким именем уже существует!" << endl;
        return true;
    }
    else
        return false;
}

bool FileManager::file_not_created(const string& path)
{
    if (ifstream(path))
    {
        return false;
    }
    else
    {
        cout << endl;
        return true;
    }
}

bool FileManager::is_forbidden_file_name(const string& fileName) {
    regex forbiddenNames("(CON|NUL|AUX|PRN|COM\\d|LPT\\d)(\\..*)?", regex_constants::icase);
    return regex_match(fileName, forbiddenNames);
}

bool FileManager::is_valid_filename(const string& path)
{
    string filename = filesystem::path(path).filename().string();

    if (filename.empty()) {
        cout << "Пустое имя файла недопустимо!" << endl;
        return false;
    }

    if (is_forbidden_file_name(filename)) {
        cout << "Имя файла запрещено в Windows: " << filename << endl;
        return false;
    }

    // Проверка на недопустимые символы
    regex invalidChars(R"([<>:"/\\|?*])");
    if (regex_search(filename, invalidChars)) {
        cout << "Имя файла содержит недопустимые символы: " << filename << endl;
        return false;
    }

    return true;
}