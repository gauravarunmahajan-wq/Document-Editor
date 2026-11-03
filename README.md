#  Group-ID : 6 
# Team Members : Gaurav Mahajan, Bhavesh Borse
# Document Editor

## Version 1

Version 1 focuses on the basic features of a text editor. It keeps the interface simple and allows users to create, read, edit, and save text documents.

## Features

- **Create a new document** — Create a new text file.
- **Open an existing document** — Open a previously saved document.
- **Read document** — Display the contents of the current document.
- **Edit entire document** — Replace the document's content by entering new text.
- **Simple multiline editing** — Type multiple lines and finish editing by entering `END` on a separate line.
- **Save document** — Save the current content to a file.
- **Exit** — Close the application.

## How to Edit a Document

1. Choose the edit option from the menu.
2. Enter your document content. You can write multiple lines.
3. Type `END` on a separate line when you are finished.
4. Save the document to keep your changes.

Example:

```text
This is my first document.
I am learning C++ file handling.
This editor supports multiline text.

END
```

`END` is used to finish input and is not included as document content.

## Project Structure

```text
Document-Editor/
├── DocumentEditor_Version1.cpp
└── README.md
```

## File Format

Version 1 works with plain-text documents (.txt). 

## Future Improvements

Possible improvements for later versions include:

- Searching document text
- Find and replace
- Undo and redo
- Document statistics
- A built-in Help and Support guide
- Quick commands and formatting options
