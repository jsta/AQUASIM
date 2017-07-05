////////////////////////////////   sdlist.h   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    05.12.90    Peter Reichert
// revisions:   12.06.91    Peter Reichert    ported to Sun
//              26.06.91    Peter Reichert    NODE::InList() added
//              15.07.91    Peter Reichert    const declarations added
//              16.09.91    Peter Reichert    NODE::NODE(const NODE&),
//                                            LIST::LIST(const LIST&)
//                                            constructors and
//                                            NODE::operator=(NODE&),
//                                            LIST::operator=(LIST&)
//                                            operators added
//              12.07.93    Peter Reichert    NODE::Next() and NODE::Prev()
//                                            replaced LIST::Next(node) and
//                                            LIST::Prev(node)
//
//////////////////////////////////////////////////////////////////////////////
//
// "sdlist" implements several list classes
// ======================================
//
//////////////////////////////////////////////////////////////////////////////
//
// Concepts
// ========
//
// The class NODE (sNODE or dNODE) represents a base class for classes to be
// inserted into lists. To prevent from programming errors, the pointers to
// neighboring nodes of the list are private and can only be changed by
// member functions of the friend LIST (sLST, sLIST, dLST of dLIST).
//
// In contrast to usual implementations, the pointer to the next node of the
// last node of a list is not set to zero, but points to the node itself.
// This allows to check, if a node is already element of a list and prevents
// from inserting a node several times.
//
// The NODE::NODE(const NODE&) constructor and the NODE::operator=(const NODE&) 
// operator initialize the pointers of the node to zero and thus allow
// copying derived classes element by element without currupting existing
// lists.
//
// The LIST::LIST(const LIST&) constructor and the LIST::operator=(const LIST&)
// operator are declared private to prevent the programmer from copying lists 
// (this is not meaningful at this level).
//
// All implementations of the list classes have the same member functions to
// allow changing the implementation used by changing only two typedef state-
// ments.
//
//////////////////////////////////////////////////////////////////////////////
//
// Classes
// =======
//
// The following list classes are implemented in this file:
//
// sNODE:        Node for singly-linked lists.
// ------
//
// dNODE:        Node for doubly-linked lists.
// ------
//
// sLST:         Singly-linked list; recommended version to save memory, if
// -----                             references to a previous node, jumps to
//                                   the last node and determination of the
//                                   position of the current node are not
//                                   frequently used.
//
// sLIST:        Singly-linked list; recommended version if jumps to the last
// ------                            node or determination of the position of
//                                   the current node are frequently used but
//                                   references to a previous node only occur
//                                   rarely
//                                   (this version needs additional memory for
//                                   one pointer and two CARDINAL's per list
//                                   as compared to sLST).
//
// dLST:         Doubly-linked list; recommended version if references to a
// -----                             previous node are frequently used, but
//                                   jumps to the last node and determination
//                                   of the position of the current node only
//                                   occur rarely.
//                                   (this version needs additional memory for
//                                   one pointer per node of the list as com-
//                                   pared to sLST)
//
// dLIST:        Doubly-linked list; recommended version if references to a
// ------                            previous node and jumps to the last node
//                                   or determination of the position of the
//                                   current node are frequently used.
//                                   (this version needs additional memory for
//                                   one pointer and two CARDINAL's per list
//                                   as compared to dLST)
//
//////////////////////////////////////////////////////////////////////////////
//
// Methods of the classes sNODE and dNODE
// ======================================
//
// To protect the lists, the node classes sNODE and dNODE do not have any
// methodes to manipulate the pointers.
//
// Answers to questions concerning the node:
// -----------------------------------------
//
// BOOLEAN  InList() const             returns TRUE if the node is element of
//                                     a list, otherwise FALSE.
//
// NODE*    Next() const               returns the next node in the list or
//                                     zero if the node is the last node.
// NODE*    Prev() const               returns the previous node in the list
//                                     or zero if the node is the first node
//                                     (this function only exists for dNODE)
//
// Output of variables to test the node:
// -------------------------------------
//
// void     Test()                     prints the addresses of the pointers
//                                     to ctest (see all.h). Virtual member.
//
//////////////////////////////////////////////////////////////////////////////
//
// Methods of the classes sLST, sLIST, dLST and dLIST
// ==================================================
//
// In the following list NODE means sNODE for the classes sLST and sLIST,
// dNODE for the classes dLST and dLIST
//
// Answers to questions concerning the list:
// -----------------------------------------
//
// BOOLEAN  Empty() const              returns TRUE if the list is empty,
//                                     otherwise FALSE.
//
// CARDINAL Size() const               returns the number of elements of the
//                                     list.
//
// CARDINAL Position(const NODE* node) returns the position of the node given
//                               const as argument, 0 if the list does not
// CARDINAL Position(const NODE& node) contain the node.
//                               const   
//
// CARDINAL Position() const           returns the position of the current
//                                     node in the list, 0 if the list is
//                                     empty.
//
// NODE*    First() const              returns a pointer to the first node of
//                                     the list, 0 if the list is empty.
//
// NODE*    Last() const               returns a pointer to the last node of
//                                     the list, 0 if the list is empty.
//
// BOOLEAN  Node(const NODE* node)     returns TRUE if node is an element of
//                           const     the list, otherwise FALSE.
// BOOLEAN  Node(const NODE& node)
//                           const
//
// NODE*    Node(CARDINAL pos) const   returns a pointer to the node at the
//                                     position pos, 0 if pos is not within
//                                     the range of the list.
//
// NODE*    Node() const               returns a pointer to the current
//                                     node or 0 if the list is empty.
//
// NODE*    Next() const               returns a pointer to the successor of
//                                     the current node, 0 if the list is
//                                     empty or if the current node is the
//                                     last node.
//
// NODE*    Prev() const               returns a pointer to the precedessor
//                                     of the current node, 0 if the list is
//                                     empty or if the current node is the
//                                     first node.
//
// Changes of which node is the current node:
// ------------------------------------------
//
// BOOLEAN  Goto(NODE* node)           sets the current node to the node given
// BOOLEAN  Goto(NODE& node)           as argument and returns TRUE if the
//                                     list contains this node, does not
//                                     change the current node and returns
//                                     FALSE otherwise.
//
// BOOLEAN  Goto(CARDINAL pos)         sets the current node to the node at
//                                     position pos and returns TRUE if pos
//                                     is within the range of the list, does
//                                     not change the current node and returns
//                                     FALSE otherwise.
//
// BOOLEAN  GotoNext()                 sets the current node to the successor
//                                     of the old current node and returns 
//                                     TRUE if this is possible, does not
//                                     change the current node and returns
//                                     FALSE if the current node is the last
//                                     node or if the list is empty.
//
// BOOLEAN  GotoPrev()                 sets the current node to the precedes-
//                                     sor of the old current node and returns 
//                                     TRUE if this is possible, does not
//                                     change the current node and returns
//                                     FALSE if the current node is the first
//                                     node or if the list is empty.
//
// BOOLEAN  GotoNextOrFirst()          sets the current node to the successor
//                                     of the old current node or to the first
//                                     node depending on if the old current
//                                     node is not or is the last node and
//                                     returns TRUE, returns FALSE if the list
//                                     is empty.
//
// BOOLEAN  GotoPrevOrLast()           sets the current node to the precedes-
//                                     sor of the old current node or to the
//                                     last node depending on if the old cur-
//                                     rent node is not or is the first node
//                                     and returns TRUE, returns FALSE if the
//                                     list is empty.
//
// BOOLEAN  GotoFirst()                sets the current node to the first node
//                                     and returns TRUE if the list is not
//                                     empty, returns FALSE otherwise.
//
// BOOLEAN  GotoLast()                 sets the current node to the last node
//                                     and returns TRUE it the list is not
//                                     empty, returns FALSE otherwise.
//
// Editing of the list:
// --------------------
//
// BOOLEAN  InsertFirst(NODE* node)    inserts the node given as argument as
// BOOLEAN  InsertFirst(Node& node)    first node of the list and returns
//                                     TRUE it the node is not yet an element
//                                     of a list, does not insert the node
//                                     and returns FALSE otherwise.
//
// BOOLEAN  InsertLast(NODE* node)     inserts the node given as argument as
// BOOLEAN  InsertLast(NODE& node)     last node of the list and returns
//                                     TRUE if the node is not yet an element
//                                     of a list, does not insert the node
//                                     and returns FALSE otherwise.
//
// BOOLEAN  Insert(NODE*    node,      inserts the node given as first argu-
//                 CARDINAL pos)       ment at the position given as second
// BOOLEAN  Insert(NODE&    node,      argument and returns TRUE if the node
//                 CARDINAL pos)       is not yet an element of a list and if
//                                     the position is within the range of the
//                                     list, does not insert the node and re-
//                                     turns FALSE otherwise.
//
// BOOLEAN  Insert(NODE* node)         inserts the node given as argument as
// BOOLEAN  Insert(NODE& node)         precedessor of the current node, sets
//                                     the current node to the new node and
//                                     returns TRUE if the node is not yet an
//                                     element of a list, does not change
//                                     the list and returns FALSE otherwise.
//
// BOOLEAN  InsertAfter(NODE* node)    inserts the node given as argument as
// BOOLEAN  InsertAfter(NODE& node)    successor of the current node, sets
//                                     the current node to the new node and
//                                     returns TRUE if the node is not yet an
//                                     element of a list, does not change
//                                     the list and returns FALSE otherwise.
//
// BOOLEAN  Replace(NODE* node)        replaces the current node with the node
// BOOLEAN  Replace(NODE& node)        given as argument and returns TRUE if
//                                     the node is not yet an element of a
//                                     list, does not change the list and
//                                     returns FALSE otherwise.
//
// BOOLEAN  RemoveFirst()              removes the first node of the list and
//                                     returns TRUE if the list was not empty,
//                                     returns FALSE otherwise.
//
// BOOLEAN  RemoveLast()               removes the last node of the list and
//                                     returns TRUE if the list was not empty,
//                                     returns FALSE otherwise.
//
// BOOLEAN  Remove(NODE* node)         removes the node given as argument,
// BOOLEAN  Remove(Node& node)         changes the current node as described
//                                     above if the node is the current node
//                                     and returns TRUE if pos is within the
//                                     range of the list, does not change
//                                     the list and returns FALSE otherwise.
//
// BOOLEAN  Remove(CARDINAL pos)       removes the node at position pos,
//                                     changes the current node as described
//                                     above if the node is the current node
//                                     and returns TRUE if the node is an
//                                     element of the list, does not change
//                                     the list and returns FALSE otherwise.
//
// BOOLEAN  Remove()                   removes the current node and set its
//                                     successor or its precedessor as current
//                                     node depending if not or if the current
//                                     node is the last node and returns TRUE
//                                     if the list is not empty, returns FALSE
//                                     otherwise.
//
// void     RemoveAll()                removes all nodes of the list.
//
// Output of variables to test the list:
// -------------------------------------
//
// void     Test()                     prints the addresses of the pointers
//                                     and the values of the variables to
//                                     ctest (see all.h). Virtual member.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef SDLIST_H
#define SDLIST_H

