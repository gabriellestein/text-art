#include <iostream>
#include <Windows.h>
#include "Definitions.h"
using namespace std;

//Create new canvas to be manipulated by user
Node* newCanvas()
{
	//Creates new node
	Node* current = new Node;
	//Initilizes canvas with spaces
	initCanvas(current->item);

	//There is no node after current so next is NULL
	current->next = NULL;

	//Returns node with initilized canvas
	return current;
}

//newCanvas(oldNode) is used the same was as copyCanvas, but with nodes.
Node* newCanvas(Node* oldNode)
{
	//Creates new node that will hold item from oldNode
	Node* current = new Node;
	copyCanvas(current->item, oldNode->item);
	current->next = NULL;
	//Returns copy of oldNode
	return current;
}

//play function is helper function that startes recursivePlay
void play(List& clips)
{

	// loops as long as the ESCAPE key is not currently being pressed
	while (!(GetKeyState(VK_ESCAPE) & 0x8000))
	{

		if (clips.count >= 2)
		{
			//calls playRerucsive, will continue until ESC is held
			playRecursive(clips.head, clips.count);
		}
	}
}

//playRecurvsive keeps cycling through all clips currently loaded until user holds ESC
void playRecursive(Node* head, int count)
{
	// If list is empty, do not play
	if (head == NULL)
	{
		return;
	}

	// Loops through all clips
	playRecursive(head->next, count - 1);

	// Stops looping through clips in ESC is held
	if((GetKeyState(VK_ESCAPE) & 0x8000))
	{
		return;
	}

	displayCanvas(head->item);

	printf("Hold <ESC> key to Quit \tClips: %3d", count);
	// Pause for 100 milliseconds to slow down animation
	Sleep(100);

}

//addUndoState function adds a copy of current canvas to undoList before changes are made to previous states are saved
void addUndoState(List& undoList, List& redoList, Node*& current)
{
	//adds current canvas into undoList node
	addNode(undoList, newCanvas(current));
	//Removes all save states saved in redoList as changes cannot be redone when new changes are made
	deleteList(redoList);
}

//restoreFunction is to undo or redo changes made to canvas
void restore(List& undoList, List& redoList, Node*& current)
{
	if (undoList.count > 0)
	{
		//Sets current canvas to canvas that was present before changes were undone
		addNode(redoList, current);
		current = removeNode(undoList);
	}
}

//addNode function adjusts points to add new nodes to list
void addNode(List& list, Node* nodeToAdd)
{
	//Points new node towards node that is currently first
	nodeToAdd->next = list.head;
	//Sets new node to be first
	list.head = nodeToAdd;
	//Increases list count
	list.count++;
}

//removeNode function is meant to remove first node in a list and decrease list count
Node* removeNode(List& list)
{
	Node* temp = list.head;
	//Deletes that first node in the list
	if (list.head != NULL)
	{
		list.head = temp->next;
		temp->next = NULL;
		list.count--;
	}
	return temp;
}

//deleteList function is meant to removed ever node of a fuction until the head of the list is NULL
void deleteList(List& list)
{
	// Iterates through entire linked list, removing every node
	while (list.head != NULL)
	{
		Node* temp = list.head->next;
		delete list.head;
		list.head = temp;
	}
	list.count = 0;
}

//loadClips function loads all clips of an animation that has already been saved
bool loadClips(List& clips, char filename[])
{
	deleteList(clips);

	char filepath[FILENAMESIZE];
	// Clips number currently being loded
	int clipNum = 1;

	bool load;

	do
	{
		Node* clip = newCanvas();
		// Formats filename so that it can be opened
		snprintf(filepath, FILENAMESIZE, "%s-%d.txt", filename, clipNum);
		load = loadCanvas(clip->item, filepath);
		if(load)
		{
			addNode(clips, clip);
			clipNum++;
		}
		else
		{
			delete clip;
		}
	} while (load);

	// Returns true if all clips were able to be loaded
	if (clips.count == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

//saveClips function is meant to save all clips of an animation
bool saveClips(List& clips, char filename[])
{
	char filepath[FILENAMESIZE];
	//value for number of clips
	int clipCount = clips.count;
	bool save;
	
	Node* head = clips.head;
	
	// Iterates through clips
	while(head != NULL)
	{
		snprintf(filepath, FILENAMESIZE, "%s-%d.txt", filename, clipCount);
		// Creates new node that will hold new clip
		save = saveCanvas(head->item, filepath);
		clipCount--;
		if (!save) {
			return false;
		}
		head = head->next;
	}
	// Returns true if all clips were able to be saved
	return true;
}