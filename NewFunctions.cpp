#include <iostream>
#include <windows.h>
#include <conio.h>
#include "Definitions.h"
using namespace std;


Point::Point(DrawPoint p)
{
	row = (int)round(p.row);
	col = (int)round(p.col);
}

// https://math.stackexchange.com/questions/39390/determining-end-coordinates-of-line-with-the-specified-length-and-angle
DrawPoint findEndPoint(DrawPoint start, int len, int angle)
{
	DrawPoint end;
	end.col = start.col + len * cos(degree2radian(angle));
	end.row = start.row + len * sin(degree2radian(angle));
	return end;
}


// Use this to draw characters into the canvas, with the option of performing animation
void drawHelper(char canvas[][MAXCOLS], Point p, char ch, bool animate)
{
	// Pause time between steps (in milliseconds)
	const int TIME = 50;

	// Make sure point is within bounds
	if (p.row >= 0 && p.row < MAXROWS && p.col >= 0 && p.col < MAXCOLS)
	{
		// Draw character into the canvas
		canvas[p.row][p.col] = ch;

		// If animation is enabled, draw to screen at same time
		if (animate)
		{
			gotoxy(p.row, p.col);
			printf("%c", ch);
			Sleep(TIME);
		}
	}
}


// Fills gaps in a row caused by mismatch between match calculations and screen coordinates
// (i.e. the resolution of our 'canvas' isn't very good)
void drawLineFillRow(char canvas[][MAXCOLS], int col, int startRow, int endRow, char ch, bool animate)
{
	// determine if we're counting up or down
	if (startRow <= endRow)
		for (int r = startRow; r <= endRow; r++)
		{
			Point point(r, col);
			drawHelper(canvas, point, ch, animate);
		}
	else
		for (int r = startRow; r >= endRow; r--)
		{
			Point point(r, col);
			drawHelper(canvas, point, ch, animate);
		}
}


// Draw a single line from start point to end point
void drawLine(char canvas[][MAXCOLS], DrawPoint start, DrawPoint end, bool animate)
{
	char ch;

	Point scrStart(start);
	Point scrEnd(end);

	// vertical line
	if (scrStart.col == scrEnd.col)
	{
		ch = '|';

		drawLineFillRow(canvas, scrStart.col, scrStart.row, scrEnd.row, ch, animate);
	}
	// non-vertical line
	else
	{
		int row = -1, prevRow;

		// determine the slope of the line
		double slope = (start.row - end.row) / (start.col - end.col);

		// choose appropriate characters based on 'steepness' and direction of slope
		if (slope > 1.8)  ch = '|';
		else if (slope > 0.08)  ch = '`';
		else if (slope > -0.08)  ch = '-';
		else if (slope > -1.8) ch = '\'';
		else ch = '|';

		// determine if columns are counting up or down
		if (scrStart.col <= scrEnd.col)
		{
			// for each column from start to end, calculate row values
			for (int col = scrStart.col; col <= scrEnd.col; col++)
			{
				prevRow = row;
				row = (int)round(slope * (col - start.col) + start.row);

				// draw from previous row to current row (to fill in row gaps)
				if (prevRow > -1)
				{
					drawLineFillRow(canvas, col, prevRow, row, ch, animate);
				}
			}
		}
		else
		{
			// for each column from start to end, calculate row values
			for (int col = scrStart.col; col >= scrEnd.col; col--)
			{
				prevRow = row;
				row = (int)round(slope * (col - start.col) + start.row);

				// draw from previous row to current row (to fill in row gaps)
				if (prevRow > -1)
				{
					drawLineFillRow(canvas, col, prevRow, row, ch, animate);
				}
			}
		}
	}
}


// Draws a single box around a center point
void drawBox(char canvas[][MAXCOLS], Point center, int height, bool animate)
{
	int sizeHalf = height / 2;
	int ratio = (int)round(MAXCOLS / (double)MAXROWS * sizeHalf);

	// Calculate where the four corners of the box should be
	DrawPoint points[4];
	points[0] = DrawPoint(center.row - sizeHalf, center.col - ratio);
	points[1] = DrawPoint(center.row - sizeHalf, center.col + ratio);
	points[2] = DrawPoint(center.row + sizeHalf, center.col + ratio);
	points[3] = DrawPoint(center.row + sizeHalf, center.col - ratio);

	// Draw the four lines of the box
	for (int x = 0; x < 3; x++)
	{
		drawLine(canvas, points[x], points[x + 1], animate);
	}
	drawLine(canvas, points[3], points[0], animate);

	// Replace the corners with a better looking character
	for (int x = 0; x < 4; x++)
	{
		drawHelper(canvas, points[x], '+', animate);
	}
}

