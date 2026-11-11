#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <limits>
using namespace std;
namespace fs = std::filesystem;

const string folderName = "documents";
vector<string> lines;
vector<vector<string>> undoHistory;
vector<vector<string>> redoHistory;
string currentFile = "";
bool documentOpen = false;
bool unsavedChanges = false;

void saveHistory()
{
    undoHistory.push_back(lines);
    if (undoHistory.size() > 50)
        undoHistory.erase(undoHistory.begin());
    redoHistory.clear();
}

void readFile(const string& fileName)
{
    lines.clear();
    ifstream file(fileName);
    string line;
    while (getline(file, line))
        lines.push_back(line);
}

bool saveFile()
{
    if (!documentOpen)
    {
        cout << "Please open or create a document first.\n";
        return false;
    }

    ofstream file(currentFile);
    if (!file)
    {
        cout << "Could not save the document.\n";
        return false;
    }

    for (size_t i = 0; i < lines.size(); i++)
    {
        file << lines[i];
        if (i + 1 < lines.size())
            file << '\n';
    }

    file.close();
    unsavedChanges = false;
    cout << "Document saved successfully.\n";
    return true;
}

void showDocument()
{
    if (!documentOpen)
    {
        cout << "Please open or create a document first.\n";
        return;
    }

    cout << "\n========== DOCUMENT ==========" << endl;
    if (lines.empty())
        cout << "(Document is empty)\n";
    else
    {
        for (size_t i = 0; i < lines.size(); i++)
            cout << i + 1 << ". " << lines[i] << '\n';
    }
    cout << "==============================\n";
    if (unsavedChanges)
        cout << "You have unsaved changes.\n";
}

string makeTextFileName(string name)
{
    if (name.size() < 4 || name.substr(name.size() - 4) != ".txt")
        name += ".txt";
    return name;
}

void createDocument()
{
    string name;
    cout << "Enter a name for the new document: ";
    getline(cin, name);

    if (name.empty())
    {
        cout << "Document name cannot be empty.\n";
        return;
    }

    name = makeTextFileName(name);
    string path = folderName + "/" + name;

    if (fs::exists(path))
    {
        cout << "A document with this name already exists.\n";
        return;
    }

    ofstream file(path);
    if (!file)
    {
        cout << "Could not create the document.\n";
        return;
    }
    file.close();

    currentFile = path;
    lines.clear();
    undoHistory.clear();
    redoHistory.clear();
    documentOpen = true;
    unsavedChanges = false;
    cout << "New document created and opened.\n";
}

void openDocument()
{
    string name;
    cout << "Enter the document name to open: ";
    getline(cin, name);

    if (name.empty())
    {
        cout << "Document name cannot be empty.\n";
        return;
    }

    name = makeTextFileName(name);
    string path = folderName + "/" + name;
    if (!fs::exists(path))
    {
        cout << "Document not found in the documents folder.\n";
        return;
    }

    currentFile = path;
    readFile(currentFile);
    undoHistory.clear();
    redoHistory.clear();
    documentOpen = true;
    unsavedChanges = false;
    cout << "Document opened successfully.\n";
    showDocument();
}

void editDocument()
{
    if (!documentOpen)
    {
        cout << "Please open or create a document first.\n";
        return;
    }

    cout << "\nEnter the complete document content below.\n";
    cout << "Type END on a separate line when you have finished.\n";
    cout << "----------------------------------------------\n";

    vector<string> newLines;
    string text;
    while (getline(cin, text))
    {
        if (text == "END")
            break;
        newLines.push_back(text);
    }

    if (cin.eof())
    {
        cout << "Input ended before END was entered.\n";
        cin.clear();
        return;
    }

    saveHistory();
    lines = newLines;
    unsavedChanges = true;
    cout << "Document content updated. Use Save Document to keep the changes.\n";
}

void searchText()
{
    if (!documentOpen)
    {
        cout << "Please open or create a document first.\n";
        return;
    }

    string search;
    cout << "Enter the text to search for: ";
    getline(cin, search);
    if (search.empty())
    {
        cout << "Search text cannot be empty.\n";
        return;
    }

    bool found = false;
    for (size_t i = 0; i < lines.size(); i++)
    {
        if (lines[i].find(search) != string::npos)
        {
            cout << "Found on line " << i + 1 << ": " << lines[i] << '\n';
            found = true;
        }
    }
    if (!found)
        cout << "Text was not found.\n";
}

