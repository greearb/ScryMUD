// $Id: rb_tree.cc,v 1.4 2003/02/25 04:14:44 greear Exp $
// $Revision: 1.4 $  $Author: greear $ $Date: 2003/02/25 04:14:44 $

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

// rb_tree.cc -- Implementation of the search tree library


#include <stdlib.h>
#include <iostream.h>


#define True  1
#define False 0

//#define Red(K,V)     TreeNode<K,V>::Red
//#define Black(K,V)   TreeNode<K,V>::Black
//#define Palette(K,V) TreeNode<K,V>::Palette

//void log(const char*); //Ben's Standard log function


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Constructors and Destructor


// Constructor with comparison function
//
template <class K, class V> Tree<K,V>::Tree (int (*F)(const K &, const K &))
{
   Root = 0;
   Compare = F;
}

// Copy constructor
//
template <class K, class V> Tree<K,V>::Tree (const Tree<K,V> &T)
{
   Root = Copy(T.Root);
   Compare = T.Compare;
}


// Copy -- Recursively copy a tree
//
template <class K, class V> TreeNode<K,V> *Tree<K,V>::Copy
   (register TreeNode<K,V> *N)
{
   if (N)
   {
      TreeNode<K,V> *M = new TreeNode<K,V>
         (N->Color, Copy(N->Left), Copy(N->Right), N->Key, N->Value);
      if (!M) {
         //log("ERROR:  (TreeNode) Memory allocation failed.");
      }
      return M;
   }
   else
      return 0;
}