// Menu for the drawing tools
void menuTwo(Node*& current, List& undoList, List& redoList, List& clips, bool& animate)
{
	// User menu choice
	char choice;

	// Second line menu options when in Draw mode
	char menu[81] = "<F>ill / <L>ine / <B>ox / <N>ested Boxes / <T>ree / <M>ain Menu: ";

	// Point variable used to determine where cursor is based on user input, starts at (0,0)
	Point pt;
	pt.row = 0;
	pt.col = 0;

	// What user inputs when choosing point on a screen, used to see is user cancelled
	char inputCh;

	// Start and end points used to create lines
	DrawPoint start, end;

	do
	{
		// Used in place of system("cls")
		gotoxy(0, 0);
		// Shows changes made to canvas after each menu option executes
		displayCanvas(current->item);
		// Makes sure no previous text is still on screen
		clearLine(MAXROWS + 2, sizeof(menu));
		clearLine(MAXROWS + 1, sizeof(menu));

		// 1st menu line
		cout << "<A>nimate: ";
		if (animate)
		{
			cout << "Y";
		}
		else
		{
			cout << "N";
		}
	
		cout << " / <U>ndo: " << undoList.count;
		
		if (redoList.count > 0)
		{
			cout << " / Red<O>: " << redoList.count;
		}
		
		cout << " / Cl<I>p: ";
		cout << clips.count;
		if (clips.count > 1)
		{
			cout << " / <P>lay";
		}
		cout << '\n';

		//Prints draw menu
		printf(menu);

		cin >> choice;
		cin.ignore();

		// Clears menu
		clearLine(MAXROWS + 2, sizeof(menu));
		clearLine(MAXROWS + 1, sizeof(menu));
 
		switch (choice)
		{
			// Undo option
		case 'U':
		case 'u':
			restore(undoList, redoList, current);
			break;

			//Animate option
		case 'a':
		case 'A':
			//Toggles animate functioning on/off
			animate = !animate;

			break;

			//Redo option
		case 'o':
		case 'O':
			restore(redoList, undoList, current);
			break;

			//Clip option
		case 'i':
		case 'I':
			//Adds a new clip to clip list, holds a slide of animation
			addNode(clips, newCanvas(current));
			break;

			//Play option
		case 'p':
		case 'P':
			//Repeats animation of all clips currently loaded
			play(clips);
			break;

			// Fill option
		case 'F':
		case 'f':
			// User chooses new char to replace char currently in chosen location
			char newCh;
			printf("Enter character to fill with from current location / <ESC> to cancel: ");
			newCh = getPoint(pt);

			// If user did not escape, chosen section will be filled with chosen char
			if (newCh != ESC)
			{
				//Add to current canvas to undoList
				addUndoState(undoList, redoList, current);
				fillRecursive(current->item, pt.row, pt.col, current->item[pt.row][pt.col], newCh, animate);
			}
			break;

			//Line option
		case 'L':
		case 'l':
			cout << "Type any letter to choose start point / <ESC> to cancel";
			inputCh = getPoint(pt);

			//If user does not escape, user will be asked to input end point for line
			if (inputCh != ESC)
			{
				cout << inputCh;
				start = pt;

				// User chooses end point for line
				clearLine(MAXROWS + 1, sizeof(menu));
				cout << "Type any letter to choose end point / <ESC> to cancel";
				inputCh = getPoint(pt);
				end = pt;

				//If user does not escape, user-specified line will be added to canvas
				if (inputCh != ESC)
				{
					//Add to current canvas to undoList
					addUndoState(undoList, redoList, current);
					drawLine(current->item, start, end, animate);
				}
			}
			break;

			//Boxes option
		case 'B':
		case 'b':
			int boxHeight;
			cout << "Enter size: ";
			cin >> boxHeight;
			cin.ignore();

			//User chooses point that will be center of box
			clearLine(MAXROWS + 1, sizeof(menu));
			cout << "Type any letter to choose box center, or <C> for screen center / <ESC> to cancel";
			inputCh = getPoint(pt);

			//If user does not escape, box is added to canvas at user specifications
			if (inputCh != ESC)
			{
				if (inputCh == 'c' || inputCh == 'C')
				{
					pt.row = MAXROWS / 2;
					pt.col = MAXCOLS / 2;
				}
				//Add to current canvas to undoList
				addUndoState(undoList, redoList, current);
				drawBox(current->item, pt, boxHeight, animate);
			}
			break;

			//Nested Boxes option
		case 'N':
		case 'n':
			//Size of largest box
			int nestedBoxHeight;
			cout << "Enter size of largest box: ";
			cin >> nestedBoxHeight;
			cin.ignore();

			//User chooses new point for center all boxes
			clearLine(MAXROWS + 1, sizeof(menu));
			cout << "Type any letter to choose box center, or <C> for screen center / <ESC> to cancel";
			inputCh = getPoint(pt);

			//If used does not escape, nested boxes are added to canvas at user specifications
			if (inputCh != ESC)
			{
				if (inputCh == 'c' || inputCh == 'C')
				{
					pt.row = MAXROWS / 2;
					pt.col = MAXCOLS / 2;
				}
				addUndoState(undoList, redoList, current);
				drawBoxesRecursive(current->item, pt, nestedBoxHeight, animate);
			}
			break;

			// Tree option
		case 'T':
		case 't':
			int treeHeight, branchAngle;

			//User chooses height of fractal tree
			cout << "Enter approximate tree height: ";
			cin >> treeHeight;
			cin.ignore();

			//User chooses angle at which branches are added
			clearLine(MAXROWS + 1, sizeof(menu));
			cout << "Enter branch angle: ";
			cin >> branchAngle;
			cin.ignore();

			//User chooses point which tree trunk begins
			clearLine(MAXROWS + 1, sizeof(menu));
			cout << "Type any letter to choose start point, or <C> for bottom center / <ESC> to cancel";
			inputCh = getPoint(pt);

			//If user does not escape, tree will be added to canvas at user specifications
			if (inputCh != ESC)
			{

				if (inputCh == 'c' || inputCh == 'C')
				{
					pt.row = MAXROWS - 1;
					pt.col = MAXCOLS / 2;
				}
				//Add to current canvas to undoList
				addUndoState(undoList, redoList, current);

				treeRecursive(current->item, pt, treeHeight, 270, branchAngle, animate);
			}
			break;
		}//end switch
	} while (choice != 'M' && choice != 'm');
	//User returns back to main menu
}


