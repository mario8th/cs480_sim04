#ifndef __libllist_llist_h__
#define __libllist_llist_h__

#include <stdlib.h>

// ----- List and list node declarations ----------------------------------- //
struct ListNode;

typedef struct ListNode {
	struct ListNode *next;
	struct ListNode *prev;
	void *value;
} ListNode;

typedef struct List {
	int count;
	ListNode *first;
	ListNode *last;
} List;

// ----- Function prototypes ----------------------------------------------- //
/*
  Just initializes the List structure (the node pointers are NULL and
  the count = 0.
*/
List *list_create();

/*
  Just initializes a ListNode structure (the node pointers are NULL and
  the pointer to the data is set to payload.
*/
ListNode *list_create_node( void *payload );

/*
  Frees any nodes in the list and then frees the list structure.
*/
List *list_destroy( List *list );

/*
  Frees any nodes in the list but leaves the list structure.
*/
void list_clear( List *list );

/*
  Returns the count in the list structure.
*/
int list_count( List *list );

/*
  Returns the first node in the list.
*/
ListNode *list_first( List *list );

/*
  Returns the last node in the list.
*/
ListNode *list_last( List *list );

/*
  Finds a node in the list by value and returns the pointer to the node.
  If no matching node is found, returns NULL.
*/
ListNode *list_find( List *list, void *value );

/*
  Inserts a node in the list after the node containing value.  If no node
  has the passed value or if value == NULL, insert at the end of the list.
*/
void list_insert_after( List *list, ListNode *node, void *value );

/*
  Inserts a node in the list before the node containing value.  If no node has
  the passed value or if value == NULL, insert at the beginning of the list.
*/
void list_insert_before( List *list, ListNode *node, void *value );

/*
  Removes the specified node from the list and frees it.  The node's value
  is saved and returned.
*/
void *list_remove_node( List *list, ListNode *node );

/*
  Removes the node with the specified value from the list.  The node's value
  is saved and returned.  If a node with the value is not found in the list,
  return NULL
*/
void *list_remove_value( List *list, void *value );

// ----- The following functions are not needed until Project 5 ------------ //
/*
  Adds a node at one end of the list (your choice but document it).
*/
void list_push( List* list, void* value );

/*
  Add a node at one end of the list (your choice, but document it).
*/
void list_enqueue( List* list, void* value );

/*
  Removes a node from the same end of the list as list_push(). The value
  of the removed node is returned.
*/
void *list_pop( List* list );

/*
  Removes a node from the opposite end of the list as list_enqueue().  The
  value of the removed node is returned.
*/
void *list_dequeue( List* list );

/*
  L = a list variable.
  S = a node in the List structure (either 'first' or 'last').
  M = a node in the ListNode structure (either 'next' or 'prev').
  V = a variable name for the iterator.

  Given:
    // myList is a List* variable to a list of nodes
    LIST_FOREACH(myList, first, next, cur){
	  printf("%s\n", (char *)cur->value);
    }

  the following macro expands to:
    ListNode* _node = NULL;
    ListNode* V = NULL;
    for (V = _node = myList->first; _node != NULL; cur = _node = _node->next){
	  printf("%s\n", (char *)cur->value);
    }

    NOTE that _node and the variable name for V are declared.  If you use the
    macro twice within scope, you will get an error about redefining those two
    variables.  Once defined, the iterator may be used freely as in the printf
    statement in the above example.
*/
#define LIST_FOREACH( L, S, M, V ) ListNode* _node = NULL;\
		ListNode* V = NULL;\
		for ( V = _node = L-> S; _node != NULL; V = _node = _node->M )

#endif
