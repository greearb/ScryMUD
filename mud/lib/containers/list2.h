//
//ScryMUD Server Code
//Copyright (C) 1998  Ben Greear
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// To contact the Author, Ben Greear:  greear@cyberhighway.net, (preferred)
//                                     greearb@agcs.com
//

// list.h -- Declarations for the list library

//  This templated list works only on pointers right now, it could, however
//  be changed to include ALL items easily.  There may be a few bugs left,
//  so be careful.  If you have any questions, write me at:  
//  greear@cyberhighway.net
//  Happy coding, Ben Greear


 
#ifndef BEN_List2_Include
#define BEN_List2_Include

#include <stdlib.h> 
#include <iostream.h>
#include <fstream.h>
#include <LogStream.h>
#include <string2.h>

#ifndef TRUE

#define TRUE 1
#define FALSE 0 

#endif

// The list keeps a buffer of cells that are accessing it currently.
// This is used to try to keep the cells in a sane state when a Node
// is removed from the list.  If more than 5 cells are in a list at
// a time, an error message will be thrown, but it still may work.
// I would strongly recomend not doing this if at all possible though!
#define NUMBER_OF_CONCURENT_CELLS 10
 

extern LogStream mudlog;
extern int core_dump(const char* msg); //misc2.cc
extern int __node_cnt;
extern int __list_cnt;
extern int __cell_cnt;

 
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~




template <class T> class Cell;