// Manipulate pt varaiable, which is used by other functions to determine where to print to canvas
char getPoint(Point& pt)
{
	//Keyboard input from user
	char input;

	//Position on canvas before user moves cursor with arrowkeys
	int row = 0;
	int col = 0;

	// Move cursor to row,col and then get a single character from the keyboard
	gotoxy(row, col);
	input = _getch();

	// Gets input from keyboard until prinatable achii key or ESC is inputted
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
		// When user inputs character other than c, then pt will be upsloded with current curser position, and char returned.
		else if (input >= 32 && input <= 126)
		{
			pt.row = row;
			pt.col = col;
			return input;
		}
		// Moves cursor is arrow key was pressed, or keep cursor in same place in valid char was pressed.
		gotoxy(row, col);
		// Get next user input.
		input = _getch();
	}
	return ESC;
}


// Recursively fill a section of the screen
void fillRecursive(char canvas[][MAXCOLS], int row, int col, char oldCh, char newCh, bool animate)
{
	// Base case: in bounds of 2D array
	if ((row < MAXROWS && row >= 0) && (col < MAXCOLS && col >= 0) && (canvas[row][col] == oldCh))
	{
		// If current position has not been filled, fill it
		if (canvas[row][col] == oldCh)
		{
			Point pt; pt.row = row; pt.col = col;
			drawHelper(canvas, pt, newCh, animate);
			fillRecursive(canvas, row, col, oldCh, newCh, animate);
		}
		//Call the function for each adjacent position
		fillRecursive(canvas, row - 1, col, oldCh, newCh, animate);
		fillRecursive(canvas, row + 1, col, oldCh, newCh, animate);
		fillRecursive(canvas, row, col - 1, oldCh, newCh, animate);
		fillRecursive(canvas, row, col + 1, oldCh, newCh, animate);
	}
}


// Recursively draw a tree
void treeRecursive(char canvas[][MAXCOLS], DrawPoint start, int height, int startAngle, int branchAngle, bool animate)
{
	// Base case: tree only continues to print if branches are large enough
	if (height > 2)
	{
		//  each new "trunk" for every new tree created by new branches
		DrawPoint end = findEndPoint(start, height / 3, startAngle);
		drawLine(canvas, start, end, animate);

		// Creates new branch to the right, branch 2 less than last "trunk"
		treeRecursive(canvas, end, height - 2, branchAngle + startAngle, branchAngle, animate);
		// Creates new branch to the left, branch 2 less than last "trunk"
		treeRecursive(canvas, end, height - 2, startAngle - branchAngle, branchAngle, animate);
	}
}


// Recursively draw nested boxes
void drawBoxesRecursive(char canvas[][MAXCOLS], Point center, int height, bool animate)
{
	// Base case: if box is big enough to draw
	if (height > 1)
	{
		// Draws box
		drawBox(canvas, center, height, animate);
		// Function called rescursively
		drawBoxesRecursive(canvas, center, height - 2, animate);
	}
}