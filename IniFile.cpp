#include "IniFile.h"
#include <fstream>
#include <iostream>
using namespace std;

void check_section(string &line) {
    // В секции точно есть [], т.е. больше двух символов
    if (line.length() <= 2) {
        line.erase(0, line.length());
    }
    // Убираем секции вида ...[] и []...
    if ((line[0] != '[') || (line[line.length() - 1] != ']')) {
        line.erase(0, line.length());
    }
    // Убираем секции с ][
    if (line.find(']') < line.find('[')) {
        line.erase(0, line.length());
    }
    // Убираем секции, в которых много скобок
    // Изначально присваиваем единички, т.к. мы знаем, что в строке есть [] из
    // функции is_section, причем первый символ - [, последний - ]. Иначе мы бы
    // обрезали строку в условии выше
    int open_brackets = 1, close_brackets = 1;
    for (size_t i = 1; i < line.length() - 1; i++) {
        if (line[i] == '[') {
            open_brackets++;
        }
        if (line[i] == ']') {
            close_brackets++;
        }
        // В секции должны быть только буквы, цифры и точки
        if ((!isalpha(line[i]) && !isdigit(line[i]) && line[i] != '.')) {
            line.erase(0, line.length());
            break;
        }
        if ((close_brackets != 1) || (open_brackets != 1)) {
            line.erase(0, line.length());
            break;
        }
    }
}

//Проверка, является ли строка секцией
int is_section(const string line) {
    if ((line.find('[') != string::npos) || (line.find(']') != string::npos)) {
        return 1; // Это секция, т.к. есть []
    }
    return 2; // Строка находится внутри секции
}

// Удаляем все пробелы в строке
void delete_spaces(string &line) {
    int i;
    while (line.find(' ') != string::npos) {
        i = line.find(' ');
        line.erase(i, 1);
    }
}

// Удаляем комментарии в строке
void delete_comments(string &line) {
    int i;
    while (line.find(';') != string::npos) {
        i = line.find(';');
        line.erase(i, line.length() - i);
    }
}

// Проверяем, является ли строка корректным ключом
void check_key(string &line) {
    // В начале или в конце строки стоит '='
    if ((line[0] == '=') || (line[line.length() - 1] == '=')) {
        line.erase(0, line.length());
    }
    // Пройдёмся по строке, проверим, сколько в ней знаков '=', есть ли в ней
    // что-то, кроме букв, цифр и точек
    int tmp = 0;
    for (size_t i = 0; i < line.length(); i++) {
        if (line[i] == '=') {
            tmp++;
        }
        if ((!isalpha(line[i])) && (!isdigit(line[i])) && (line[i] != '=') &&
            (line[i] != '.')) {
            line.erase(0, line.length());
            break;
        }
    }
    if (tmp != 1) {
        line.erase(0, line.length());
    }
}

// Обрабатываем строку и возвращаем тип строки
// 0 - пустая
// 1 - секция
// 2 - значение внутри секции
int check_line(string &line) {
    delete_spaces(line);   // Удаляем все пробелы в строке
    delete_comments(line); // Удаляем комментарии
    if (line.empty()) {
        return 0; // Строка пустая
    }
    int tmp = is_section(line);
    if (tmp == 1) { // Это секция
        check_section(line);

    } else if (tmp == 2) { //Внутри секции
        check_key(line);
    }

    if (line.empty())
        return 0; // Строка пустая
    else if (tmp == 1)
        return 1; // Строка - секция
    else
        return 2; // Строка - значение внутри секции
}

// Получаем имя секции, обрезая []
string &section_name(string &line) {
    line.erase(0, 1);
    line.erase(line.size() - 1, 1);
    return line;
}

IniFile::IniFile(const string &path) : filePath(path) {
    ifstream f(filePath);
    if (f.is_open()) {
        string line, section, key, value;
        KeysMap keys; // Временный контейнер с ключами и значениями
        SectionsMap sections;
        // Флаг нам нужен, чтобы не начать записывать пустоту до первой секции
        // куда-то в космос.
        bool flag = false;
        int line_type;
        // Пока в не дошли до конца файла, читаем его
        while (getline(f, line)) {
            line_type = check_line(line); // определяем тип строки
            // Это секция
            if (line_type == 1) {
                // Если это не первая секция, которую мы нашли, то
                if (flag) {
                    // Если мы ничего не попутали и она действительно есть
                    if (data.find(section) != data.end()) {
                        // Проходимся по всей секции
                        for (const auto &element : data[section]) {
                            // Если у нас есть ключ, которого до этого не было
                            if (keys.find(element.first) == keys.end()) {
                                // То записываем значение по этому ключу в нашу секцию
                                keys[element.first] = element.second;
                            }
                        }
                    }
                    // Вываливаем временный контейнер в нашу data
                    data[section] = keys;
                    // Очищаем временный контейнер
                    keys.clear();
                }
                // Запоминаем имя секции, в которую зашли
                section = section_name(line);
                if (!flag) {
                    flag = true;
                }
            }
            //Это ключ со значением
            if (line_type == 2) {
                int tmp = line.find('=');
                // Разделяем строку на ключ и значение
                key = line.substr(0, tmp);
                value = line.substr(tmp + 1, line.length());
                // Добавляем во временный контейнер значение по ключу
                keys[key] = value;
                // Очищаем наши временные ключ и значение
                key.clear();
                value.clear();
            }
        }
        // Записываем последнюю секцию, т.к. в while() мы делаем этого
        if (data.find(section) != data.end()) {
            for (const auto &element : data[section]) {
                if (keys.find(element.first) == keys.end()) {
                    keys[element.first] = element.second;
                }
            }
        }
        data[section] = keys;
        keys.clear();
    }
}