#include "all.h"

//////////////////////////////////////////////////////////////////////////////

class sNODE
{
      friend class sLST;
      friend class sLIST;

   public:

      sNODE();
      sNODE(const sNODE& node);
      sNODE(const sNODE* node);

      sNODE&       operator=(const sNODE& node);

      BOOLEAN      InList() const;

      sNODE*       Next() const;

      virtual void Test();

   private:

      sNODE*       nextnode;
};

//////////////////////////////////////////////////////////////////////////////

class dNODE
{
      friend class dLST;
      friend class dLIST;

   public:

      dNODE();
      dNODE(const dNODE& node);
      dNODE(const dNODE* node);

      dNODE&       operator=(const dNODE& node);

      BOOLEAN      InList() const;

      dNODE*       Next() const;
      dNODE*       Prev() const;

      virtual void Test();

   private:

      dNODE*       prevnode;
      dNODE*       nextnode;
};

//////////////////////////////////////////////////////////////////////////////

class sLST
{
   public:

      sLST();

      BOOLEAN      Empty() const;
      CARDINAL     Size() const;
      CARDINAL     Position(const sNODE* node) const;
      CARDINAL     Position(const sNODE& node) const
                      { return Position(&node); }
      CARDINAL     Position() const
                      { return Position(currnode); }
      sNODE*       First() const
                      { return firstnode; }
      sNODE*       Last() const;
      BOOLEAN      Node(const sNODE* node) const;
      BOOLEAN      Node(const sNODE& node) const
                      { return Node(&node); }
      sNODE*       Node(CARDINAL pos) const;
      sNODE*       Node() const
                      { return currnode; }
      sNODE*       Next() const;
      sNODE*       Prev() const;