void searchDocumentNames()
{
    string namePart;
    cout << "Enter part of the document name to search for: ";
    getline(cin, namePart);
    if (namePart.empty())
    {
        cout << "Search text cannot be empty.\n";
        return;
    }

    bool found = false;
    cout << "\nMatching documents:\n";
    for (const auto& entry : fs::directory_iterator(folderName))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".txt")
        {
            string fileName = entry.path().filename().string();
            if (fileName.find(namePart) != string::npos)
            {
                cout << fileName << '\n';
                found = true;
            }
        }
    }
    if (!found)
        cout << "No document names matched your search.\n";
}

void searchAllDocumentsByContent()
{
    string search;
    cout << "Enter a word or phrase to search across saved documents: ";
    getline(cin, search);
    if (search.empty())
    {
        cout << "Search text cannot be empty.\n";
        return;
    }

    bool found = false;
    for (const auto& entry : fs::directory_iterator(folderName))
    {
        if (!entry.is_regular_file() || entry.path().extension() != ".txt")
            continue;

        if (documentOpen && entry.path().string() == currentFile && unsavedChanges)
        {
            for (size_t i = 0; i < lines.size(); i++)
            {
                if (lines[i].find(search) != string::npos)
                {
                    cout << entry.path().filename().string() << " (current unsaved version), line "
                         << i + 1 << ": " << lines[i] << '\n';
                    found = true;
                }
            }
        }
        else
        {
            ifstream file(entry.path());
            string line;
            size_t lineNumber = 0;
            while (getline(file, line))
            {
                lineNumber++;
                if (line.find(search) != string::npos)
                {
                    cout << entry.path().filename().string() << ", line "
                         << lineNumber << ": " << line << '\n';
                    found = true;
                }
            }
        }
    }

    if (!found)
        cout << "No matching content was found in saved documents.\n";
}

void replaceText()
{
    if (!documentOpen)
    {
        cout << "Please open or create a document first.\n";
        return;
    }

    string oldText, newText;
    cout << "Enter the text to find: ";
    getline(cin, oldText);
    if (oldText.empty())
    {
        cout << "Text to find cannot be empty.\n";
        return;
    }
    cout << "Enter the replacement text: ";
    getline(cin, newText);

    vector<string> updatedLines = lines;
    int replacements = 0;
    for (string& line : updatedLines)
    {
        size_t position = 0;
        while ((position = line.find(oldText, position)) != string::npos)
        {
            line.replace(position, oldText.length(), newText);
            position += newText.length();
            replacements++;
        }
    }

    if (replacements == 0)
    {
        cout << "Text was not found, so nothing was changed.\n";
        return;
    }

    saveHistory();
    lines = updatedLines;
    unsavedChanges = true;
    cout << "Replacement complete. Number of replacements: " << replacements << '\n';
}

void undoChange()
{
    if (!documentOpen)
    {
        cout << "Please open or create a document first.\n";
        return;
    }
    if (undoHistory.empty())
    {
        cout << "There is no change to undo.\n";
        return;
    }

    redoHistory.push_back(lines);
    lines = undoHistory.back();
    undoHistory.pop_back();
    unsavedChanges = true;
    cout << "Last change undone.\n";
}

void redoChange()
{
    if (!documentOpen)
    {
        cout << "Please open or create a document first.\n";
        return;
    }
    if (redoHistory.empty())
    {
        cout << "There is no change to redo.\n";
        return;
    }

    undoHistory.push_back(lines);
    lines = redoHistory.back();
    redoHistory.pop_back();
    unsavedChanges = true;
    cout << "Change restored.\n";
}

void showStatistics()
{
    if (!documentOpen)
    {
        cout << "Please open or create a document first.\n";
        return;
    }

    size_t characters = 0;
    size_t words = 0;
    for (const string& line : lines)
    {
        characters += line.length();
        bool insideWord = false;
        for (char ch : line)
        {
            if (ch == ' ' || ch == '\t')
                insideWord = false;
            else if (!insideWord)
            {
                words++;
                insideWord = true;
            }
        }
    }

    cout << "\n----- Document Statistics -----\n";
    cout << "Lines      : " << lines.size() << '\n';
    cout << "Words      : " << words << '\n';
    cout << "Characters : " << characters << " (spaces between words included)\n";
    cout << "--------------------------------\n";
}

void listDocuments()
{
    cout << "\n----- Saved Text Documents -----\n";
    bool found = false;
    for (const auto& entry : fs::directory_iterator(folderName))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".txt")
        {
            cout << entry.path().filename().string() << '\n';
            found = true;
        }
    }
    if (!found)
        cout << "No text documents found.\n";
    cout << "--------------------------------\n";
}