template <class T> class List {
protected:
   friend class Cell<T>;
      //
      // Cell is a safe pointer to a list node
   
   class Node {
   public:
      Node *prev;
      Node *next;
      T     item; // Node holds an item of parameterized type T

      Node() : prev(NULL), next(NULL) {
         __node_cnt++;
      }

      Node(const Node& rhs) : prev(rhs.prev), next(rhs.next) {
         item = rhs.item;
         __node_cnt++;
      }//copy constructor

      ~Node() {
         __node_cnt--;
      }
   };//Node

   void handleLosingCell(Cell<T>& cll) {
      for (int i = 0; i<NUMBER_OF_CONCURENT_CELLS; i++) {
         if (cll_list[i] && (cll_list[i]->node == cll.node) &&
            (cll_list[i] != &cll)) {
            cll_list[i]->next(); //increment it through the list.
            // Assume there will always at least be the header node.
         }//if
      }//for
   }//handleLosingCell

   int handleRemoveCell(const Cell<T>& cll) {
      for (int i = 0; i<NUMBER_OF_CONCURENT_CELLS; i++) {
         if (cll_list[i] == &cll) {
            cll_list[i] = NULL;
            return TRUE;
         }//if
      }//for

      mudlog << "List<> ERROR:  could not remove cell: " << (void*)(&cll)
             << " " << toString() << endl;

      //::core_dump("handleRemoveCell");
      return FALSE;
   }//handleRemoveCell      


   int handleAddCell(Cell<T>& cll) {
      for (int i = 0; i<NUMBER_OF_CONCURENT_CELLS; i++) {
         if (cll_list[i] == NULL) {
            cll_list[i] = &cll;
            return TRUE;
         }//if
      }//for
      
      mudlog << "List<> ERROR:  could not add cell: " << (void*)(&cll)
             << " " << toString() << endl;

      //::core_dump("handleAddCell");
      return FALSE;
   }//handleAddCell


   Node *header;
   Cell<T>* cll_list[NUMBER_OF_CONCURENT_CELLS];

public:

   static int getNodeCnt() { return __node_cnt; }
   static int getListCnt() { return __list_cnt; }
   static int getCellCnt() { return __cell_cnt; }

   List  (const List<T> &L) { //copy constructor
      //mudlog << "In List<T> copy constructor, (L.this: " << (void*)(&L)
      //       << "): " << L.toString() << "\n" << flush;
      __list_cnt++;

      header = new Node;
      memset(cll_list, 0, NUMBER_OF_CONCURENT_CELLS * sizeof(Cell<T>*));
      
      //for (int i = 0; i<NUMBER_OF_CONCURENT_CELLS; i++) {
      //   cll_list[i] = NULL;
      //}


      if (!header) {
         //log("ERROR:  out of Memory: List2 constructor, exiting!\n");
         exit (101);
      }//if
      else {
         Cell<T> cll(L); 
         T ptr;

         header->item = (T)(NULL);
         header->next = header;
         header->prev = header;
  
         while ((ptr = cll.next())) { 
            append(ptr);
         }//while
      }//else

      //mudlog << "Done with List<T> copy constructor, L: " << L.toString()
      //       << "\n" << flush;
   } // copy constructor


   List  (const T &Nil) {
      __list_cnt++;
      header = new Node;

      memset(cll_list, 0, NUMBER_OF_CONCURENT_CELLS * sizeof(Cell<T>*));
      
      //for (int i = 0; i<NUMBER_OF_CONCURENT_CELLS; i++) {
      //   cll_list[i] = NULL;
      //}

      if (!header) { 
         //log("ERROR:  out of mem, list constructor, exiting!\n");
         exit (101);
      }//if
      else {
         header->item = Nil;
         header->next = header;
         header->prev = header;
      }//else

   } // constructor


   List  () {
      __list_cnt++;
      header = new Node;

      memset(cll_list, 0, NUMBER_OF_CONCURENT_CELLS * sizeof(Cell<T>*));
      //for (int i = 0; i<NUMBER_OF_CONCURENT_CELLS; i++) {
      //   cll_list[i] = NULL;
      //}

      if (!header) { 
         //log("ERROR:  out of mem, list constructor, exiting!\n");
         exit (101);
      }//if
      else {
         header->item = (T)(NULL);
         header->next = header;
         header->prev = header;
      }//else

   } // constructor


   // Destructor for List<T> -- Destroy the list, linear
   // DOES NOT delete the items it points too.
   virtual ~List () { 
      __list_cnt--;

      for (int i = 0; i<NUMBER_OF_CONCURENT_CELLS; i++) {
         if (cll_list[i]) {
            cll_list[i]->removeFromLst();
            cll_list[i] = NULL;
         }//if
      }//for

      this->clear();

      delete header;
   } //destructor 

   
   List<T> &operator= (const List &L) {
 
      if (this != &L) { // Don't copy yourself
         this->clear();       

         Cell<T> cll(L);
         T datum; 

         while ((datum = cll.next())) {
            append(datum);
         }//for
      }//if

      return *this;
   }//assignment operator

                 /* functions */

   /*  CLEAR--   removes every node in a List (except the header node
       of course), DOES NOT delete data pointed to */
   void clear() {
      Cell<T> cll(*this);
      T ptr = cll.next();
      while (ptr) {
         handleLosingCell(cll);
         ptr = cll.lose();
      }//while

      if (!header) {
         mudlog.log(ERR, "ERROR:  HEADER NULL in Clear.\n");
      }//if
   }// clear

   String toString() const {
      String retval(200);
      String tmp(50);

      Sprintf(retval, "this: %p  cells: ", this);
      for (int i = 0; i<NUMBER_OF_CONCURENT_CELLS; i++) {
         Sprintf(tmp, "[%i] %p ", i, cll_list[i]);
         retval.Append(tmp);
      }//for

      return retval;
   }//toString

   /*  LOSEDATA  finds and cuts the node with the specified data in it out 
       of the list.  Only cuts ONE instance, not multiple, if they exist. */
   T loseData (const T& data) {
      Cell<T> cell(*this);
      T ldata;

      Assert((int)data);

      ldata = cell.next();
      while (ldata) {
         if (ldata == data) {
            handleLosingCell(cell);
            cell.lose();  //this WILL NOT delete the item pointed to.
            return ldata;
         }//if
         ldata = cell.next();
      }//while
      return (T)(0);
   }//LoseData

 
   // Substitute_Data //
   /* If it doesn't find the target data, it will insert anyway 
      (at the end).  It returns FALSE if no substitution was made,
      TRUE otherwise.  Does not delete what it replaced. */
   int substituteData(const T& targ, const T& source, int j) {
      Cell<T> cell(*this);
      T ldata;
      int count = 0;

      while ((ldata = cell.next())) { 
         if (ldata == targ) {
            count++;
            if (count == j) {
               cell.node->item = source;
               return TRUE;
            }//if
         }//if
      }//while
      pushBack(source);
      return FALSE; //no substitution
   }//SubstituteData   


   int haveData (const T& data) const {
      Cell<T> cell(*this);
      T ldata;
      
      while ((ldata = cell.next())) {
         if (ldata == data) {
            return TRUE;
         }//if
      }//while
      return FALSE;
   }//HaveData   

   int dataCount(const T& data) const {
      Cell<T> cell(*this);
      T ldata;
      int retval = 0;
      
      while ((ldata = cell.next())) {
         if (ldata == data) {
            retval++;
         }//if
      }
      return retval;
   }//DataCount
 
   void pushFront(const T& data) {
      prepend(data);
   }

   void prepend(const T& data) {
      Cell<T> cell(*this);
      Assert((int)data);
      cell.insertAfter(data);
   }//push
 
   void pushBack(const T& data) {
      append(data);
   }

   void append(const T& data) {
      Cell<T> C(*this);
      Assert((int)data);
      C.insertBefore(data);
   }//put

   void gainData(const T& data) {
      if (!haveData(data)) {
         pushBack(data);
      }//if
   }//GainData

     
   /*  PULL--
       Returns the item contained at the BEGINNING of the list, it
       DOES delete the item from the list.  */
   T popFront() {
      Cell<T> cll(*this);
      T ret = cll.next();
   
      if (ret) {
         handleLosingCell(cll);
         cll.lose();
      }//if
      return ret;
   }//Pull
 
/*  DRAG--
    Returns the item contained at the END of the list, it
    DOES delete the item from the list.  */
   T popBack() {
      Cell<T> cll(*this);
      T ret = cll.prev();
      if (ret) {
         handleLosingCell(cll);
         cll.lose();
      }//if
      return ret;
   }//Drag



   int insertAt(int posn, T& val) {
      Cell<T> cll(*this);
      int i = 0;

      if (!val)
         return FALSE;

      T ptr;
      while ((ptr = cll.next())) {
         if (i == posn) {
            cll.insertBefore(val);
            return TRUE;
         }
         i++;
      }//while
      
      cll.insertBefore(val);
      return TRUE;
   }//insertAt

   T removeAt(int posn) {
      Cell<T> cll(*this);
      int i = 0;

      if (i < 0)
         return (T)(0);

      T ptr;
      while ((ptr = cll.next())) {
         if (i == posn) {
            handleLosingCell(cll);
            cll.lose();
            return ptr;
         }
         i++;
      }//while
      return (T)(0);
   }//removeAt

   T elementAt(int index) const {
      Cell<T> cll(*this);
      int i = 0;

      if (index < 0)
         return (T)(0);

      T ptr;
      while ((ptr = cll.next())) {
         if (i == index) {
            return ptr;
         }
         i++;
      }//while
      return (T)(0);
   }//elementAt

   /*  FRONT--
       Returns the item contained at the BEGINNING of the list, it
       DOES NOT delete the item from the list.  */
   T peekRear() const {
      return header->prev->item; 
   }//

   T peekBack() const {
      return peekRear();
   }//

   T peekFront() const {
      return header->next->item; 
   }//top
 

   /*  HEAD--  Make cll point to the head of the list. */
   void head(Cell<T>& cll) {
      if (cll.isInList(this)) {
         cll.node = header;
      }
      else {
         cll.clear(); //important, removes it from it's previous list.
         cll.node = header;
         cll.in_lst = (List<T>*)(this);
         handleAddCell(cll);
      }//else
   }//head

   int assign(Cell<T>& cll, const T& data) {
      Assert(cll.isInList(this));
      cll.assign(data);
      return TRUE;
   }//Assign

   void insertBefore(Cell<T>& cll, const T& data) {
      Assert(cll.isInList(this));
      cll.insertBefore(data);
   }//insertBefore

   void insertAfter(Cell<T>& cll, const T& data) {
      Assert(cll.isInList(this));
      cll.insertAfter(data);
   }//insertBefore

   T lose(Cell<T>& cll) {
      if (cll.node != header) {
         Assert(cll.isInList(this));
         handleLosingCell(cll);
         return cll.lose();
      }//if
      else {
         mudlog << "ERROR:  Trying to lose header, this: " << this
                << " Cell<T>: " << (void*)(&cll) << endl;
         return (T)(0);
      }//else
   }//insertBefore

   int isEmpty() const {
      Assert((int)header);
      return (header->next == header);
   }//IsEmpty

   /*  SIZE--  Returns the number of cells contained in a List.  */
   int size() const {
      class List<T>::Node* node_ptr = header->next;
      int count = 0;
   
      while (node_ptr != header) {
         count++;
         node_ptr = node_ptr->next;
      }//while
      return count;
   }//size

   int Assert(const int boolean_val) const {
      if (!boolean_val)
         ::core_dump((const char*)("List2.h"));
      return TRUE;
   }

   friend void clear_ptr_list(List<T> &L);  //DOES delete the data
   friend void Put(const T& val, List<T> &L);
   friend T Top(const List<T> &L);
   friend int IsEmpty(const List<T> &L);
   friend int HaveData(const T& val, const List<T>& L);
};