      BOOLEAN      Goto(sNODE* node);
      BOOLEAN      Goto(sNODE& node)
                      { return Goto(&node); }
      BOOLEAN      Goto(CARDINAL pos);
      BOOLEAN      GotoNext();
      BOOLEAN      GotoPrev();
      BOOLEAN      GotoNextOrFirst();
      BOOLEAN      GotoPrevOrLast();
      BOOLEAN      GotoFirst();
      BOOLEAN      GotoLast();

      BOOLEAN      InsertFirst(sNODE* node);
      BOOLEAN      InsertFirst(sNODE& node)
                      { return InsertFirst(&node); }
      BOOLEAN      InsertLast(sNODE* node);
      BOOLEAN      InsertLast(sNODE& node)
                      { return InsertLast(&node); }
      BOOLEAN      Insert(sNODE* node, CARDINAL pos);
      BOOLEAN      Insert(sNODE& node, CARDINAL pos)
                      { return Insert(&node,pos); }
      BOOLEAN      Insert(sNODE* node);
      BOOLEAN      Insert(sNODE& node)
                      { return Insert(&node); }
      BOOLEAN      InsertAfter(sNODE* node);
      BOOLEAN      InsertAfter(sNODE& node)
                      { return InsertAfter(&node); }
      BOOLEAN      Replace(sNODE* node);
      BOOLEAN      Replace(sNODE& node)
                      { return Replace(&node); }
      BOOLEAN      RemoveFirst();
      BOOLEAN      RemoveLast();
      BOOLEAN      Remove(sNODE* node);
      BOOLEAN      Remove(sNODE& node)
                      { return Remove(&node); }
      BOOLEAN      Remove(CARDINAL pos)
                      { return Remove(Node(pos)); }
      BOOLEAN      Remove();
      void         RemoveAll();

