#include <iostream>
#include <fstream>
#include <string>
#include <limits>

using namespace std;

string fileName = "";
string documentContent = "";

bool documentOpen = false;
bool unsavedChanges = false;

void clearInput()
{
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void showMenu()
{
    cout << "           \n  DOCUMENT EDITOR \n";
    cout << "\n--------------- MAIN MENU ---------------\n";
    cout << "1. Create New Document\n";
    cout << "2. Open and Read Document\n";
    cout << "3. Edit Entire Document\n";
    cout << "4. Save Document\n";
    cout << "5. Exit\n";
    cout << "-----------------------------------------\n";
    cout << "Enter your choice: ";
}

void createNewDocument()
{
    cout << "\nEnter the name of your new document: ";
    getline(cin, fileName);

    if (fileName.empty())
    {
        cout << "File name cannot be empty.\n";
        return;
    }

    if (fileName.size() < 4 ||
        fileName.substr(fileName.size() - 4) != ".txt")
    {
        fileName += ".txt";
    }

    documentContent = "";
    documentOpen = true;
    unsavedChanges = true;

    cout << "\nNew document created successfully!\n";
    cout << "Document name: " << fileName << "\n";
    cout << "Select option 3 to enter your content.\n";
}

void openAndReadDocument()
{
    string name;

    cout << "\nEnter the name of the document to open: ";
    getline(cin, name);

    if (name.empty())
    {
        cout << "File name cannot be empty.\n";
        return;
    }

    if (name.size() < 4 ||
        name.substr(name.size() - 4) != ".txt")
    {
        name += ".txt";
    }

    ifstream inputFile(name);

    if (!inputFile)
    {
        cout << "\nCould not open the document.\n";
        cout << "Please check the file name and try again.\n";
        return;
    }

    string line;
    string content = "";

    while (getline(inputFile, line))
    {
        if (!content.empty())
        {
            content += '\n';
        }

        content += line;
    }

    inputFile.close();

    fileName = name;
    documentContent = content;
    documentOpen = true;
    unsavedChanges = false;

    cout << "\nDocument opened successfully!\n";
    cout << "\n========== DOCUMENT CONTENT ==========\n";

    if (documentContent.empty())
    {
        cout << "The document is empty.\n";
    }
    else
    {
        cout << documentContent << "\n";
    }

    cout << "======================================\n";
}

void editEntireDocument()
{
    if (!documentOpen)
    {
        cout << "\nPlease create or open a document first.\n";
        return;
    }

    cout << "\nCurrent document: " << fileName << "\n";

    cout << "\nCurrent content:\n";
    cout << "----------------------------------------\n";

    if (documentContent.empty())
    {
        cout << "The document is empty.\n";
    }
    else
    {
        cout << documentContent << "\n";
    }

    cout << "----------------------------------------\n";

    cout << "\nEnter the new document content.\n";
    cout << "Press Enter to start a new line.\n";
    cout << "Type END on a separate line to finish.\n\n";

    string newContent = "";
    string line;

    while (true)
    {
        getline(cin, line);

        if (line == "END")
        {
            break;
        }

        if (!newContent.empty())
        {
            newContent += '\n';
        }

        newContent += line;
    }

    documentContent = newContent;
    unsavedChanges = true;

    cout << "\nDocument content updated successfully!\n";
    cout << "Select option 4 to save your changes.\n";
}

void saveDocument()
{
    if (!documentOpen)
    {
        cout << "\nPlease create or open a document first.\n";
        return;
    }

    ofstream outputFile(fileName);

    if (!outputFile)
    {
        cout << "\nError saving the document.\n";
        return;
    }

    outputFile << documentContent;
    outputFile.close();

    unsavedChanges = false;

    cout << "\nDocument saved successfully!\n";
    cout << "File name: " << fileName << "\n";
}

int main()
{
    int choice;

    cout << "Welcome to Document Editor!\n";
    cout << "A simple text editor made using C++.\n";

    do
    {
        showMenu();

        if (!(cin >> choice))
        {
            cout << "\nInvalid input. Please enter a number.\n";
            cin.clear();
            clearInput();
            continue;
        }

        clearInput();

        switch (choice)
        {
            case 1:
                createNewDocument();
                break;

            case 2:
                openAndReadDocument();
                break;

            case 3:
                editEntireDocument();
                break;

            case 4:
                saveDocument();
                break;

            case 5:
                if (unsavedChanges)
                {
                    cout << "\nWarning: You have unsaved changes.\n";
                    cout << "Save your document before exiting if you want to keep them.\n";
                }

                cout << "\nThank you for using Document Editor!\n";
                cout << "Goodbye!\n";
                break;

            default:
                cout << "\nInvalid choice. Please select 1 to 5.\n";
        }

    } while (choice != 5);

    return 0;
}