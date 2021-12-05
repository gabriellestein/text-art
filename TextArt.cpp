/*
* This program allows display and editing of text art (also called ASCII art).
*/

#include <iostream>
#include <fstream>
#include <cctype>
#include <windows.h>
#include <conio.h>
#include "Definitions.h"
using namespace std;


int main()
{
	// Start with a clear the screen
	system("cls");

	// Creates and initilizes new canvas
	Node* current;
	current = newCanvas();

	// Linked List that will hold all previous states of canvas
	List undoList;

	// Linked List that will hold redo states of canvas
	List redoList;

	// Linked list that will hold all clips in animation
	List clips;

	// Char that holds user's menu choice
	char choice;

	// Bool variable used in draw function
	bool animate = false;

	// Second line of menu option
	char menu[] = "<E>dit / <M>ove / <R>eplace / <D>raw / <C>lear / <U>ndo / <L>oad / <S>ave / <Q>uit: ";

	// Continues to run program until user chooeses QUIT from menu
	do
	{
		// Displays canvas and prints menu option.
		displayCanvas(current->item);
		//Removes menu options.
		clearLine(MAXROWS + 2, sizeof(menu));
		clearLine(MAXROWS + 1, sizeof(menu));

		// Char array that will contains user-inputted name of file
		char inputname[FILENAMESIZE - 15];
		// Char array that wil. format user input to look like "SavedFiles\\inputname.txt".
		char filename[FILENAMESIZE];

		// Char varaible that will will hold user's choice for what type of file they are trying to load/save
		char type;

		// 1st menu line
		cout << "<A>nimate: ";
		// Prints out state of animate
		if (animate)
		{
			cout << "Y";
		}
		else
		{
			cout << "N";
		}
		// Prints out number of available undos
		cout << " / <U>ndo: " << undoList.count;
		// Prints out number of available redos
		if (redoList.count > 0)
		{
			cout << " / Red<O>: " << redoList.count;
		}
		// Prints out number of clips available
		cout << " / Cl<I>p: " << clips.count;
		if (clips.count > 1)
		{
			cout << " / <P>lay";
		}
		cout << '\n';

		// Second line for menu
		printf(menu);

		// User's menu choice
		cin >> choice;
		cin.ignore();

		//Removes menu options.
		clearLine(MAXROWS + 2, sizeof(menu));
		clearLine(MAXROWS + 1, sizeof(menu));

		switch (choice)
		{
			// Undo option
		case 'U':
		case 'u':
			restore(undoList, redoList, current);
			break;

			// Animate option: toggle
		case 'a':
		case 'A':
			animate = !animate;
			break;

			// Redo option
		case 'o':
		case 'O':
			restore(redoList, undoList, current);
			break;

			// Clip option
		case 'i':
		case 'I':
			// Turns current canvas into a clip that can be animated
			addNode(clips, newCanvas(current));
			break;

			// Play option
		case 'p':
		case 'P':
			// Repeats animation of all clips currently loaded
			play(clips);
			break;

			// Edit option
		case 'E':
		case 'e':
			// Add to current canvas to undoList
			addUndoState(undoList, redoList, current);

			cout << "Press <ESC> to stop editing";
			editCanvas(current->item);
			break;

			// Move option
		case 'M':
		case 'm':
			// Add to current canvas to undoList
			addUndoState(undoList, redoList, current);

			// Int variables that will hold how much canvas should be shifted by rows/cols
			int colMove, rowMove;

			// Prompts user for amount of coloumns to move canvas
			cout << "Enter coloumn units to move: ";
			cin >> colMove;
			cin.ignore();
			while (cin.fail())
			{
				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			}

			// Prompts user for amount of rows to move canvas
			cout << "Enter row units to move: ";
			cin >> rowMove;
			cin.ignore();
			while (cin.fail())
			{
				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			}

			// Calls moveCanvas fucntion with user-inputted ints.
			moveCanvas(current->item, rowMove, colMove);
			break;

			// Replace option
		case 'R':
		case 'r':
			// Add to current canvas to undoList
			addUndoState(undoList, redoList, current);

			// Char variable that will hold char currently in canvas and char that will replace it
			char oldCh, newCh;

			// Prompts user to enter char they wish to replace.
			cout << "Enter character to replace: ";
			cin.get(oldCh);
			cin.ignore();
			while (cin.fail())
			{
				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			}

			// Prompts user for char they wish to replace with.
			cout << "Enter character to replace with: ";
			cin.get(newCh);
			while (cin.fail())
			{
				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			}

			// Calls replaceCanvas function with user-inputted chars
			replace(current->item, oldCh, newCh);

			break;

			// Draw option
		case 'D':
		case 'd':
			// Opens draw menu and draw options
			menuTwo(current, undoList, redoList, clips, animate);
			break;

			// Clear option
		case 'C':
		case 'c':
			// Add to current canvas to undoList
			addUndoState(undoList, redoList, current);

			initCanvas(current->item);
			break;
			// Load option
		case 'L':
		case 'l':
			cout << "<C>anvas or <A>nimation ? ";
			cin >> type;
			cin.ignore();
			clearLine(MAXROWS + 1, 87);
			cin.clear();

			cout << "Enter file name (don't enter txt). ";
			cin.getline(inputname, FILENAMESIZE - 15);


			// If user said file was canvas, load normally.
			if (type == 'c' || type == 'C')
			{
				snprintf(filename, FILENAMESIZE, "SavedFiles\\%s.txt", inputname);

				if (!loadCanvas(current->item, filename))
				{
					cerr << "ERROR: File cannot be read. ";
					system("pause");
					break;
				}
			}
			// Else, load using loadClips
			if (type == 'a' || type == 'A')
			{
				// Leave off .txt
				snprintf(filename, FILENAMESIZE, "SavedFiles\\%s", inputname);

				if (!loadClips(clips, filename))
				{
					cerr << "Error: file cannot be read. ";
					system("pause");
					break;
				}
				else
				{
					cout << "Clips loaded! ";
					system("pause");
				}
			}

			break;

			// Save option
		case 'S':
		case 's':
			cout << "<C>anvas or <A>nimation ? ";
			cin >> type;
			clearLine(MAXROWS + 1, sizeof(menu));
			cin.clear();

			cout << "Enter file name (don't enter txt). ";
			cin.ignore();
			cin.getline(inputname, FILENAMESIZE - 15);
			while (cin.fail())
			{
				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			}


			// If user said file was canvas, save normally.
			if (type == 'c' || type == 'C')
			{
				snprintf(filename, FILENAMESIZE, "SavedFiles\\%s.txt", inputname);

				if (!saveCanvas(current->item, filename))
				{
					cerr << "ERROR: File cannot be read\n";
					system("pause");
					break;
				}
				else {
					cout << "Canvas saved! ";
					system("pause");
				}
			}
			// Else, save file using saveClips function
			else if (type == 'a' || type == 'A')
			{
				snprintf(filename, FILENAMESIZE, "SavedFiles\\%s", inputname);
				if (!saveClips(clips, filename))
				{
					cout << "ERROR: file cannot be written" << endl;
					system("pause");
					break;
				}
				else {
					cout << "Clips saved! ";
					system("pause");
				}
			}
			break;
		}//end switch
	} while (choice != 'Q' && choice != 'q');

	// Delete nodes and lists to prevent memory leaks
	delete current;
	deleteList(undoList);
	deleteList(redoList);
	deleteList(clips);
	return 0;

}