      virtual void Test();

   protected:

      sLST(const sLST& list);

      sLST&        operator=(const sLST& list);

      sNODE*       firstnode;
      sNODE*       currnode;
};

//////////////////////////////////////////////////////////////////////////////

class sLIST: public sLST
{
   public:

      sLIST();

      CARDINAL     Size() const
                      { return numelem; }
      CARDINAL     Position(const sNODE* node) const;
      CARDINAL     Position(const sNODE& node) const
                      { return Position(&node); }
      CARDINAL     Position() const
                      { return currpos; }
      sNODE*       Last() const
                      { return lastnode; }

      BOOLEAN      Goto(sNODE* node);
      BOOLEAN      Goto(sNODE& node)
                      { return Goto(&node); }
      BOOLEAN      Goto(CARDINAL pos);
      BOOLEAN      GotoNext();
      BOOLEAN      GotoPrev();
      BOOLEAN      GotoNextOrFirst();
      BOOLEAN      GotoPrevOrLast();
      BOOLEAN      GotoFirst();
      BOOLEAN      GotoLast();

      BOOLEAN      InsertFirst(sNODE* node);
      BOOLEAN      InsertFirst(sNODE& node)
                      { return InsertFirst(&node); }
      BOOLEAN      InsertLast(sNODE* node);
      BOOLEAN      InsertLast(sNODE& node)
                      { return InsertLast(&node); }
      BOOLEAN      Insert(sNODE* node, CARDINAL pos);
      BOOLEAN      Insert(sNODE& node, CARDINAL pos)
                     { return Insert(&node,pos); }
      BOOLEAN      Insert(sNODE* node);
      BOOLEAN      Insert(sNODE& node)
                      { return Insert(&node); }
      BOOLEAN      InsertAfter(sNODE* node);
      BOOLEAN      InsertAfter(sNODE& node)
                      { return InsertAfter(&node); }
      BOOLEAN      Replace(sNODE* node);
      BOOLEAN      Replace(sNODE& node)
                      { return Replace(&node); }
      BOOLEAN      RemoveFirst();
      BOOLEAN      RemoveLast();
      BOOLEAN      Remove(sNODE* node);
      BOOLEAN      Remove(sNODE& node)
                      { return Remove(&node); }
      BOOLEAN      Remove(CARDINAL pos)
                      { return Remove(Node(pos)); }
      BOOLEAN      Remove();
      void         RemoveAll();

      virtual void Test();
      
   protected:

      sLIST(const sLIST& list);

      sLIST&       operator=(const sLIST& list);

      sNODE*       lastnode;
      CARDINAL     currpos;
      CARDINAL     numelem;
};

//////////////////////////////////////////////////////////////////////////////

class dLST
{
   public:

      dLST();

      BOOLEAN      Empty() const;
      CARDINAL     Size() const;
      CARDINAL     Position(const dNODE* node) const;
      CARDINAL     Position(const dNODE& node) const
                      { return Position(&node); }
      CARDINAL     Position() const
                      { return Position(currnode); }
      dNODE*       First() const
                      { return firstnode; }
      dNODE*       Last() const;
      BOOLEAN      Node(const dNODE* node) const;
      BOOLEAN      Node(const dNODE& node) const
                      { return Node(&node); }
      dNODE*       Node(CARDINAL pos) const;
      dNODE*       Node() const
                      { return currnode; }
      dNODE*       Next() const;
      dNODE*       Prev() const;

