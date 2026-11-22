#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <limits>
#include <algorithm>
#include <cctype>
#include <cstdlib>
using namespace std;
namespace fs = std::filesystem;
const string folder = "documents";
vector<string> lines;
vector<vector<string>> undoList;
vector<vector<string>> redoList;
vector<string> savedLines;
string currentFile = "";
bool opened = false;
bool changed = false;
// Clear the console screen.
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
void waitEnter() {
    cout << "\nPress Enter to continue...";
    string temp;
    getline(cin, temp);
}
string getExtension(string name) {
    string ext = fs::path(name).extension().string();
    for (int i = 0; i < (int)ext.length(); i++) {
        ext[i] = (char)tolower((unsigned char)ext[i]);
    }
    return ext;
}
bool isAllowedName(string name) {
    if (name.empty() || name == "." || name == "..")
        return false;
    for (int i = 0; i < (int)name.length(); i++) {
        char c = name[i];
        if (!isalnum((unsigned char)c) && c != '_' && c != '-' && c != '.') {
            return false;
        }
    }
    return name.find("..") == string::npos;
}
bool isSupported(string ext) {
    vector<string> types = {
        ".txt", ".log", ".md", ".markdown", ".csv",
        ".cpp", ".cc", ".cxx", ".c", ".h", ".hpp",
        ".py", ".java", ".js", ".html", ".css",
        ".json", ".xml", ".sql"
    };
    return find(types.begin(), types.end(), ext) != types.end();
}
bool isCodeFile(string ext) {
    return ext == ".cpp" || ext == ".cc" || ext == ".cxx" ||
           ext == ".c" || ext == ".py" || ext == ".java" ||
           ext == ".js";
}
string languageName(string ext) {
    if (ext == ".cpp" || ext == ".cc" || ext == ".cxx") {
        return "C++";
    }
    if (ext == ".c") {
        return "C";
    }
    if (ext == ".py") {
        return "Python";
    }
    if (ext == ".java") {
        return "Java";
    }
    if (ext == ".js") {
        return "JavaScript";
    }
    if (ext == ".html") {
        return "HTML";
    }
    if (ext == ".css") {
        return "CSS";
    }
    if (ext == ".sql") {
        return "SQL";
    }
    if (ext == ".md" || ext == ".markdown") {
        return "Markdown";
    }
    if (ext == ".csv") {
        return "CSV";
    }
    if (ext == ".json") {
        return "JSON";
    }
    if (ext == ".xml") {
        return "XML";
    }
    if (ext == ".h" || ext == ".hpp") {
        return "Header file";
    }
    return "Text file";
}
string readText(string message) {
    cout << message;
    string value;
    getline(cin, value);
    return value;
}
int readNumber(string message) {
    string input;
    cout << message;
    getline(cin, input);
    try {
        size_t used = 0;
        int number = stoi(input, &used);
        if (used == input.size())
            return number;
    }
    catch (...) {
    }
    return -1;
}
void updateChanged() {
    changed = (lines != savedLines);
}
void rememberChange() {
    undoList.push_back(lines);
    if (undoList.size() > 30)
        undoList.erase(undoList.begin());
    redoList.clear();
}
void saveFile() {
    if (!opened)
        return;
    ofstream file(currentFile);
    if (!file) {
        cout << "Could not save this file.\n";
        return;
    }
    for (size_t i = 0; i < lines.size(); i++) {
        file << lines[i];
        if (i + 1 < lines.size())
            file << '\n';
    }
    savedLines = lines;
    updateChanged();
    cout << "File saved.\n";
}
void loadFile(string path) {
    lines.clear();
    undoList.clear();
    redoList.clear();
    ifstream file(path);
    string line;
    while (getline(file, line))
        lines.push_back(line);
    currentFile = path;
    savedLines = lines;
    opened = true;
    changed = false;
}
void viewFile() {
    clearScreen();
    cout << "--- "
         << fs::path(currentFile).filename().string()
         << " ---\n\n";
    if (lines.empty())
        cout << "(Empty file)\n";
    for (size_t i = 0; i < lines.size(); i++)
        cout << i + 1 << "  " << lines[i] << '\n';
}
void editFile() {
    clearScreen();
    cout << "EDIT FILE: "
         << fs::path(currentFile).filename().string() << "\n";
    cout << "This option replaces the full content.\n";
    cout << "Type END on a separate line when finished.\n\n";
    vector<string> newLines;
    string line;
    while (getline(cin, line) && line != "END")
        newLines.push_back(line);
    if (!cin) {
        cin.clear();
        cout << "Input ended. No changes were made.\n";
        return;
    }
    if (newLines != lines) {
        rememberChange();
        lines = newLines;
        updateChanged();
        cout << "Changes made. Save the file to keep them.\n";
    }
    else {
        cout << "No changes made.\n";
    }
}
void addText() {
    clearScreen();
    cout << "ADD TEXT TO END OF FILE\n";
    cout << "Type END on a separate line when finished.\n\n";
    vector<string> extraLines;
    string line;
    while (getline(cin, line) && line != "END")
        extraLines.push_back(line);
    if (!cin) {
        cin.clear();
        cout << "Input ended. No changes were made.\n";
        return;
    }
    if (!extraLines.empty()) {
        rememberChange();
        for (int i = 0; i < (int)extraLines.size(); i++) {
            lines.push_back(extraLines[i]);
        }
        updateChanged();
        cout << "Text added.\n";
    }
    else {
        cout << "Nothing was added.\n";
    }
}
void insertLine() {
    clearScreen();
    int position = readNumber("Insert before which line number? ");
    if (position < 1 || position > (int)lines.size() + 1) {
        cout << "Invalid line number.\n";
        return;
    }
    string text = readText("Enter the new line: ");
    rememberChange();
    lines.insert(lines.begin() + position - 1, text);
    updateChanged();
    cout << "Line inserted.\n";
}
void deleteLine() {
    clearScreen();
    if (lines.empty()) {
        cout << "The file is empty.\n";
        return;
    }
    viewFile();
    int position = readNumber("Delete which line number? ");
    if (position < 1 || position > (int)lines.size()) {
        cout << "Invalid line number.\n";
        return;
    }
    rememberChange();
    lines.erase(lines.begin() + position - 1);
    updateChanged();
    cout << "Line deleted.\n";
}
void findText() {
    clearScreen();
    string word = readText("Find text: ");
    if (word.empty()) {
        cout << "Search text cannot be empty.\n";
        return;
    }
    bool found = false;
    int total = 0;
    for (size_t i = 0; i < lines.size(); i++) {
        size_t position = 0;
        int lineCount = 0;
        while ((position = lines[i].find(word, position))
               != string::npos) {
            lineCount++;
            total++;
            position += word.size();
        }
        if (lineCount > 0) {
            cout << "Line " << i + 1
                 << " (" << lineCount << " match(es)): ";
            cout << lines[i] << '\n';
            found = true;
        }
    }
    if (found)
        cout << "\nTotal matches: " << total << '\n';
    else
        cout << "Text not found.\n";
}
void replaceText() {
    clearScreen();
    string oldText = readText("Find: ");
    if (oldText.empty()) {
        cout << "Search text cannot be empty.\n";
        return;
    }
    string newText = readText("Replace with: ");
    vector<string> newLines = lines;
    int count = 0;
    for (string& line : newLines) {
        size_t position = 0;
        while ((position = line.find(oldText, position))
               != string::npos) {
            line.replace(position, oldText.size(), newText);
            position += newText.size();
            count++;
        }
    }
    if (count == 0) {
        cout << "Text not found.\n";
        return;
    }
    rememberChange();
    lines = newLines;
    updateChanged();
    cout << count << " replacement(s) made.\n";
}
void showStatistics() {
    clearScreen();
    int words = 0;
    int characters = 0;
    for (int i = 0; i < (int)lines.size(); i++) {
        string line = lines[i];
        characters += (int)line.size();
        bool insideWord = false;
        for (int j = 0; j < (int)line.length(); j++) {
            char c = line[j];
            if (isspace((unsigned char)c)) {
                insideWord = false;
            }
            else if (!insideWord) {
                words++;
                insideWord = true;
            }
        }
    }
    cout << "DOCUMENT STATISTICS\n\n";
    cout << "Lines: " << lines.size() << '\n';
    cout << "Words: " << words << '\n';
    cout << "Characters (without line breaks): "
         << characters << '\n';
}
void undoChange() {
    clearScreen();
    if (undoList.empty()) {
        cout << "Nothing to undo.\n";
        return;
    }
    redoList.push_back(lines);
    lines = undoList.back();
    undoList.pop_back();
    updateChanged();
    cout << "Undo completed.\n";
}
void redoChange() {
    clearScreen();
    if (redoList.empty()) {
        cout << "Nothing to redo.\n";
        return;
    }
    undoList.push_back(lines);
    lines = redoList.back();
    redoList.pop_back();
    updateChanged();
    cout << "Redo completed.\n";
}
string quotePath(string path) {
    return "\"" + path + "\"";
}
void compileAndRun() {
    clearScreen();
    string ext = getExtension(currentFile);
    string name = fs::path(currentFile).filename().string();
    string base = fs::path(currentFile).stem().string();
    string command;
    string runCommand;
    cout << "FILE: " << name << "\n";
    cout << "TYPE: " << languageName(ext) << "\n\n";
    if (changed) {
        cout << "Saving latest changes...\n";
        saveFile();
    }
    if (ext == ".cpp" || ext == ".cc" ||
        ext == ".cxx" || ext == ".c") {
        string outputPath = (fs::path(folder) / base).string();
#ifdef _WIN32
        outputPath += ".exe";
#endif
        if (ext == ".c")
            command = "gcc " + quotePath(currentFile)
                      + " -o " + quotePath(outputPath);
        else
            command = "g++ " + quotePath(currentFile)
                      + " -o " + quotePath(outputPath);
#ifdef _WIN32
        runCommand = quotePath(outputPath);
#else
        runCommand = quotePath("./" + outputPath);
#endif
    }
    else if (ext == ".py") {
#ifdef _WIN32
        command = "py " + quotePath(currentFile);
#else
        command = "python3 " + quotePath(currentFile);
#endif
    }
    else if (ext == ".js") {
        command = "node " + quotePath(currentFile);
    }
    else if (ext == ".java") {
        command = "javac " + quotePath(currentFile);
        runCommand = "java -cp " + quotePath(folder)
                     + " " + base;
    }
    if (ext == ".cpp" || ext == ".cc" ||
        ext == ".cxx" || ext == ".c" || ext == ".java") {
        cout << "Compiling...\n\n";
        int result = system(command.c_str());
        if (result != 0) {
            cout << "\nCompilation failed. Check the error messages above.\n";
            return;
        }
        cout << "\nCompilation successful.\n";
        cout << "Running program...\n\n";
        system(runCommand.c_str());
    }
    else {
        cout << "Running program...\n\n";
        system(command.c_str());
    }
    cout << "\nProgram finished.\n";
}
void listFiles() {
    clearScreen();
    cout << "FILES IN DOCUMENTS\n\n";
    bool found = false;
    for (const auto& item : fs::directory_iterator(folder)) {
        if (item.is_regular_file() &&
            isSupported(getExtension(item.path().string()))) {
            cout << item.path().filename().string() << '\n';
            found = true;
        }
    }
    if (!found)
        cout << "No files yet.\n";
}
bool closeFile() {
    if (changed) {
        string answer = readText(
            "Unsaved changes. Save? (y/n/c to cancel): "
        );
        if (answer == "c" || answer == "C")
            return false;
        if (answer == "y" || answer == "Y") {
            saveFile();
            if (changed)
                return false;
        }
    }
    lines.clear();
    savedLines.clear();
    undoList.clear();
    redoList.clear();
    currentFile = "";
    opened = false;
    changed = false;
    return true;
}
void documentMenu() {
    while (opened) {
        clearScreen();
        string ext = getExtension(currentFile);
        cout << "DOCED  |  "
             << fs::path(currentFile).filename().string();
        if (changed)
            cout << "  *";
        cout << "\n" << languageName(ext) << "\n\n";
        cout << "1. View file       2. Edit all\n";
        cout << "3. Add text        4. Insert line\n";
        cout << "5. Delete line     6. Find / Search\n";
        cout << "7. Find & Replace  8. Undo\n";
        cout << "9. Redo           10. Save\n";
        cout << "11. Document statistics\n";
        if (isCodeFile(ext))
            cout << "12. Compile / Run\n";
        cout << "0. Close file\n\n";
        int choice = readNumber("Choose: ");
        if (choice == 0) {
            if (closeFile())
                return;
        }
        else if (choice == 1)
            viewFile();
        else if (choice == 2)
            editFile();
        else if (choice == 3)
            addText();
        else if (choice == 4)
            insertLine();
        else if (choice == 5)
            deleteLine();
        else if (choice == 6)
            findText();
        else if (choice == 7)
            replaceText();
        else if (choice == 8)
            undoChange();
        else if (choice == 9)
            redoChange();
        else if (choice == 10) {
            clearScreen();
            saveFile();
        }
        else if (choice == 11)
            showStatistics();
        else if (choice == 12 && isCodeFile(ext))
            compileAndRun();
        else {
            clearScreen();
            cout << "Please choose a valid option.\n";
        }
        if (opened)
            waitEnter();
    }
}
void createFile() {
    clearScreen();
    string name = readText(
        "New file name (example: hello.cpp): "
    );
    if (!isAllowedName(name)) {
        cout << "Invalid file name. Use letters, numbers, _ or -.\n";
        return;
    }
    if (fs::path(name).extension().empty())
        name += ".txt";
    if (!isSupported(getExtension(name))) {
        cout << "This file type is not supported.\n";
        return;
    }
    string path = (fs::path(folder) / name).string();
    if (fs::exists(path)) {
        cout << "File already exists. Open it instead.\n";
        return;
    }
    ofstream file(path);
    if (!file) {
        cout << "Could not create the file.\n";
        return;
    }
    file.close();
    loadFile(path);
    cout << "Created: " << name << "\n";
    cout << "Detected type: "
         << languageName(getExtension(name)) << "\n";
}
void openFile() {
    clearScreen();
    string name = readText(
        "File name from documents folder: "
    );
    if (!isAllowedName(name) ||
        !isSupported(getExtension(name))) {
        cout << "Invalid file name or unsupported file type.\n";
        return;
    }
    string path = (fs::path(folder) / name).string();
    if (!fs::exists(path) || !fs::is_regular_file(path)) {
        cout << "File not found.\n";
        return;
    }
    loadFile(path);
    cout << "Opened: " << name << "\n";
    cout << "Detected type: "
         << languageName(getExtension(name)) << "\n";
}
int main() {
    fs::create_directories(folder);
    while (true) {
        clearScreen();
        cout << " Document Editor \n\n";
        cout << "1. Create file\n";
        cout << "2. Open file\n";
        cout << "3. List files\n";
        cout << "0. Exit\n\n";
        int choice = readNumber("Choose: ");
        if (choice == 0)
            break;
        else if (choice == 1) {
            createFile();
            if (opened)
                documentMenu();
            else
                waitEnter();
        }
        else if (choice == 2) {
            openFile();
            if (opened)
                documentMenu();
            else
                waitEnter();
        }
        else if (choice == 3) {
            listFiles();
            waitEnter();
        }
        else {
            cout << "Invalid choice.\n";
            waitEnter();
        }
    }
    clearScreen();
    cout << "DocEd closed. Goodbye!\n";
    return 0;
}
