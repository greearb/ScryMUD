// $Id: rb_tree.h,v 1.4 2003/02/25 04:14:44 greear Exp $
// $Revision: 1.4 $  $Author: greear $ $Date$

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

// rb_tree.h -- Declarations for the search tree library


#ifndef RB_TreeInclude
#define RB_TreeInclude


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


template <class K, class V> class Tree;
template <class T> int TreeCompare (const T &, const T &);
//
// TreeCompare is the default key comparison function


enum Palette {Red, Black};


template <class K, class V> class TreeNode
{
   friend class Tree<K,V>;   
   // Fields
   //
   Palette   Color;
   TreeNode *Left;
   TreeNode *Right;
   K         Key;
   V         Value;
   
   // Constructors
   //
   TreeNode (Palette C, TreeNode *L, TreeNode *R, const K &X, const V &Y)
      : Color(C), Left(L), Right(R), Key(X), Value(Y) {}
   TreeNode () { Color = Red; Left = 0; Right = 0; }
};


template <class K, class V> class Tree
{
   // Fields
   //
   TreeNode<K,V> *Root;
   int          (*Compare)(const K &, const K &);
   
   // Scaffolding
   //
   TreeNode<K,V> *RotateRight (TreeNode<K,V> *);
   TreeNode<K,V> *RotateLeft  (TreeNode<K,V> *);
   void           Flip        (TreeNode<K,V> *);
   void           FlipLeft    (TreeNode<K,V> *);
   void           FlipRight   (TreeNode<K,V> *);
   void           FlipBoth    (TreeNode<K,V> *);
   TreeNode<K,V> *InsertRebalance (TreeNode<K,V> *, const K &);
   TreeNode<K,V> *DeleteBasis     (TreeNode<K,V> *, TreeNode<K,V> *, short &);
   TreeNode<K,V> *DeleteRebalance (TreeNode<K,V> *, TreeNode<K,V> *, short &);
   TreeNode<K,V> *Copy    (TreeNode<K,V> *);
   void           Dump    (TreeNode<K,V> *, int = 0);

   // Recursive Helping  
   TreeNode<K,V> *Insert1 (K, V, TreeNode<K,V> *);
   TreeNode<K,V> *Delete1 (K, TreeNode<K,V> *, short &);
   TreeNode<K,V> *Delete2 (TreeNode<K,V> *, TreeNode<K,V> *, short &);
   void		  Size1  (TreeNode<K,V> *, int &);
   void		  Clear1 (TreeNode<K,V> *);
   
public:

   // Constructors and destructor
   //
   Tree  (int (*)(const K &, const K &) = TreeCompare);
   Tree  (const Tree<K,V> &);
   ~Tree ();
   
   // Operators
   //
   Tree<K,V> &operator = (const Tree<K,V> &);
   
   // Insertion and deletion
   //
   void Insert (const K &, const V &);
   void Delete (const K &);
      //
      // Insert and Delete do not make any assumptions about keys.
      // Insert changes the associated value if the key is already in the tree.
      // Delete does nothing if the key is not in the tree.

   // Access
   //
   short Find (const K &, V &);
      //
      // Find returns 1 if the key is in the tree, and 0 otherwise.
      // If the key is found, the associated value is returned through the
      // second reference argument.
   
   // Iteration
   //
   short Min  (K &);
   short Max  (K &);
   short Next (K &);
   short Prev (K &);
      //
      // Min and Max return 1 if the tree is not empty, and 0 otherwise.
      // The corresponding key is returned through their reference argument.
      //
      // Next and Prev return 1 if there is a key respectively larger or
      // smaller than their argument.  This key is returned through their
      // reference argument.
      
   // Miscellaneous
   //
   int   Size    ();
   short IsEmpty ();
   void  Clear   ();
};


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#include "rb_tree.cc"


#endif // TreeInclude