// Destructor
//
template <class K, class V> Tree<K,V>::~Tree ()
{
   Clear ();
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Operators


template <class K, class V> Tree<K, V> &Tree<K,V>::operator =
   (const Tree<K,V> &T)
{
   Root = Copy(T.Root);
   Compare = T.Compare;
   return *this;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Access


// Find -- Find the value associated with a key
//
// Find returns 1 if the key is in the tree, and 0 otherwise.
// The value associated with the key is returned through the second argument.
//
template <class K, class V> short Tree<K,V>::Find
   (register const K &Key, V &Value)
{
   register TreeNode<K,V> *N = Root;
   while (N)
   {
      register int C = Compare(Key, N->Key); 
      if (C < 0)
         N = N->Left;
      else if (C > 0)
         N = N->Right;
      else
         break;
   }
   return N ? (Value = N->Value, True) : False;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Insertion and deletion


// Insert -- Insert a key-value pair into a tree
//
// If the key is already in the tree, the associated value is updated.
//
template <class K, class V> void Tree<K,V>::Insert
   (const K &Y, const V &X)
{
   Root = Insert1(Y, X, Root);
   Root->Color = Black;
}

// Insert1 -- Recursive helper function of Insert.
//
template <class K, class V> TreeNode<K,V> *Tree<K,V>::Insert1
   (K Y, V X, TreeNode<K,V> *N)
{
   if (!N)
   {
      TreeNode<K,V> *M = new TreeNode<K,V> (Red, (TreeNode<K,V> *)0, 
                                           (TreeNode<K,V> *)0, Y, X);
      if (!M) {
	 //log("ERROR:  (rb_tree) No memory left");
      }
      return M;
   }
   else if ((*Compare)(Y, N->Key) < 0)
   {
      N->Left = Insert1 (Y, X, N->Left);
      return InsertRebalance(N,Y);
   }
   else if ((*Compare)(Y, N->Key) > 0)
   {
      N->Right = Insert1 (Y, X, N->Right);
      return InsertRebalance(N,Y);
   }
   else 
   {  
      N->Value = X;
      return N;
   }
}

// InsertRebalance -- Restore the color invariants at a node after insertion
//
template <class K, class V> TreeNode<K,V> *Tree<K,V>::InsertRebalance
   (TreeNode<K,V> *N, const K &Key)
{
   if (N && N->Color == Black)
      if ((*Compare)(Key, N->Key) < 0) // Cases 1-4
      {
         register TreeNode<K,V> *M = N->Left;
         if (M && M->Color == Red)
            if (N->Right && N->Right->Color == Red) // Cases 1,3
            {
               FlipBoth(N);
               return N;
            }
            else // Cases 2,4
            {
               if ((*Compare)(Key, M->Key) > 0) // Case 4
                  N->Left = RotateLeft(M);
               FlipLeft(N);
               return RotateRight(N);
            }
         else
            return N;
      }
      else // Cases 5-8
      {
         register TreeNode<K,V> *M = N->Right;
         if (M && M->Color == Red)
            if (N->Left && N->Left->Color == Red) // Cases 5,7
            {
               FlipBoth(N);
               return N;
            }
            else // Cases 6,8
            {
               if ((*Compare)(Key, M->Key) < 0) // Case 8
                  N->Right = RotateRight(M);
               FlipRight(N);
               return RotateLeft(N);
            }
         else
            return N;
      }
   else
      return N;
}


// Delete -- Delete a key-value pair from a tree, given the key
//
// If the key is not in the tree, nothing is changed.
//
template <class K, class V> void Tree<K,V>::Delete (register const K &Y)
{
   short Defect;

   Root = Delete1(Y, Root, Defect);

   if (Root)
      Root->Color = Black;
}

// Delete1 -- Recursive helper function of Delete
//
template <class K, class V> TreeNode<K,V> *Tree<K,V>::Delete1
   (K Y, TreeNode<K,V> *N, short &Defect)
{
   if (!N)
     return ((TreeNode<K,V> *) 0);
   else if (N && (*Compare)(Y, N->Key) < 0)
   {
      N->Left = Delete1 (Y, N->Left, Defect);
      return DeleteRebalance (N, N->Left, Defect); 
   }
   else if (N && (*Compare)(Y, N->Key) > 0)
   {
      N->Right = Delete1(Y, N->Right, Defect);
      return DeleteRebalance (N, N->Right, Defect);
   }
   else if (!N->Left)
      return DeleteBasis (N, N->Right, Defect);
   else if (!N->Right)
      return DeleteBasis (N, N->Right, Defect);
   else    
   {
      N->Right = Delete2(N->Right, N, Defect);
      return DeleteRebalance(N, N->Right, Defect);
   }
}


// Delete2 -- 2nd recursive helper function of Delete, called in Delete1
//
template <class K, class V> TreeNode<K,V> *Tree<K,V>::Delete2
   (TreeNode<K,V> *N, TreeNode<K,V> *P, short &Defect)
{
   if (N->Left)
   {
      N->Left = Delete2(N->Left, P, Defect);
      return DeleteRebalance (N, N->Left, Defect);
   }
   else // (N->Left)
   {
      P->Key = N->Key;
      P->Value = N->Value;
      return DeleteRebalance(N, N->Right, Defect);
   }
}


// DeleteBasis -- Base cases for deletion and rebalancing
//
// Splice out a node with one child, and return the child.
//
template <class K, class V> inline TreeNode<K,V> *Tree<K,V>::DeleteBasis
   (register TreeNode<K,V> *N, register TreeNode<K,V> *M, short &Defect)
{
   if (N->Color == Red)
      Defect = False;
   else if (!M || M->Color == Black)
      Defect = True;
   else
   {
      Flip(M);
      Defect = False;
   }
   delete N;
   return M;
}


// DeleteRebalance -- Restore the color invariants at a node after deletion
//
template <class K, class V> TreeNode<K,V> *Tree<K,V>::DeleteRebalance
   (register TreeNode<K,V> *N, register TreeNode<K,V> *M, short &Defect)
{
   if (Defect)
   {
      if (M && M->Color == Red) // Case 0
      {
         Flip(M);
         Defect = False;
      }
      else if (M == N->Left) // Cases 1-4
      {
         register TreeNode<K,V> *S = N->Right;
         if (S->Color == Red) // Case 1
         {
            FlipRight(N);
            N = RotateLeft(N);
            N->Left = DeleteRebalance(N->Left, N->Left->Left, Defect);
               //
               // This call will encounter one of cases 2-4, and within
               // two steps, set Defect to False
         }
         else if ((!S->Left  || S->Left->Color  == Black) &&
                  (!S->Right || S->Right->Color == Black)) // Case 2
         {
            Flip(S);
            Defect = True;
         }
         else // Cases 3,4
         {
            if (S->Left && S->Left->Color == Red) // Case 3
            {
               N->Right = RotateRight(N->Right);
               FlipRight(N->Right);
            }
            N = RotateLeft(N);
            FlipLeft(N);
            Flip(N->Right);
            Defect = False;
         }
      }
      else // Cases 5-8
      {
         register TreeNode<K,V> *S = N->Left;
         if (S->Color == Red) // Case 5
         {
            FlipLeft(N);
            N = RotateRight(N);
            N->Right = DeleteRebalance(N->Right, N->Right->Right, Defect);
               //
               // This call will encounter one of cases 6-8, and within
               // two steps, set Defect to False
         }
         else if ((!S->Right || S->Right->Color == Black) &&
                  (!S->Left  || S->Left->Color  == Black)) // Case 6
         {
            Flip(S);
            Defect = True;
         }
         else // Cases 7,8
         {
            if (S->Right && S->Right->Color == Red) // Case 7
            {
               N->Left = RotateLeft(N->Left);
               FlipLeft(N->Left);
            }
            N = RotateRight(N);
            FlipRight(N);
            Flip(N->Left);
            Defect = False;
         }
      }
   }
   return N;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Scaffolding


// TreeCompare -- Default key comparison function
//
template <class T> inline int TreeCompare (const T &X, const T &Y)
{
  return (X < Y) ? -1 : ((X > Y) ? 1 : 0);
}


// Flip -- Flip the color at a node
//
// Assumes the node is not nil.
//
template <class K, class V> inline void Tree<K,V>::Flip
   (register TreeNode<K,V> *N)
{
   N->Color = (N->Color == Red) ? Black : Red;
}


// FlipLeft -- Exchange the color of a node with its left child
//
// Assumes the node and its left child are not nil.
//
template <class K, class V> inline void Tree<K,V>::FlipLeft
   (register TreeNode<K,V> *N)
{
   Palette C = N->Color;
   N->Color = N->Left->Color;
   N->Left->Color = C;
}


// FlipRight -- Exchange the color of a node with its right child
//
// Assumes the node and its right child are not nil.
//
template <class K, class V> inline void Tree<K,V>::FlipRight
   (register TreeNode<K,V> *N)
{
   Palette C = N->Color;
   N->Color = N->Right->Color;
   N->Right->Color = C;
}


// FlipBoth -- Exchange the color of a node with its left and right children
//
// Assumes the node and its right and left children are not nil.
//
template <class K, class V> inline void Tree<K,V>::FlipBoth
   (register TreeNode<K,V> *N)
{
   Palette C = N->Color;
   N->Color = N->Left->Color;
   N->Left->Color = C;
   N->Right->Color = C;
}


// RotateRight -- Right rotation at a node
//
// Assumes that both the node and its left child are not nil.
//
template <class K, class V> inline TreeNode<K,V> *Tree<K,V>::RotateRight
   (register TreeNode<K,V> *N)
{
   register TreeNode<K,V> *M = N->Left;
   N->Left = M->Right;
   M->Right = N;
   return M;
}


// RotateLeft -- Left rotation at a node
//
// Assumes that both the node and its right child are not nil.
//
template <class K, class V> inline TreeNode<K,V> *Tree<K,V>::RotateLeft
   (register TreeNode<K,V> *N)
{
   register TreeNode<K,V> *M = N->Right;
   N->Right = M->Left;
   M->Left = N;
   return M;
}


// Dump -- Write the subtree rooted a given node in preorder on standard error
//
template <class K, class V> void Tree<K,V>::Dump (TreeNode<K,V> *N, int D)
{
   if (N)
   {
      for (int i = 1; i <= D; i++)
         cerr << " ";
      cerr << "(" << N->Key << ", " << N->Value << ")\n";
      Dump(N->Left, D + 1);
      Dump(N->Right, D + 1);
   }
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
// Iteration

// Min -- Looks up minimum key 
//
// Min returns a 1 if the tree is not empty, 0 otherwise.
// If the tree is not empty, the key is returned in Min's 
// reference argument.
template <class K, class V> short Tree<K,V>::Min (K &Y)
{
   if (Root)
   {
      TreeNode<K,V> *N = Root;
      while (N->Left)
         N = N->Left;
      Y = N->Key;
      return 1;
   }
   else
      return 0;
}

// Max -- Looks up maximum key
//
// Max returns a 1 if the tree is not empty, 0 otherwise.
// If the tree is not empty, the key is returned in Max's
// reference argument.
template <class K, class V> short Tree<K,V>::Max (K &Y)
{
   if (Root)
   {   
      TreeNode<K,V> *N = Root;
      while (N->Right)
         N = N->Right;
      Y = N->Key;
      return 1;
   }
   else 
      return 0;
}

// Next -- Finds the next highest key 
//
// Next returns a 1 if there is a next higher key, 0 otherwise.
// If there is a next higher key, even if the given key isn't in
// the tree, Next returns the next highest key in its reference 
// argument.
template <class K, class V> short Tree<K,V>::Next (K &Y)
{
   K I;
   short S = Max(I);
   if (S && ((*Compare)(Y, I) < 0)) //if there is a Next
   {
      TreeNode<K,V> *N, *A, *P;
      N = Root; A = Root; P = Root;
      while (N && ((*Compare)(Y, N->Key))) //find N in tree, if there is one
         switch ((*Compare)(Y, N->Key))    //read these rem's as N is Node
         {                                 //that has Y as key
            case -1: P = N; A = N;
                     N = N->Left; break;
            case 1:  P = N; 
                     N = N->Right; break;
         }
      if (!N) // if N isn't in tree
         switch ((*Compare)(Y, P->Key))
         {
             case -1: Y = P->Key; break;
             case 1:  Y = A->Key; break;
         }
      // having reached here, N is in the tree
      else if (!N->Right) // N doesn't have a rt child
         Y = A->Key;
      else // (N->Right), N does have a rt. child 
      {
         N = N->Right;
         while (N->Left)
            N = N->Left;
         Y = N->Key;
      }
      return 1;
   }
   else // there is not a Next
      return 0;
}


// Prev -- Finds the next lowest key
//
// Prev returns a 1 if there is a next lower key, 0 otherwise.
// If there is a next lower key, even if the given key isn't in
// the tree, Prev returns the next lowest key in its reference
// argument.
template <class K, class V> short Tree<K,V>::Prev (K &Y)
// This algorithm is basically based on the idea that if the Node N
// that contains key Y, has a left child, the previous Node is
// the Maximum Node in the subtree of which N's left child is the 
// root.  If Node N doesn't have a left child, the previous node
// is the closest left grandparent of N.
{
   K I;
   short S = Max(I);
   if (S && ((*Compare)(Y, I) > 0)) // if there is a previous Node
   { 
      TreeNode<K,V> *N, *A, *P;
      N, A, P = Root;
      while (N && (*Compare)(Y, N->Key)) //try to find N, if there is one
         switch ((*Compare)(Y, N->Key))
         {
            case -1: P = N; 
                     N = N->Left; break;
            case 1:  A = N; P = N;
                     N = N->Right; break;
         }
      if (!N) //if there isn't an N
         switch ((*Compare)(Y, P->Key))
         {
            case -1: Y = A->Key; break;
            case 1:  Y = P->Key; break;
         }
      // there is an N in the tree
      else if (!N->Left) //if N has no left child
         Y = A->Key;
      else // (N->Left), N has a left child
      {
         while (N->Right)
            N = N->Right;
         Y = N->Key;
      }
      return 1;
   }
   else //if there is not a previous node
      return 0;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Iteration

// Size  -- Calculates the size of the tree and returns that value
//
template <class K, class V> int Tree<K,V>::Size ()
{
   int X = 0;
   if (Root)
      Size1 (Root, X);
   return X;
}


// Size1 -- Help function of Size
//
template <class K, class V> void Tree<K,V>::Size1 
   (TreeNode<K,V> *N, int &X)
{
   if (N->Left)
      Size1 (N->Left);
   if (N->Right)
      Size1 (N->Right);
   X++;
}


// IsEmpty -- Boolean for "Is the tree empty?"
//
// Isempty returns a 1 if the tree is not empty, 0 otherwise.
template <class K, class V> short Tree<K,V>::IsEmpty ()
{ 
   return Root ? 1 : 0;
}

// Clear -- Deallocates the memory taken up by a tree.
//
// Clear uses the recursive helper function Clear1.
template <class K, class V> void Tree<K,V>::Clear ()
{
   if (Root)
      Clear1(Root);
   delete Root;
   Root = (TreeNode<K,V> *) 0;
}

// Clear1 -- Recursive helper function of Clear.
//
template <class K, class V> void Tree<K,V>::Clear1 
   (TreeNode<K,V> *N)
{
   if (N->Left)
   {
      if (N->Left->Left || N->Left->Right)
         Clear1(N->Left);
      delete N->Left;
    }
    if (N->Right)
    {
       if (N->Right->Left || N->Right->Right)
          Clear1(N->Right);
       delete N->Right;
    }
}   