      BOOLEAN      Goto(dNODE* node);
      BOOLEAN      Goto(dNODE& node)
                      { return Goto(&node); }
      BOOLEAN      Goto(CARDINAL pos);
      BOOLEAN      GotoNext();
      BOOLEAN      GotoPrev();
      BOOLEAN      GotoNextOrFirst();
      BOOLEAN      GotoPrevOrLast();
      BOOLEAN      GotoFirst();
      BOOLEAN      GotoLast();

      BOOLEAN      InsertFirst(dNODE* node);
      BOOLEAN      InsertFirst(dNODE& node)
                      { return InsertFirst(&node); }
      BOOLEAN      InsertLast(dNODE* node);
      BOOLEAN      InsertLast(dNODE& node)
                      { return InsertLast(&node); }
      BOOLEAN      Insert(dNODE* node, CARDINAL pos);
      BOOLEAN      Insert(dNODE& node, CARDINAL pos)
                      { return Insert(&node,pos); }
      BOOLEAN      Insert(dNODE* node);
      BOOLEAN      Insert(dNODE& node)
                      { return Insert(&node); }
      BOOLEAN      InsertAfter(dNODE* node);
      BOOLEAN      InsertAfter(dNODE& node)
                      { return InsertAfter(&node); }
      BOOLEAN      Replace(dNODE* node);
      BOOLEAN      Replace(dNODE& node)
                      { return Replace(&node); }
      BOOLEAN      RemoveFirst();
      BOOLEAN      RemoveLast();
      BOOLEAN      Remove(dNODE* node);
      BOOLEAN      Remove(dNODE& node)
                      { return Remove(&node); }
      BOOLEAN      Remove(CARDINAL pos)
                      { return Remove(Node(pos)); }
      BOOLEAN      Remove();
      void         RemoveAll();

      virtual void Test();
      
   protected:

      dLST(const dLST& list);

      dLST&        operator=(const dLST& list);

      dNODE*       firstnode;
      dNODE*       currnode;
};

//////////////////////////////////////////////////////////////////////////////

class dLIST: public dLST
{
   public:

      dLIST();

      CARDINAL     Size() const
                      { return numelem; }
      CARDINAL     Position(const dNODE* node) const;
      CARDINAL     Position(const dNODE& node) const
                      { return Position(&node); }
      CARDINAL     Position() const
                      { return currpos; }
      dNODE*       Last() const
                      { return lastnode; }

      BOOLEAN      Goto(dNODE* node);
      BOOLEAN      Goto(dNODE& node)
                      { return Goto(&node); }
      BOOLEAN      Goto(CARDINAL pos);
      BOOLEAN      GotoNext();
      BOOLEAN      GotoPrev();
      BOOLEAN      GotoNextOrFirst();
      BOOLEAN      GotoPrevOrLast();
      BOOLEAN      GotoFirst();
      BOOLEAN      GotoLast();

      BOOLEAN      InsertFirst(dNODE* node);
      BOOLEAN      InsertFirst(dNODE& node)
                      { return InsertFirst(&node); }
      BOOLEAN      InsertLast(dNODE* node);
      BOOLEAN      InsertLast(dNODE& node)
                      { return InsertLast(&node); }
      BOOLEAN      Insert(dNODE* node, CARDINAL pos);
      BOOLEAN      Insert(dNODE& node, CARDINAL pos)
                      { return Insert(&node,pos); }
      BOOLEAN      Insert(dNODE* node);
      BOOLEAN      Insert(dNODE& node)
                      { return Insert(&node); }
      BOOLEAN      InsertAfter(dNODE* node);
      BOOLEAN      InsertAfter(dNODE& node)
                      { return InsertAfter(&node); }
      BOOLEAN      Replace(dNODE* node);
      BOOLEAN      Replace(dNODE& node)
                      { return Replace(&node); }
      BOOLEAN      RemoveFirst();
      BOOLEAN      RemoveLast();
      BOOLEAN      Remove(dNODE* node);
      BOOLEAN      Remove(dNODE& node)
                      { return Remove(&node); }
      BOOLEAN      Remove(CARDINAL pos)
                      { return Remove(Node(pos)); }
      BOOLEAN      Remove();
      void         RemoveAll();

      virtual void Test();

   protected:

      dLIST(const dLIST& list);

      dLIST&       operator=(const dLIST& list);

      dNODE*       lastnode;
      CARDINAL     currpos;
      CARDINAL     numelem;
};

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
