// $Id$
// $Revision: 1.3 $  $Author$ $Date$

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

///**********************  tree2.h  *********************************///
/* This is the most heinous piece of code I have ever produced, other than
   that failed attempt at a suffix tree.  Hope I can get it working. */


#ifndef TREE_2_INCLUDE
#define TREE_2_INCLUDE

template <class T> class Tree2Node;
template <class T> class Tree2;

template <class T> class Tree2Cell {
   
   friend class Tree2Node<T>;
   friend class Tree2<T>;
   
private:
   Tree2Node<T>* ptr;
   
public:
   
   operator const int() const {
      if (ptr)
         return TRUE;
      return FALSE;
   }
   
   Tree2Cell(Tree2<T>& tree) { ptr = tree.root; }
   Tree2Cell(Tree2Node<T>& node) { ptr = &node; }
   Tree2Cell() { ptr = NULL; }
   
   void Head(Tree2<T>& tree) { ptr = tree.root;}
   void Head(Tree2Node<T>& nd) { ptr = &nd; }
   
   T& Data() { 
      if (ptr) {
         return ptr->data;
      }//if
      //log("TRACK: ERROR:  FATAL trying to get->NULL data in Data().\n");
      exit(102);
   }//Data()


   /* return current value, THEN increment by one */
   T& Next_Breadth() {	//will crash if let run too far
      if (!ptr) {
         //log("TRACK:  ERROR:  ptr is NULL, Next_Breadth.\n");
         exit(102);
      }//if
      T* tmp_ptr = &(ptr->data);
      ptr = ptr->next;
      return *tmp_ptr;
   }
   
   T& Parent() {		//will crash if let run too far
      if (!ptr) {
         //log("TRACK:  ERROR:  ptr is NULL, Parent.\n");
         exit(102);
      }//if
      T* tmp_ptr = &(ptr->data);
      ptr = ptr->parent;
      return *tmp_ptr;
   }


   /* This next one should add a child to the first of the children
      of the Tree2Node pointed to by the current TreeCell.
      A pointer to the just inserted Tree2Node will be returned. */
   
   Tree2Node<T>* Push_Child(T& dat) const {
      Tree2Node<T>* tmp, *tmp_par;
      
      if (!ptr) {
         //log("TRACK:  ERROR:  Tree2Cell is NULL in Push_Child()\n");
         return NULL;
      }//if
      else if (!ptr->prev && !ptr->next) { //if empty tree
         //log("TRACK:  inserting in an empty tree.\n");
         ptr->Insert_After(ptr, ptr->level + 1, dat);
         ptr->first = ptr->next;
         return ptr->next;
      }//else
      else if (ptr->first) { //if has a first child
         ptr->first->Insert_Before(ptr, ptr->level + 1, dat);
         ptr->first = ptr->first->prev; //new one is now first
         return ptr->first;
      }//if
      else { //no immediate children of this node,
         /* If previous sibs have children, put it after them,
            else put it after last sibling */
         /* first, check for previous sibs with children */
         
         tmp = ptr;// tmp points to current tree node
         
         while (TRUE) {
	    if (tmp->prev->level != tmp->level) { //left most end, no dice
               break;
            }//if
	    if (tmp->first) { //found a child, good to go
               tmp_par = tmp;
               tmp = tmp->first;
               while (TRUE) {
                  if (!tmp->next) { // end of tree, good do it here
                     break;
                  }//if
                  if (tmp->next->parent != tmp_par) { //also good, end of 
                     //tmp_par's children
                     break;
                  }//if
                  tmp = tmp->next;  //keep walking foward through children
               }//while
               /* now we are at posn */
               tmp->Insert_After(ptr, ptr->level + 1, dat);
               ptr->first = tmp->next;
               return ptr->first;
	    }//if
            tmp = tmp->prev; //keep walking left through cibs
         }//while
         
         /* if got to here, then at left most cib, no children.
            Must now find right most cib and place the new node
            after it, with 'ptr' as its parent. */
         
         while (TRUE) {
            if (!tmp->next) { //if last one then done
               break;
            }//if
            if (tmp->level != tmp->next->level) { //at end of level, done
               break;
            }//if
            tmp = tmp->next; //walk on down cibs
         }//while
         /* ok, good to go, insert after tmp */
         tmp->Insert_After(ptr, ptr->level + 1, dat);
         ptr->first = tmp->next; //original ptr gets child
         return ptr->first;
      }//else no immediate children
   }//Push_Child()		  
};//Tree2Cell


template <class T> class Tree2Node {

   friend class Tree2Cell<T>;
   friend class Tree2<T>;

private:
   short 	level;
   Tree2Node<T>* first;
   Tree2Node<T>* parent;
   T 	data;
   Tree2Node<T>* next;
   Tree2Node<T>* prev;
   
public:
   Tree2Node(int lvl, Tree2Node<T>* frst, Tree2Node<T>* par, T& dat,
             Tree2Node<T>* nxt, Tree2Node<T>* prv) { 
      level = lvl;
      first = frst; 
      parent = par;
      data = dat;
      next = nxt;
      prev = prv; 
   }
   
   Tree2Node() { parent = first = next = prev = NULL; level = 0; }
   
   
   ~Tree2Node() {
      delete next;
      next = prev = first = parent = NULL;
   }//destructor
   
   T& Data() { return data; }
   
   void Insert_Before(Tree2Node<T>* par, short lvl, T& data) {
      Tree2Node<T>* nn = new Tree2Node<T>(lvl, NULL, par,
                                          data, NULL, NULL);
      if (this->prev) {
         nn->prev = this->prev;
         this->prev = nn;
         nn->prev->next = nn;
         nn->next = this;
      }//if
      else { //new beginning of list
         nn->next = this;
         this->prev = nn;
      }//else
   }//Insert_Before
   
   
   void Insert_After(Tree2Node<T>* par, short lvl, T& data) {
      Tree2Node<T>* nn = new Tree2Node<T>(lvl, NULL, 
                                          par, data, NULL, NULL);
      if (this->next) {
         nn->prev = this;
         nn->next = this->next;
         nn->next->prev = nn;
         this->next = nn;
      }//if
      else { //new end
         nn->prev = this;
         this->next = nn;
      }//else
   }//Insert_After

}; //Tree2Node



template <class T> class Tree2 {

   friend class Tree2Node<T>;
   friend class Tree2Cell<T>;

private:
   Tree2Node<T>* root;
   
public:
   
   Tree2(T& data) { 
      root = new Tree2Node<T>();
      root->data = data;
   }
   
   ~Tree2() { Clear(); root = NULL; }
   
   
   void Clear() {
      delete root; //should fire destructors
      root = NULL;
   }//if
   
   void Log() { //write to logfile
      String buf(100);
      
      //log("TRACK:  Tree2 Log.\n"); //what other kind can there be?? :)
      
      Tree2Cell<T> cll(*this);
      if (!cll.ptr) {
         //log("TRACK:  cll.ptr is NULL.\n");
      }//if
      while (cll) {
         Sprintf(buf, 
                 "TRACK: this: %i, prv: %i, nxt: %i, par: %i, frst: %i, data: %i, lvl: %i",
                 (int)(cll.ptr), (int)(cll.ptr->prev), 
                 (int)(cll.ptr->next), (int)(cll.ptr->parent),
                 (int)(cll.ptr->first), (int)(cll.ptr->data),
                 (int)(cll.ptr->level));
         //log(buf);
         cll.Next_Breadth();
      }//while
   }//log
}; //Tree2


#endif








