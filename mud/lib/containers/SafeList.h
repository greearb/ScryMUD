// $Id: SafeList.h,v 1.3 1999/08/09 06:00:40 greear Exp $
// $Revision: 1.3 $  $Author: greear $ $Date: 1999/08/09 06:00:40 $

//
//ScryMUD Server Code
//Copyright (C) 1999  Ben Greear
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

// The list keeps a buffer of cells that are accessing it currently.
// This is used to try to keep the cells in a sane state when a Node
// is removed from the list.  If more than 5 cells are in a list at
// a time, an error message will be thrown, but it still may work.
// I would strongly recomend not doing this if at all possible though!
#define NUMBER_OF_CONCURENT_SCELLS 10

extern LogStream mudlog;
extern int core_dump(const char* msg); //misc2.cc
extern int __s_node_cnt;
extern int __s_list_cnt;
extern int __s_cell_cnt;


template <class T> class SCell;
template <class T> class SafeList;

/**  This class contains objects.  The container knows all of its
 * objects, and the objects know all of their containers, so hopefully
 * memory management will be cleaner and wrapped up at this lower layer.
 *  The object type T must implement a few special methods, which I wrap
 * up in the ContainedObject class.
 */
template <class T> class SafeList {
protected:
   friend class SCell<T>;
   
   class Node {
   public:
      Node *prev;
      Node *next;
      T     item; // Node holds an item of parameterized type T

      Node() : prev(NULL), next(NULL) { __s_node_cnt++; }
      Node(const Node& rhs) : prev(rhs.prev), next(rhs.next) {
         item = rhs.item; __s_node_cnt++;
      }//copy constructor

      ~Node() { __s_node_cnt--; }
   };//Node

   virtual void handleLosingSCell(SCell<T>& cll) {
      for (int i = 0; i<NUMBER_OF_CONCURENT_SCELLS; i++) {
         if (cll_list[i] && (cll_list[i]->node == cll.node) &&
            (cll_list[i] != &cll)) {
            cll_list[i]->next(); //increment it through the list.
            // Assume there will always at least be the header node.
         }//if
      }//for
   }//handleLosingSCell

   virtual int handleRemoveSCell(const SCell<T>& cll) {
      for (int i = 0; i<NUMBER_OF_CONCURENT_SCELLS; i++) {
         if (cll_list[i] == &cll) {
            cll_list[i] = NULL;
            return TRUE;
         }//if
      }//for

      mudlog << "List<> ERROR:  could not remove scell: " << (void*)(&cll)
             << " " << toString() << endl;

      ::core_dump("handleRemoveSCell");
      return FALSE;
   }//handleRemoveSCell      


   virtual int handleAddSCell(SCell<T>& cll) {
      for (int i = 0; i<NUMBER_OF_CONCURENT_SCELLS; i++) {
         if (cll_list[i] == NULL) {
            cll_list[i] = &cll;
            return TRUE;
         }//if
      }//for
      
      mudlog << "List<> ERROR:  could not add scell: " << (void*)(&cll)
             << " " << toString() << endl;

      ::core_dump("handleAddSCell");
      return FALSE;
   }//handleAddSCell


   Node *header;
   int sz;
   SCell<T>* cll_list[NUMBER_OF_CONCURENT_SCELLS];

public:

   static int getNodeCnt() { return __s_node_cnt; }
   static int getListCnt() { return __s_list_cnt; }
   static int getSCellCnt() { return __s_cell_cnt; }

   const T getNil() const { return header->item; }

   /**  Only makes a shallow copy. */
   SafeList(SafeList<T> &L) : sz(0) { //copy constructor
      __s_list_cnt++;

      header = new Node;
      header->item = L.getNil();
      header->next = header;
      header->prev = header;
      memset(cll_list, 0, NUMBER_OF_CONCURENT_SCELLS * sizeof(SCell<T>*));
      
      SCell<T> cll(L); 
      T ptr;
      while ((ptr = cll.next()) != getNil()) { 
         append(ptr);
      }//while
   } // copy constructor


   SafeList(T Nil) : sz(0) { //Compiler doesn't like T& Nil
      __s_list_cnt++;
      header = new Node;
      header->item = Nil;
      header->next = header;
      header->prev = header;

      memset(cll_list, 0, NUMBER_OF_CONCURENT_SCELLS * sizeof(SCell<T>*));
   } // constructor


   /** Destructor for SafeList<T> -- Destroy the list, linear
       DOES NOT delete the items it points too. */
   virtual ~SafeList () { 
      __s_list_cnt--;

      for (int i = 0; i<NUMBER_OF_CONCURENT_SCELLS; i++) {
         if (cll_list[i]) {
            cll_list[i]->removeFromLst();
            cll_list[i] = NULL;
         }//if
      }//for

      this->clear();
      delete header;
   } //destructor 

   /** Shallow copy of data contained, full copy of all cells. */
   virtual SafeList<T> &operator= (SafeList &L) {
      
      if (this != &L) { // Don't copy yourself
         this->clear();       
         header->item = L.getNil();

         SCell<T> cll(L);
         T datum;
         while ((datum = cll.next()) != getNil()) {
            append(datum);
         }//for
      }//if

      return *this;
   }//assignment operator

                 /* functions */

   /**  CLEAR--   removes every node in a SafeList (except the header node
       of course), DOES NOT delete data pointed to */
   virtual void clear() {
      while (!isEmpty()) {
         popFront();
      }
      sz = 0;
   }// clear

   virtual String toString() {
      String retval(200);
      String tmp(50);

      Sprintf(retval, "this: %p  sz: %i  size: %i cells: ", this, sz, size());
      for (int i = 0; i<NUMBER_OF_CONCURENT_SCELLS; i++) {
         Sprintf(tmp, "[%i] %p ", i, cll_list[i]);
         retval.Append(tmp);
      }//for

      return retval;
   }//toString

   /*  LOSEDATA  finds and cuts the node with the specified data in it out 
    *  of the list.  Removes only one instance of the data from the list.
    */
   virtual T loseData (T& data) {
      SCell<T> cell(*this);
      T ldata;

      Assert(data != getNil());

      ldata = cell.next();
      while (ldata != getNil()) {
         if (ldata == data) {
            handleLosingSCell(cell);
            cell.lose();  //this WILL NOT delete the item pointed to.
            sz--;
            return ldata;
         }//if
         ldata = cell.next();
      }//while
      return getNil();
   }//LoseData


   /*  Don't call this unless you really understand what it does.
    *  NOTE:  The 'const' is a lie, but it makes the compiler not put
    *  out a warning about passing references of lvalues. --Ben
    */
   virtual T privRemoveObject__(T& data) {
      SCell<T> cell(*this);
      T ldata;

      Assert(data != getNil());

      ldata = cell.next();
      while (ldata != getNil()) {
         if (ldata == data) {
            handleLosingSCell(cell);
            cell.lose(TRUE);  //this WILL NOT delete the item pointed to.
            sz--;
            return ldata;
         }//if
         ldata = cell.next();
      }//while
      Assert(0);
      return getNil(); //never get here.
   }//privRemoveObject__

 
   // Substitute_Data //
   /** If it doesn't find the target data, it will insert anyway 
      (at the end).  It returns FALSE if no substitution was made,
      TRUE otherwise.  Does not delete what it replaced. */
   virtual int substituteData(T& targ, T& source, int j) {
      SCell<T> cell(*this);
      T ldata;
      int count = 0;

      while ((ldata = cell.next())) { 
         if (ldata == targ) {
            count++;
            if (count == j) {
               cell.assign(source);
               return TRUE;
            }//if
         }//if
      }//while
      append(source);
      return FALSE; //no substitution
   }//SubstituteData   


   virtual int haveData (const T& data) {
      SCell<T> cell(*this);
      T ldata;
      
      while ((ldata = cell.next()) != getNil()) {
         if (ldata == data) {
            return TRUE;
         }//if
      }//while
      return FALSE;
   }//HaveData   

   virtual int dataCount(T& data) {
      SCell<T> cell(*this);
      T ldata;
      int retval = 0;
      
      while ((ldata = cell.next()) != getNil()) {
         if (ldata == data) {
            retval++;
         }//if
      }
      return retval;
   }//DataCount
 
   virtual void prepend(T& data) {
      SCell<T> cell(*this);
      Assert(data != getNil());
      cell.insertAfter(data);
      sz++;
   }//push
 
   virtual void append(T& data) {
      SCell<T> C(*this);
      Assert(data != getNil());
      C.insertBefore(data);
      sz++;
   }//put

   virtual void appendUnique(T& data) {
      if (!haveData(data)) {
         append(data);
      }//if
   }//GainData

     
   /** Returns the item contained at the BEGINNING of the list, it
       DOES delete the item from the list.  */
   virtual T popFront() {
      SCell<T> cll(*this);
      T ret = cll.next();
   
      if (ret != getNil()) {
         handleLosingSCell(cll);
         sz--;
         cll.lose();
      }//if
      return ret;
   }//Pull
 
/** Returns the item contained at the END of the list, it
    DOES delete the item from the list.  */
   virtual T popBack() {
      SCell<T> cll(*this);
      T ret = cll.prev();
      if (ret != getNil()) {
         handleLosingSCell(cll);
         sz--;
         cll.lose();
      }//if
      return ret;
   }//Drag

   virtual int insertAt(int posn, T& val) {
      SCell<T> cll(*this);
      int i = 0;

      Assert(val != getNil());

      sz++;
      T ptr;
      while ((ptr = cll.next()) != getNil()) {
         if (i == posn) {
            cll.insertBefore(val);
            return TRUE;
         }
         i++;
      }//while
      
      cll.insertBefore(val);
      return TRUE;
   }//insertAt

   virtual T removeAt(int posn) {
      SCell<T> cll(*this);
      int i = 0;

      if (i < 0)
         return getNil();

      T ptr;
      while ((ptr = cll.next()) != getNil()) {
         if (i == posn) {
            handleLosingSCell(cll);
            cll.lose();
            sz--;
            return ptr;
         }
         i++;
      }//while
      return getNil();
   }//removeAt

   virtual T elementAt(int index) {
      SCell<T> cll(*this);
      int i = 0;

      if (index < 0)
         return getNil();

      T ptr;
      while ((ptr = cll.next()) != getNil()) {
         if (i == index) {
            return ptr;
         }
         i++;
      }//while
      return getNil();
   }//elementAt

   /**  Returns the item contained at the End of the list, it
       DOES NOT delete the item from the list.  */
   virtual T peekRear() {
      return header->prev->item; 
   }//

   /**  Returns the item contained at the Beginning of the list, it
       DOES NOT delete the item from the list.  */
   virtual T peekFront() {
      return header->next->item; 
   }//top
 

   /*  HEAD--  Make cll point to the head of the list. */
   virtual void head(SCell<T>& cll) {
      if (cll.isInSafeList(this)) {
         cll.node = header;
      }
      else {
         cll.clear(); //important, removes it from it's previous list.
         cll.node = header;
         cll.in_lst = (SafeList<T>*)(this);
         handleAddSCell(cll);
      }//else
   }//head

   virtual int assign(SCell<T>& cll, T& data) {
      Assert(cll.isInSafeList(this));
      cll.assign(data);
      return TRUE;
   }//Assign

   virtual void insertBefore(SCell<T>& cll, T& data) {
      Assert(cll.isInSafeList(this));
      sz++;
      cll.insertBefore(data);
   }//insertBefore

   virtual void insertAfter(SCell<T>& cll, T& data) {
      Assert(cll.isInSafeList(this));
      sz++;
      cll.insertAfter(data);
   }//insertBefore

   virtual T lose(SCell<T>& cll) {
      if (cll.node != header) {
         Assert(cll.isInSafeList(this));
         handleLosingSCell(cll);
         sz--;
         return cll.lose();
      }//if
      else {
         mudlog << "ERROR:  Trying to lose header, this: " << this
                << " SCell<T>: " << (void*)(&cll) << endl;
         Assert(0); //Dump core, need to debug this.
         return getNil(); //never get here.
      }//else
   }//insertBefore

   virtual int isEmpty() const {
      Assert((int)header);
      return (header->next == header);
   }//IsEmpty

   /*  size--  Returns the number of cells contained in a SafeList.  */
   virtual int size() {
      class SafeList<T>::Node* node_ptr = header->next;
      int count = 0;
   
      while (node_ptr != header) {
         count++;
         node_ptr = node_ptr->next;
      }//while

      // Leave this in for a while till we're sure it's debugged!!
      if (count != sz) {
         mudlog << "ERROR: sz: " << sz << " does not match calculated size: "
                << count << endl;
      }
      return count;
   }//sz

   virtual int Assert(const int boolean_val) const {
      if (!boolean_val)
         ::core_dump((const char*)("SafeList2.h"));
      return TRUE;
   }
};