/// Деструктор
/// @note Должен перезаписывать файл кешированой конифгурацией
IniFile::~IniFile() {
    save();
}

/// Запись кешированной конфигурации в файл
void IniFile::save() {
    ofstream file(filePath);
    if (file.is_open()) {
        for (const auto &sections : data) {
            file << '[' << sections.first << ']' << '\n';
            for (const auto &keys : sections.second) {
                file << keys.first << '=' << keys.second << '\n';
            }
        }
    } else {
        throw "Error";
    }
}

/// Чтение значения типа int
/// @param section Секция ini-файла
/// @param key Ключ в секции
/// @param def Значение по умолчанию, возвращаемое в случае отсутсвия
/// ключа/секции
int IniFile::readInt(const string &section, const string &key, int def) {
    if ((isSectionExist(section) == true) &&
        (isKeysExist(section, key) == true)) {
        return stoi(data[section][key]);
    }
    return def;
}

// Чтение значения типа double
double IniFile::readDouble(const string &section, const string &key,
                           double def) {
    if ((isSectionExist(section) == true) &&
        (isKeysExist(section, key) == true)) {
        return stod(data[section][key]);
    }
    return def;
}

// Чтение значения типа std::string
string IniFile::readString(const std::string &section, const std::string &key,
                           const std::string &def) {
    if ((isSectionExist(section) == true) && (isSectionExist(section) == true)) {
        return data[section][key];
    }
    return def;
}

// Чтение значения типа bool
// При чтении строка должна сопоставляться со всеми значениям из trueValues,
// независимо от регистра В остальные случаях - вовзращается false
bool IniFile::readBool(const std::string &section, const std::string &key,
                       bool def) {
    if (isSectionExist(section) && isKeysExist(section, key)) {
        string str = data[section][key];
        for (auto element : trueValues) {
            if (str == element) {
                return true;
            }
        }
    }
    return def;
}

// Запись значения типа int
void IniFile::writeInt(const std::string &section, const std::string &key,
                       int value) {
    data[section][key] = to_string(value);
}

// Запись значения типа double
void IniFile::writeDouble(const std::string &section, const std::string &key,
                          double value) {
    data[section][key] = to_string(value);
}

// Запись значения типа std::string
void IniFile::writeString(const std::string &section, const std::string &key,
                          const std::string &value) {
    data[section][key] = value;
}

// Наша вспомогательная функция
string boolToString(bool value) { return value ? "true" : "false"; }

// Запись значения типа bool
void IniFile::writeBool(const std::string &section, const std::string &key,
                        bool value) {
    data[section][key] = boolToString(value);
}

// Проверка существования секции
bool IniFile::isSectionExist(const std::string &section) {
    return data.find(section) != data.end();
}

// Проверка существования ключа в секции
bool IniFile::isKeysExist(const std::string &section, const std::string &key) {
    if (data.find(section) != data.end()) {
        return data[section].find(key) != data[section].end();
    }
    return false;
}

// Получение количества секций
size_t IniFile::getSectionsCount() { return data.size(); }

// Получение количества ключей в секции
size_t IniFile::getKeysCount(const std::string &section) {
    if (isSectionExist(section)) {
        return data[section].size();
    } else {
        return 0;
    }
}

// Получение значений всех секций
SectionsMap IniFile::getSections() const { return data; }

// Удаление секции
bool IniFile::deleteSection(const string &section) {
    if (data.find(section) == data.end()) {
        return false;
    }
    return data.erase(section) == 1;
}

// Удаление ключа из секции
bool IniFile::deleteKey(const std::string &section, const std::string &key) {
    if (isSectionExist(section)) {
        return data[section].erase(key) == 1;
    } else {
        return false;
    }
}

// Добавление новой секции
bool IniFile::addNewSection(const std::string &section) {
    if (isSectionExist(section) == false) {
        data[section] = {};
        return true;
    }
    return false;
}