void closeDocument()
{
    if (!documentOpen)
    {
        cout << "No document is currently open.\n";
        return;
    }

    if (unsavedChanges)
    {
        char choice;
        cout << "There are unsaved changes. Save before closing? (y/n): ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (choice == 'y' || choice == 'Y')
        {
            if (!saveFile())
                return;
        }
    }

    currentFile = "";
    lines.clear();
    undoHistory.clear();
    redoHistory.clear();
    documentOpen = false;
    unsavedChanges = false;
    cout << "Document closed.\n";
}

void showHelpSupport()
{
    cout << "\n========== HELP AND SUPPORT - VERSION 2 ==========\n";
    cout << "WELCOME\n";
    cout << "This is a simple console-based editor for plain-text .txt files.\n\n";
    cout << "GETTING STARTED\n";
    cout << "1. Choose 1 to create a document. Enter a name such as Notes.\n";
    cout << "2. Choose 2 to open a saved document by its name.\n";
    cout << "3. Choose 3 to read the currently open document.\n";
    cout << "4. Choose 4 to replace the whole document with new content.\n";
    cout << "   Type your text on separate lines, then type END alone on a line.\n";
    cout << "5. Choose 13 to save your changes.\n\n";
    cout << "SEARCH AND EDITING\n";
    cout << "- Option 5 searches for a word or phrase in the open document.\n";
    cout << "- Option 6 searches document names.\n";
    cout << "- Option 7 searches text inside all saved documents.\n";
    cout << "- Option 8 replaces matching text in the open document.\n";
    cout << "- Options 9 and 10 undo and redo editing changes.\n";
    cout << "- Option 11 shows line, word, and character counts.\n";
    cout << "- Option 12 lists saved text documents.\n\n";
    cout << "SAVING AND CLOSING\n";
    cout << "Documents are stored in the 'documents' folder as .txt files.\n";
    cout << "Save your work before closing. The program warns about unsaved changes.\n\n";
    cout << "TIPS AND LIMITATIONS\n";
    cout << "- Enter END in capital letters on a separate line to finish editing.\n";
    cout << "- Do not include END as a line of document content.\n";
    cout << "- This version supports plain text, not Word formatting or PDF editing.\n";
    cout << "- If a document cannot be opened, check its name in the documents folder.\n";
    cout << "==================================================\n";
}

void showMenu()
{
    cout << "\n========== DOCUMENT EDITOR ==========\n";
    cout << "1. Create New Document\n";
    cout << "2. Open Existing Document\n";
    cout << "3. Read Current Document\n";
    cout << "4. Edit Entire Document\n";
    cout << "5. Search Word/Phrase in Current Document\n";
    cout << "6. Search Document by File Name\n";
    cout << "7. Search Content Across All Documents\n";
    cout << "8. Find and Replace Text in Current Document\n";
    cout << "9. Undo\n";
    cout << "10. Redo\n";
    cout << "11. Show Document Statistics\n";
    cout << "12. List Saved Documents\n";
    cout << "13. Save Document\n";
    cout << "14. Close Current Document\n";
    cout << "15. Help and Support - How to Use the Editor\n";
    cout << "0. Exit\n";
    cout << "Choose an option: ";
}

int main()
{
    try
    {
        if (!fs::exists(folderName))
            fs::create_directory(folderName);
    }
    catch (const fs::filesystem_error& error)
    {
        cout << "Could not prepare the documents folder: " << error.what() << '\n';
        return 1;
    }

    int choice;
    do
    {
        showMenu();
        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a menu number.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice)
        {
            case 1: createDocument(); break;
            case 2: openDocument(); break;
            case 3: showDocument(); break;
            case 4: editDocument(); break;
            case 5: searchText(); break;
            case 6: searchDocumentNames(); break;
            case 7: searchAllDocumentsByContent(); break;
            case 8: replaceText(); break;
            case 9: undoChange(); break;
            case 10: redoChange(); break;
            case 11: showStatistics(); break;
            case 12: listDocuments(); break;
            case 13: saveFile(); break;
            case 14: closeDocument(); break;
            case 15: showHelpSupport(); break;
            case 0:
                if (documentOpen && unsavedChanges)
                {
                    char saveChoice;
                    cout << "You have unsaved changes. Save before exiting? (y/n): ";
                    cin >> saveChoice;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    if (saveChoice == 'y' || saveChoice == 'Y')
                        saveFile();
                }
                cout << "Thank you for using Document Editor.\n";
                break;
            default: cout << "Invalid option. Please choose again.\n";
        }
    } while (choice != 0);

    return 0;
}