template <class T> int addedToList(SafeList<T>* lst, T& data);
template <class T> int removedFromList(SafeList<T>* lst, T& data);

///************************************************************************///
///*************************  SCell  ***************************************///
///************************************************************************///
 
template <class T> class SCell {
private:

   // Before implementing this, think out the case where one cell is in
   // a list, and is coppied.  The list doesn't know of the new cell.
   // This can be fixed if it proves needed. --BEN
   SCell<T>& operator=(const SCell<T>& src);

   // Same issue with this guy.
   SCell (const SCell<T>& src);// :
      //node(src.node), in_lst(src.in_lst) {
      //__cell_cnt++; 
      //in_lst->handleAddSCell(*this);
   //}//constructor


protected:
   friend class SafeList<T>;

   SafeList<T>::Node *node;
   SafeList<T>* in_lst;

   virtual void insertBefore (T& data) {
      Assert((int)node);
      SafeList<T>::Node *N = new SafeList<T>::Node; 
      N->item = data;
      N->prev = node->prev;
      N->next = node;
      node->prev->next = N;
      node->prev = N;
      addedToList(in_lst, data);
   }// insert before

   virtual int assign(T& data) {
      Assert(node && data);
      removedFromList(in_lst, node->item);
      node->item = data;
      addedToList(in_lst, data);
      return TRUE;
   }//Assign


   /**  INSTERT_AFTER-- Insert an item into a list after a given cell. */
   virtual void insertAfter(T& data) {
      Assert((int)node);
      SafeList<T>::Node *N = new SafeList<T>::Node; 
      N->item = data;
      N->prev = node;
      N->next = node->next;
      node->next = N;
      N->next->prev = N;
      addedToList(in_lst, data);
   }// insert after
 
   /**  LOSE   Deletes a node, sets cell equal to next cell,  
       returns what the new cell contains.  Can only be called by
       SafeList<>, and that takes care of keeping other SCell<> iterators
       cleaned up.  Only set dont_tell_item to true when doing the
       privLoseObject, which should be treated as a private method
       by most user code.
   */
   virtual T lose(int dont_tell_item = FALSE) {
      class SafeList<T>::Node* node_ptr = node;    
      
      node->prev->next = node->next;
      node->next->prev = node->prev;
      node = node->next;
      
      if (!dont_tell_item) {
         removedFromList(in_lst, node_ptr->item);
      }
      delete node_ptr;
      return node->item;
   }// Lose

   virtual void removeFromLst() {
      in_lst = NULL;
   }

public:
   SCell () : node(NULL), in_lst(NULL) { __s_cell_cnt++; }
   SCell (SafeList<T>& L) : node(L.header) {
      __s_cell_cnt++;
      in_lst = (SafeList<T>*)(&L);
      in_lst->handleAddSCell(*this);
   }//constructor

   virtual ~SCell() {
      __s_cell_cnt--;
      clear(); //this is important, allows the list to clean *this up.
   }//destructor

   virtual void clear() {
      if (in_lst) {
         in_lst->handleRemoveSCell(*this);
         in_lst = NULL;
      }//if
   }

   int Assert(const int boolean_val) const {
      if (!boolean_val) {
         ::core_dump("SCell<T>");
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

   /**  PREV--   prev(cell)  
        This function sets "cell" equal to the previous cell in the list
        and returns the new cell's item.  */
   T prev() {
      node = node->prev;
      return node->item;
   }//prev
 

   /**  ITEM-- returns the data stored in the target cell, the cell
       is NOT changed. */
   T item () const { return node->item; }//item 

   SafeList<T>* getInSafeList() { return in_lst; }

   int isInSafeList(SafeList<T>* lst) const { return (in_lst == lst); }

};


template <class T> class SafePtrList : public SafeList<T*> {
public:
   SafePtrList() : SafeList<T*>((T*)NULL) { }
   SafePtrList(SafePtrList<T> &L) : SafeList<T*>(L) { }
   SafePtrList(T &Nil) : List<T*>(Nil) { }
   virtual ~SafePtrList() { }

   /** NOTE:  does a clearAndDestroy on self before starting on copying
    * the other.
    */
   void becomeDeepCopyOf(SafePtrList<T>& lst) {
      clearAndDestroy();
      SCell<T*> cll(lst);
      T* ptr;
      T* ptr2;
      while ((ptr = cll.next())) {
         ptr2 = new T(*ptr); //Adding this extra line makes the compiler happier.
         append(ptr2);
      }//while
   }//becomeDeepCopyOf

   void clearAndDestroy() {
      T* val;
      while (!isEmpty()) {
         val = popFront();
         delete val;
      }//while
   }//clearAndDestroy

};//SafePtrList