// A few of these are back wards compatable hacks

template <class T> int HaveData(const T& val, const List<T>& L) {
   return L.haveData(val);
}

template <class T> int IsEmpty(const List<T>& L) {
   return L.isEmpty();
}

template <class T> T Top(const List<T>& L) {
   return L.peekFront();
}

//on the end of the list
template <class T> void Put(const T& val, List<T>& L) {
   L.append(val);
}

template <class T> void clear_ptr_list(List<T>& L) {
   Cell<T> cll(L);
   T tmp = cll.next();
   while (tmp) {
      delete tmp;
      tmp = L.lose(cll);
   }//while
}//


 
///************************************************************************///
///*************************  Cell  ***************************************///
///************************************************************************///
 
template <class T> class Cell {
private:

   // Before implementing this, think out the case where one cell is in
   // a list, and is coppied.  The list doesn't know of the new cell.
   // This can be fixed if it proves needed. --BEN
   Cell<T>& operator=(const Cell<T>& src);

   // Same issue with this guy.
   Cell (const Cell<T>& src);// :
      //node(src.node), in_lst(src.in_lst) {
      //__cell_cnt++; 
      //in_lst->handleAddCell(*this);
   //}//constructor


protected:
   friend class List<T>;

   List<T>::Node *node;
   List<T>* in_lst;

   void insertBefore (const T& data) {
      Assert((int)node);
      List<T>::Node *N = new List<T>::Node; 
      if (!N) {
         mudlog.log(ERR, "ERROR, out of Memory trying to allocate a cell.\n");
         exit (101);
      }//if
      N->item = data;
      N->prev = node->prev;
      N->next = node;
      node->prev->next = N;
      node->prev = N;
   }// insert before

   assign(const T& data) {
      Assert(node && data);
      node->item = data;
      return TRUE;
   }//Assign


   /*  INSTERT_AFTER-- Insert an item into a list after a given cell. */
   void insertAfter(const T& data) {
      Assert((int)node);
      List<T>::Node *N = new List<T>::Node; 
      N->item = data;
      N->prev = node;
      N->next = node->next;
      node->next = N;
      N->next->prev = N;
   }// insert after
 
   /*  LOSE   Deletes a node, sets cell equal to next cell,  
       returns what the new cell contains.  Can only be called by
       List<>, and that takes care of keeping other Cell<> iterators
       cleaned up. */
   T lose() {
      class List<T>::Node* node_ptr = node;    
      
      node->prev->next = node->next;
      node->next->prev = node->prev;
      
      node = node->next;
      if (node_ptr->item)
         delete node_ptr;
      return node->item;
   }// Lose

   void removeFromLst() {
      in_lst = NULL;
   }

public:
   
   // Constructors
   //
   Cell () { node = NULL; __cell_cnt++; in_lst = NULL; }

   Cell (const List<T>& L) : node(L.header) {
      __cell_cnt++;
      in_lst = (List<T>*)(&L);
      in_lst->handleAddCell(*this);
   }//constructor

   virtual ~Cell() {
      __cell_cnt--;
      clear(); //this is important, allows the list to clean *this up.
   }//destructor

   void clear() {
      if (in_lst) {
         in_lst->handleRemoveCell(*this);
         in_lst = NULL;
      }//if
   }

   int Assert(const int boolean_val) const {
      if (!boolean_val) {
         ::core_dump("Cell<T>");
         return FALSE;
      }
      else
         return TRUE;
   }//Assert

   /*  NEXT--  next(cell)  
       This function sets "cell" equal to the next cell in the list and
       returns this cell's item.  */
   T next() {
      node = node->next;
      return node->item;
   }//next

/*  PREV--   prev(cell)  
    This function sets "cell" equal to the previous cell in the list
    and returns the new cell's item.  */
   T prev() {
      node = node->prev;
      return node->item;
   }//prev
 

   /*  ITEM-- returns the data stored in the target cell, the cell
       is NOT changed. */
   T item () const { return node->item; }//item
 
   List<T>* getInList() { return in_lst; }

   int isInList(List<T>* lst) const { return (in_lst == lst); }

};

#endif