//gotoxy function positions cursor on screen.
void gotoxy(short row, short col)
{
	COORD pos = { col, row };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

//clearLine function clears chosen line of output from screen.
void clearLine(int lineNum, int numOfChars)
{
	// Move cursor to the beginning of the specified line on the console
	gotoxy(lineNum, 0);

	// Write a specified number of spaces to overwrite characters
	for (int x = 0; x < numOfChars; x++)
		cout << " ";

	// Move cursor back to the beginning of the line
	gotoxy(lineNum, 0);
}

//replace() function re-assigns every instance of old char with new char in canvas array.
void replace(char canvas[][MAXCOLS], char oldCh, char newCh)
{
	for (int row = 0; row < MAXROWS; row++)
	{
		for (int col = 0; col < MAXCOLS; col++)
		{
			//If element of canvas matches old char, old char is changed to new char.
			if (canvas[row][col] == oldCh)
			{
				canvas[row][col] = newCh;
			}
		}
	}
}

//editCanvas function puts canvas in editing mode and allows user to directly manipulate data in canvas array.
void editCanvas(char canvas[][MAXCOLS])
{
	char input = 0;
	int row = 0;
	int col = 0;

	// Move cursor to row,col and then get a single character from the keyboard
	gotoxy(row, col);
	input = _getch();

	// Stays in editing mode until user presses ESC
	while (input != ESC)
	{
		// Checks to make sure user input is not null.
		if (input == '\0')
		{
			input = _getch();
		}
		// Checks to see is user input is SPECIAL and therefore an arrow key
		else if (input == SPECIAL)
		{
			// Gets new input which is the arrow key.
			input = _getch();

			// Adjusts row or column cursor will move to depending on arrow key pressed
			switch (input)
			{
				// Makes sure cursor will stay inbounds before changing row and col depending on arrow key.
			case RIGHTARROW:
				if (col < MAXCOLS - 1)
				{
					col++;
				}
				break;
			case LEFTARROW:
				if (col > 0)
				{
					col--;
				}
				break;
			case UPARROW:
				if (row > 0)
				{
					row--;
				}
				break;
			case DOWNARROW:
				if (row < MAXROWS - 1)
				{
					row++;
				}
				break;
			}
		}
		// Checks to see if user input is a valid char that can be in canvas
		else if (input >= 32 && input <= 126)
		{
			// Updates canvas array to be new char.
			canvas[row][col] = input;
			// Prints char so user can see the change they made
			cout << input;
		}

		// Moves cursor is arrow key was pressed, or keep cursor in same place in valid char was pressed.
		gotoxy(row, col);
		// Get next user input.
		input = _getch();
	}
}

//moveCanvas function used to shift all characters in canvas left/right and up/down based on user-input
void moveCanvas(char canvas[][MAXCOLS], int rowValue, int colValue)
{
	// Temporary canvas that will contain changes made to canvas array
	char tempCanvas[MAXROWS][MAXCOLS];
	initCanvas(tempCanvas);

	//Interates forward through rows and cols because both rowValue and colValue are negative
	for (int row = 0; row < MAXROWS; row++)
	{
		for (int col = 0; col < MAXCOLS; col++)
		{
			int newRowValue = row + rowValue;
			int newColValue = col + colValue;

			if (newRowValue >= 0 && newRowValue < MAXROWS && newColValue >= 0 && newColValue < MAXCOLS)
			{
				tempCanvas[newRowValue][newColValue] = canvas[row][col];
			}
		}
	}

	// Copies tempCavnas into canvas array
	copyCanvas(canvas, tempCanvas);
}

//initCanvas fucntion fills a canvas array with spaces
void initCanvas(char canvas[][MAXCOLS])
{
	//Fills every position in canvas array with a space.
	for (int row = 0; row < MAXROWS; row++)
	{
		for (int col = 0; col < MAXCOLS; col++)
		{
			canvas[row][col] = ' ';
		}
	}
}

//displayCanvas function used to print enitre canvas to the console
void displayCanvas(char canvas[][MAXCOLS])
{
	// Begins printing canvas in top right of console
	gotoxy(0, 0);

	// Iterates through array plus one row and one col for border.
	for (int row = 0; row <= MAXROWS; row++)
	{
		for (int col = 0; col <= MAXCOLS; col++)
		{
			// Checks for the end of the row and prints the right side '|' border
			if (col == MAXCOLS && row != MAXROWS)
			{
				cout << '|' << '\n';
			}
			// Checks for the end of the column and prints the bottom '-' border
			else if (row == MAXROWS && col != MAXCOLS)
			{
				cout << '-';
			}
			// Prints space in bottom right corner of the canvas.
			else if (row == MAXROWS && col == MAXCOLS)
			{
				cout << ' ';
			}
			// Prints canvas
			else
			{
				cout << canvas[row][col];
			}
		}
	}
	//Makes sure menu starts on new line below canvas.
	cout << '\n';
}

//copyCanvas function saves a version of canvas before change is made 
void copyCanvas(char to[][MAXCOLS], char from[][MAXCOLS])
{
	//Puts data from canvas into temp canvas before a change is made so it can be undone.
	for (int row = 0; row < MAXROWS; row++)
	{
		for (int col = 0; col < MAXCOLS; col++)
		{
			to[row][col] = from[row][col];
		}
	}
}

//saveCanvas function saves current canvas to a new file on computer.
bool saveCanvas(char canvas[][MAXCOLS], char filename[])
{
	// Opens writable file.
	ofstream newFile;
	newFile.open(filename);

	// Checks if the file can be made
	if (newFile.fail())
	{
		return false;
	}
	else
	{
		// Prints canvas to file
		for (int row = 0; row < MAXROWS; row++)
		{
			for (int col = 0; col < MAXCOLS; col++)
			{
				newFile << canvas[row][col];
			}
			// Makes sure eash line ends in iwth enter char so it can be read correctly my loadCanvas.
			newFile << '\n';
		}
		newFile << '\n';
		// Closes file after all data from canvas has be written.
		newFile.close();
	}
	return true;
}

//loadCanvas function reads contens of user-specified file, and stores it in canvas array.
bool loadCanvas(char canvas[][MAXCOLS], char filename[])
{
	// Opens readable file.
	ifstream userFile;
	userFile.open(filename);
	// Next char to be read in text file
	char c;

	// Checks to make sure file exists
	if (!userFile)
	{
		return false;
	}
	else
	{
		// Clears canvas so that previously loaded canvas with more rows/cols than current canvas do not show up.
		initCanvas(canvas);
		

		// Checks to see that rows are only read until MAXROWS, and that extra rows are discarded.
		for (int row = 0; row < MAXROWS && !userFile.eof(); row++)
		{
			// Gets first character from this line in file
			c = userFile.get();
			
			// Checks until '\n' for all chars in current row.
			for (int col = 0; col < MAXCOLS && c != '\n'; col++)
			{
				// Adds current char to canvas array and gets next char in file
				canvas[row][col] = c;
				c = userFile.get();
			}
			// If there are more cols in current row than MAXCOLS, it reads and discards extra chars until next line is reached.
			while (c != '\n' && !userFile.eof())
			{
				c = userFile.get();
			}
		}
		// Closes file after all applicable data has been read.
		userFile.close();
	}
	return true;
}