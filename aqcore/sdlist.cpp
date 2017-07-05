////////////////////////////////   sdlist.C   ////////////////////////////////
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

#include "all.h"
#include "sdlist.h"

//////////////////////////////////////////////////////////////////////////////

sNODE::sNODE()
{
   nextnode = 0;
}

dNODE::dNODE()
{
   prevnode = 0;
   nextnode = 0;
}


sNODE::sNODE(const sNODE&)
{
   nextnode = 0;   // not node.nextnode to save existing lists!
}


sNODE::sNODE(const sNODE*)
{
   nextnode = 0;   // not node.nextnode to save existing lists!
}


dNODE::dNODE(const dNODE&)
{
   prevnode = 0;   // not node.prevnode to save existing lists!
   nextnode = 0;   // not node.nextnode to save existing lists!
}


dNODE::dNODE(const dNODE*)
{
   prevnode = 0;   // not node.prevnode to save existing lists!
   nextnode = 0;   // not node.nextnode to save existing lists!
}


sNODE& sNODE::operator=(const sNODE&)
{
   if ( InList() == TRUE )
      FatalError("sNODE::operator=: Reassign a node which is in a list");
   nextnode = 0;   // not node.nextnode to save existing lists!
   return *this;
}


dNODE& dNODE::operator=(const dNODE&)
{
   if ( InList() == TRUE )
      FatalError("dNODE::operator=: Reassign a node which is in a list");
   prevnode = 0;   // not node.prevnode to save existing lists!
   nextnode = 0;   // not node.nextnode to save existing lists!
   return *this;
}


BOOLEAN sNODE::InList() const
{
   if (nextnode==0) return FALSE;
   else             return TRUE;
}

BOOLEAN dNODE::InList() const
{
   if (nextnode==0) return FALSE;
   else             return TRUE;
}


sNODE* sNODE::Next() const
{
   if (nextnode==this) return 0;
   return nextnode;
}

dNODE* dNODE::Next() const
{
   if (nextnode==this) return 0;
   return nextnode;
}

dNODE* dNODE::Prev() const
{
   if (prevnode==this) return 0;
   return prevnode;
}


void sNODE::Test()
{
   *ctest << "\nclass sNODE:      node        = " << this
          << "\n                  nextnode    = " << nextnode;
   return;
}

void dNODE::Test()
{
   *ctest << "\nclass dNODE:      node        = " << this
          << "\n                  prevnode    = " << prevnode
          << "\n                  nextnode    = " << nextnode;
   return;
}

//////////////////////////////////////////////////////////////////////////////

sLST::sLST()
{
   firstnode = 0;
   currnode  = 0;
}

sLIST::sLIST()
{
   lastnode  = 0;
   currpos   = 0;
   numelem   = 0;
}

dLST::dLST()
{
   firstnode = 0;
   currnode  = 0;
}

dLIST::dLIST()
{
   lastnode  = 0;
   currpos   = 0;
   numelem   = 0;
}


sLST::sLST(const sLST&)
{
   FatalError("sLST::sLST: Copying lists is not allowed");
}


sLIST::sLIST(const sLIST&)
{
   FatalError("sLIST::sLIST: Copying lists is not allowed");
}


dLST::dLST(const dLST&)
{
   FatalError("dLST::dLST: Copying lists is not allowed");
}


dLIST::dLIST(const dLIST&)
{
   FatalError("dLIST::dLIST: Copying lists is not allowed");
}


sLST& sLST::operator=(const sLST&)
{
   FatalError("sLST::sLST: Copying lists is not allowed");
   return *this;
}


sLIST& sLIST::operator=(const sLIST&)
{
   FatalError("sLIST::sLIST: Copying lists is not allowed");
   return *this;
}


dLST& dLST::operator=(const dLST&)
{
   FatalError("dLST::dLST: Copying lists is not allowed");
   return *this;
}


dLIST& dLIST::operator=(const dLIST&)
{
   FatalError("dLIST::dLIST: Copying lists is not allowed");
   return *this;
}


BOOLEAN sLST::Empty() const
{
   if (firstnode==0) return TRUE;
   else              return FALSE;
}

BOOLEAN dLST::Empty() const
{
   if (firstnode==0) return TRUE;
   else              return FALSE;
}


CARDINAL sLST::Size() const
{
   if (firstnode==0) return 0;
   CARDINAL i=1;
   sNODE*   n=firstnode;
   while (n->nextnode!=n) { n=n->nextnode; i++; }
   return i;
}

// sLIST::Size() is inline

CARDINAL dLST::Size() const
{
   if (firstnode==0) return 0;
   CARDINAL i=1;
   dNODE*   n=firstnode;
   while (n->nextnode!=n) { n=n->nextnode; i++; }
   return i;
}

// dLIST::Size() is inline


CARDINAL sLST::Position(const sNODE* node) const
{
   if (firstnode==0) return 0;
   CARDINAL i=1;
   sNODE*   n=firstnode;
   while (1)
   {
      if (n==node) return i;
      if (n->nextnode==n) return 0;
      n = n->nextnode;
      i++;
   }
}

CARDINAL sLIST::Position(const sNODE* node) const
{
   return sLST::Position(node);
}

CARDINAL dLST::Position(const dNODE* node) const
{
   if (firstnode==0) return 0;
   CARDINAL i=1;
   dNODE*   n=firstnode;
   while (1)
   {
      if (n==node) return i;
      if (n->nextnode==n) return 0;
      n = n->nextnode;
      i++;
   }
}

CARDINAL dLIST::Position(const dNODE* node) const
{
   return dLST::Position(node);
}

// sLST::Position() is inline

// sLIST::Position() is inline

// dLST::Position() is inline

// dLIST::Position() is inline   


// sLST::First() is inline

// dLST::First() is inline


sNODE* sLST::Last() const
{
   sNODE* n=currnode;
   if (n==0) return 0;
   while (n->nextnode!=n) n=n->nextnode;
   return n;
}

// sLIST::Last() is inline

dNODE* dLST::Last() const
{
   dNODE* n=currnode;
   if (n==0) return 0;
   while (n->nextnode!=n) n=n->nextnode;
   return n;
}

// dLIST::Last() is inline


BOOLEAN sLST::Node(const sNODE* node) const
{
   if (Position(node)>0) return TRUE;
   else                  return FALSE;
}

BOOLEAN dLST::Node(const dNODE* node) const
{
   if (Position(node)>0) return TRUE;
   else                  return FALSE;
}


sNODE* sLST::Node(CARDINAL pos) const
{
   if (pos<1 || firstnode==0) return 0;
   sNODE* n=firstnode;
   for ( CARDINAL i=1; i<pos; i++ )
   {
      if (n->nextnode==n) return 0;
      n=n->nextnode;
   }
   return n;
}

dNODE* dLST::Node(CARDINAL pos) const
{
   if (pos<1 || firstnode==0) return 0;
   dNODE* n=firstnode;
   for ( CARDINAL i=1; i<pos; i++ )
   {
      if (n->nextnode==n) return 0;
      n=n->nextnode;
   }
   return n;
}


// sLST::Node() is inline

// dLST::Node() is inline


sNODE* sLST::Next() const
{
   if (currnode==0)                  return 0;
   if (currnode->nextnode==currnode) return 0;
   return currnode->nextnode;
}

dNODE* dLST::Next() const
{
   if (currnode==0)                  return 0;
   if (currnode->nextnode==currnode) return 0;
   return currnode->nextnode;
}


sNODE* sLST::Prev() const
{
   sNODE* n=firstnode;
   if (n==currnode) return 0;
   while (n->nextnode!=currnode) { n=n->nextnode; }
   return n;
}

dNODE* dLST::Prev() const
{
   if (currnode==0)                  return 0;
   if (currnode->prevnode==currnode) return 0;
   return currnode->prevnode;
}


BOOLEAN sLST::Goto(sNODE* node)
{
   if (Position(node)>0) { currnode=node; return TRUE; }
   else                  { return FALSE;               }
}

BOOLEAN sLIST::Goto(sNODE* node)
{
   CARDINAL pos=Position(node);
   if (pos>0) { currnode=node; currpos=pos; return TRUE; }
   else       { return FALSE;                            }
}

BOOLEAN dLST::Goto(dNODE* node)
{
   if (Position(node)>0) { currnode=node; return TRUE; }
   else                  { return FALSE;               }
}

BOOLEAN dLIST::Goto(dNODE* node)
{
   CARDINAL pos=Position(node);
   if (pos>0) { currnode=node; currpos=pos; return TRUE; }
   else       { return FALSE;                            }
}


BOOLEAN sLST::Goto(CARDINAL pos)
{
   sNODE* node=Node(pos);
   if (node!=0) { currnode=node; return TRUE; }
   else         { return FALSE;               }
}

BOOLEAN sLIST::Goto(CARDINAL pos)
{
   sNODE* node=Node(pos);
   if (node!=0) { currnode=node; currpos=pos; return TRUE; }
   else         { return FALSE;                            }
}

BOOLEAN dLST::Goto(CARDINAL pos)
{
   dNODE* node=Node(pos);
   if (node!=0) { currnode=node; return TRUE; }
   else         { return FALSE;               }
}

BOOLEAN dLIST::Goto(CARDINAL pos)
{
   dNODE* node=Node(pos);
   if (node!=0) { currnode=node; currpos=pos; return TRUE; }
   else         { return FALSE;                            }
}


BOOLEAN sLST::GotoNext()
{
   sNODE* node=Next();
   if (node!=0) { currnode=node; return TRUE; }
   else         { return FALSE;               }
}

BOOLEAN sLIST::GotoNext()
{
   sNODE* node=Next();
   if (node!=0) { currnode=node; currpos++; return TRUE; }
   else         { return FALSE;                          }
}

BOOLEAN dLST::GotoNext()
{
   dNODE* node=Next();
   if (node!=0) { currnode=node; return TRUE; }
   else         { return FALSE;               }
}

BOOLEAN dLIST::GotoNext()
{
   dNODE* node=Next();
   if (node!=0) { currnode=node; currpos++; return TRUE; }
   else         { return FALSE;                          }
}


BOOLEAN sLST::GotoPrev()
{
   sNODE* node=Prev();
   if (node!=0) { currnode=node; return TRUE; }
   else         { return FALSE;               }
}

BOOLEAN sLIST::GotoPrev()
{
   sNODE* node=Prev();
   if (node!=0) { currnode=node; currpos--; return TRUE; }
   else         { return FALSE;                          }
}

BOOLEAN dLST::GotoPrev()
{
   dNODE* node=Prev();
   if (node!=0) { currnode=node; return TRUE; }
   else         { return FALSE;               }
}

BOOLEAN dLIST::GotoPrev()
{
   dNODE* node=Prev();
   if (node!=0) { currnode=node; currpos--; return TRUE; }
   else         { return FALSE;                          }
}


BOOLEAN sLST::GotoNextOrFirst()
{
   if (firstnode==0) return FALSE;
   sNODE* node=Next();
   if (node!=0) currnode=node;
   else         currnode=firstnode;
   return TRUE;
}

BOOLEAN sLIST::GotoNextOrFirst()
{
   if (firstnode==0) return FALSE;
   sNODE* node=Next();
   if (node!=0) { currnode=node;      currpos++; }
   else         { currnode=firstnode; currpos=1; }
   return TRUE;
}

BOOLEAN dLST::GotoNextOrFirst()
{
   if (firstnode==0) return FALSE;
   dNODE* node=Next();
   if (node!=0) currnode=node;
   else         currnode=firstnode;
   return TRUE;
}

BOOLEAN dLIST::GotoNextOrFirst()
{
   if (firstnode==0) return FALSE;
   dNODE* node=Next();
   if (node!=0) { currnode=node;      currpos++; }
   else         { currnode=firstnode; currpos=1; }
   return TRUE;
}


BOOLEAN sLST::GotoPrevOrLast()
{
   if (firstnode==0) return FALSE;
   sNODE* node=Prev();
   if (node!=0) currnode=node;
   else         currnode=Last();
   return TRUE;
}

BOOLEAN sLIST::GotoPrevOrLast()
{
   if (firstnode==0) return FALSE;
   sNODE* node=Prev();
   if (node!=0) { currnode=node;     currpos--;       }
   else         { currnode=lastnode; currpos=numelem; }
   return TRUE;
}

BOOLEAN dLST::GotoPrevOrLast()
{
   if (firstnode==0) return FALSE;
   dNODE* node=Prev();
   if (node!=0) currnode=node;
   else         currnode=Last();
   return TRUE;
}

BOOLEAN dLIST::GotoPrevOrLast()
{
   if (firstnode==0) return FALSE;
   dNODE* node=Prev();
   if (node!=0) { currnode=node;     currpos--;       }
   else         { currnode=lastnode; currpos=numelem; }
   return TRUE;
}


BOOLEAN sLST::GotoFirst()
{
   if (firstnode==0) return FALSE;
   currnode = firstnode;
   return TRUE;
}

BOOLEAN sLIST::GotoFirst()
{
   if (firstnode==0) return FALSE;
   currnode = firstnode;
   currpos  = 1;
   return TRUE;
}

BOOLEAN dLST::GotoFirst()
{
   if (firstnode==0) return FALSE;
   currnode = firstnode;
   return TRUE;
}

BOOLEAN dLIST::GotoFirst()
{
   if (firstnode==0) return FALSE;
   currnode = firstnode;
   currpos  = 1;
   return TRUE;
}


BOOLEAN sLST::GotoLast()
{
   if (firstnode==0) return FALSE;
   currnode = Last();
   return TRUE;
}

BOOLEAN sLIST::GotoLast()
{
   if (firstnode==0) return FALSE;
   currnode = lastnode;
   currpos  = numelem;
   return TRUE;
}

BOOLEAN dLST::GotoLast()
{
   if (firstnode==0) return FALSE;
   currnode = Last();
   return TRUE;
}

BOOLEAN dLIST::GotoLast()
{
   if (firstnode==0) return FALSE;
   currnode = lastnode;
   currpos  = numelem;
   return TRUE;
}


BOOLEAN sLST::InsertFirst(sNODE* node)
{
   if (node==0 || node->nextnode!=0) return FALSE;
   if (firstnode==0) { node->nextnode=node; currnode=node; }
   else              { node->nextnode=firstnode;           }
   firstnode = node;
   return TRUE;
}

BOOLEAN sLIST::InsertFirst(sNODE* node)
{
   if (sLST::InsertFirst(node)!=TRUE) return FALSE;
   currpos++;
   numelem++;
   if (numelem==1) lastnode=firstnode;
   return TRUE;
}

BOOLEAN dLST::InsertFirst(dNODE* node)
{
   if (node==0 || node->nextnode!=0 || node->prevnode!=0) return FALSE;
   if (firstnode==0)
   {
      node->nextnode = node;
      currnode       = node;
   }
   else
   {
      firstnode->prevnode = node;
      node->nextnode      = firstnode;
   }
   node->prevnode = node;
   firstnode      = node;
   return TRUE;
}

BOOLEAN dLIST::InsertFirst(dNODE* node)
{
   if (dLST::InsertFirst(node)!=TRUE) return FALSE;
   currpos++;
   numelem++;
   if (numelem==1) lastnode=firstnode;
   return TRUE;
}


BOOLEAN sLST::InsertLast(sNODE* node)
{
   if (node==0 || node->nextnode!=0) return FALSE;
   sNODE* n=Last();
   if (n!=0) { n->nextnode=node;              }
   else      { currnode=node; firstnode=node; }
   node->nextnode = node;
   return TRUE;
}

BOOLEAN sLIST::InsertLast(sNODE* node)
{
   if (sLST::InsertLast(node)!=TRUE) return FALSE;
   lastnode=node;
   numelem++;
   if (numelem==1) currpos=1;
   return TRUE;
}

BOOLEAN dLST::InsertLast(dNODE* node)
{
   if (node==0 || node->nextnode!=0 || node->prevnode!=0) return FALSE;
   dNODE* n=Last();
   if (n!=0) { node->prevnode=n;    n->nextnode=node;                 }
   else      { node->prevnode=node; currnode=node;    firstnode=node; }
   node->nextnode = node;
   return TRUE;
}

BOOLEAN dLIST::InsertLast(dNODE* node)
{
   if (dLST::InsertLast(node)!=TRUE) return FALSE;
   lastnode=node;
   numelem++;
   if (numelem==1) currpos=1;
   return TRUE;
}


BOOLEAN sLST::Insert(sNODE* node, CARDINAL pos)
{
   if (node==0 || node->nextnode!=0 || pos<1) return FALSE;
   if (pos==1)       return InsertFirst(node);
   if (firstnode==0) return FALSE;
   sNODE* n=firstnode;
   for ( CARDINAL i=2; i<pos; i++ )
   {
      if (n->nextnode==n) return FALSE;
      n = n->nextnode;
   }
   if (n->nextnode==n) node->nextnode=node;
   else                node->nextnode=n->nextnode;
   n->nextnode = node;
   return TRUE;
}

BOOLEAN sLIST::Insert(sNODE* node, CARDINAL pos)
{
   if (sLST::Insert(node,pos)!=TRUE) return FALSE;
   numelem++;
   currpos = Position(currnode);
   if (pos==numelem) lastnode=node;
   return TRUE;
}

BOOLEAN dLST::Insert(dNODE* node, CARDINAL pos)
{
   if (node==0 || node->nextnode!=0 || node->prevnode!=0 || pos<1) return FALSE;
   if (pos==1)       return InsertFirst(node);
   if (firstnode==0) return FALSE;
   dNODE* n=firstnode;
   for ( CARDINAL i=2; i<pos; i++ )
   {
      if (n->nextnode==n) return FALSE;
      n = n->nextnode;
   }
   if (n->nextnode==n)
   {
      node->nextnode = node;
   }
   else
   {
      node->nextnode        = n->nextnode;
      n->nextnode->prevnode = node;
   }
   n->nextnode = node;
   node->prevnode = n;
   return TRUE;
}

BOOLEAN dLIST::Insert(dNODE* node, CARDINAL pos)
{
   if (dLST::Insert(node,pos)!=TRUE) return FALSE;
   numelem++;
   currpos = Position(currnode);
   if (pos==numelem) lastnode=node;
   return TRUE;
}


BOOLEAN sLST::Insert(sNODE* node)
{
   if (node==0 || node->nextnode!=0) return FALSE;
   if (currnode!=0)
   {
      node->nextnode = currnode;
      sNODE* n=Prev();
      if (n!=0) n->nextnode=node;
      else      firstnode  =node;
   }
   else
   {
      node->nextnode = node;
      firstnode      = node;
   }
   currnode = node;
   return TRUE;
}

BOOLEAN sLIST::Insert(sNODE* node)
{
   if (sLST::Insert(node)!=TRUE) return FALSE;
   numelem++;
   if (numelem==1) { currpos=1; lastnode=firstnode; }
   return TRUE;
}

BOOLEAN dLST::Insert(dNODE* node)
{
   if (node==0 || node->nextnode!=0 || node->prevnode!=0) return FALSE;
   if (currnode!=0)
   {
      node->nextnode = currnode;
      if (currnode->prevnode==currnode)
      {
         node->prevnode = node;
         firstnode      = node;
      }
      else
      {
         currnode->prevnode->nextnode = node;
         node->prevnode               = currnode->prevnode;
      }
      currnode->prevnode = node;
   }
   else
   {
      node->nextnode = node;
      node->prevnode = node;
      firstnode      = node;
   }
   currnode = node;
   return TRUE;
}

BOOLEAN dLIST::Insert(dNODE* node)
{
   if (dLST::Insert(node)!=TRUE) return FALSE;
   numelem++;
   if (numelem==1) { currpos=1; lastnode=firstnode; }
   return TRUE;
}


BOOLEAN sLST::InsertAfter(sNODE* node)
{
   if (node==0 || node->nextnode!=0) return FALSE;
   if (currnode!=0)
   {
      if (currnode->nextnode==currnode)
      {
         node->nextnode = node;
      }
      else
      {
         node->nextnode = currnode->nextnode;
      }
      currnode->nextnode = node;
   }
   else
   {
      node->nextnode = node;
      firstnode      = node;
   }
   currnode = node;
   return TRUE;
}

BOOLEAN sLIST::InsertAfter(sNODE* node)
{
   if (sLST::InsertAfter(node)!=TRUE) return FALSE;
   numelem++;
   currpos++;
   if (numelem==currpos) lastnode=currnode;
   return TRUE;
}

BOOLEAN dLST::InsertAfter(dNODE* node)
{
   if (node==0 || node->nextnode!=0 || node->prevnode!=0) return FALSE;
   if (currnode!=0)
   {
      node->prevnode = currnode;
      if (currnode->nextnode==currnode)
      {
         node->nextnode = node;
      }
      else
      {
         currnode->nextnode->prevnode = node;
         node->nextnode               = currnode->nextnode;
      }
      currnode->nextnode = node;
   }
   else
   {
      node->prevnode = node;
      node->nextnode = node;
      firstnode      = node;
   }
   currnode = node;
   return TRUE;
}

BOOLEAN dLIST::InsertAfter(dNODE* node)
{
   if (dLST::InsertAfter(node)!=TRUE) return FALSE;
   numelem++;
   currpos++;
   if (numelem==currpos) lastnode=currnode;
   return TRUE;
}


BOOLEAN sLST::Replace(sNODE* node)
{
   if (node==0 || node->nextnode!=0) return FALSE;
   if (currnode==0) return FALSE;
   if (currnode->nextnode==currnode) node->nextnode = node;
   else                              node->nextnode = currnode->nextnode;
   sNODE* n=Prev();
   if (n==0) firstnode=node; 
   else      n->nextnode=node;
   currnode->nextnode = 0;
   currnode = node;
   return TRUE;
}

BOOLEAN sLIST::Replace(sNODE* node)
{
   if (sLST::Replace(node)!=TRUE) return FALSE;
   if (currpos==numelem) lastnode=currnode;
   return TRUE;
}

BOOLEAN dLST::Replace(dNODE* node)
{
   if (node==0 || node->nextnode!=0 || node->prevnode!=0) return FALSE;
   if (currnode==0) return FALSE;
   if (currnode->nextnode==currnode)
   {
      node->nextnode = node;
   }
   else
   {
      node->nextnode           = currnode->nextnode;
      node->nextnode->prevnode = node;
   }
   if (currnode->prevnode==currnode)
   {
      node->prevnode = node;
      firstnode      = node;
   }
   else
   {
      node->prevnode           = currnode->prevnode;
      node->prevnode->nextnode = node;
   }
   currnode->nextnode = 0;
   currnode->prevnode = 0;
   currnode = node;
   return TRUE;
}

BOOLEAN dLIST::Replace(dNODE* node)
{
   if (dLST::Replace(node)!=TRUE) return FALSE;
   if (currpos==numelem) lastnode=currnode;
   return TRUE;
}


BOOLEAN sLST::RemoveFirst()
{
   if (firstnode==0)        return FALSE;
   if (firstnode==currnode) return Remove();
   sNODE* n=firstnode;
   firstnode   = firstnode->nextnode;
   n->nextnode = 0;
   return TRUE;
}

BOOLEAN sLIST::RemoveFirst()
{
   if (firstnode==currnode)       return Remove();
   if (sLST::RemoveFirst()!=TRUE) return FALSE;
   numelem--;
   currpos--;
   return TRUE;
}

BOOLEAN dLST::RemoveFirst()
{
   if (firstnode==0)        return FALSE;
   if (firstnode==currnode) return Remove();
   dNODE* n=firstnode;
   firstnode           = firstnode->nextnode;
   firstnode->prevnode = firstnode;
   n->nextnode = 0;
   n->prevnode = 0;
   return TRUE;
}

BOOLEAN dLIST::RemoveFirst()
{
   if (firstnode==currnode)       return Remove();
   if (dLST::RemoveFirst()!=TRUE) return FALSE;
   numelem--;
   currpos--;
   return TRUE;
}


BOOLEAN sLST::RemoveLast()
{
   if (firstnode==0) return FALSE;
   sNODE* n=firstnode;
   while (n->nextnode->nextnode!=n->nextnode) n=n->nextnode;
   if (n->nextnode==currnode) return Remove();
   n->nextnode->nextnode = 0;
   n->nextnode = n;
   return TRUE;
}

BOOLEAN sLIST::RemoveLast()
{
   if (firstnode==0) return FALSE;
   sNODE* n=firstnode;
   while (n->nextnode->nextnode!=n->nextnode) n=n->nextnode;
   if (n->nextnode==currnode) return Remove();
   n->nextnode->nextnode = 0;
   n->nextnode = n;
   lastnode = n;
   numelem--;
   return TRUE;
}

BOOLEAN dLST::RemoveLast()
{
   if (firstnode==0) return FALSE;
   dNODE* n=Last();
   if (n==currnode) return Remove();
   n->prevnode->nextnode = n->prevnode;
   n->nextnode = 0;
   n->prevnode = 0;
   return TRUE;
}

BOOLEAN dLIST::RemoveLast()
{
   if (firstnode==0)       return FALSE;
   if (lastnode==currnode) return Remove();
   lastnode = lastnode->prevnode;
   lastnode->nextnode->nextnode = 0;
   lastnode->nextnode->prevnode = 0;
   lastnode->nextnode = lastnode;
   numelem--;
   return TRUE;
}


BOOLEAN sLST::Remove(sNODE* node)
{
   if (node==0)        return FALSE;
   if (node==currnode) return Remove();
   if (node==firstnode)
   {
      firstnode = firstnode->nextnode;
   }
   else
   {
      sNODE* n=firstnode;
      if (n==0) return FALSE;
      while (n->nextnode!=node)
      {
         if (n->nextnode==n) return FALSE;
         n = n->nextnode;
      }
      if (node->nextnode==node) n->nextnode = n;
      else                      n->nextnode = node->nextnode;
   }
   node->nextnode = 0;
   return TRUE;
}

BOOLEAN sLIST::Remove(sNODE* node)
{
   if (node==0) return FALSE;
   if (sLST::Remove(node)!=TRUE) return FALSE;
   numelem--;
   currpos = Position(currnode);
   return TRUE;
}

BOOLEAN dLST::Remove(dNODE* node)
{
   if (node==0)        return FALSE;
   if (node==currnode) return Remove();
   if (node==firstnode)
   {
      firstnode           = node->nextnode;
      firstnode->prevnode = firstnode;
   }
   else
   {
      if (Position(node)==0) return FALSE;
      if (node->nextnode==node)
      {
         node->prevnode->nextnode = node->prevnode;
      }
      else
      {
         node->prevnode->nextnode = node->nextnode;
         node->nextnode->prevnode = node->prevnode;
      }
   }
   node->nextnode = 0;
   node->prevnode = 0;
   return TRUE;
}

BOOLEAN dLIST::Remove(dNODE* node)
{
   if (node==0) return FALSE;
   if (node==lastnode) lastnode=node->prevnode;
   if (dLST::Remove(node)!=TRUE) return FALSE;
   numelem--;
   currpos = Position(currnode);
   return TRUE;
}


BOOLEAN sLST::Remove()
{
   if (currnode==0) return FALSE;
   sNODE* n =Prev();
   sNODE* cn=currnode;
   if (n!=0)
   {
      if (cn->nextnode==cn) n->nextnode=n;
      else                  n->nextnode=cn->nextnode;
      currnode = n;
   }
   else
   {
      if (cn->nextnode==cn) { currnode=0;            firstnode=0;        }
      else                  { currnode=cn->nextnode; firstnode=currnode; }
   }
   cn->nextnode = 0;
   return TRUE;
}

BOOLEAN sLIST::Remove()
{
   if (sLST::Remove()!=TRUE) return FALSE;
   numelem--;
   if (currpos>numelem) { currpos=numelem; lastnode=currnode; }
   return TRUE;
}

BOOLEAN dLST::Remove()
{
   if (currnode==0) return FALSE;
   dNODE* cn=currnode;
   if (cn->prevnode!=cn)
   {
      if (cn->nextnode==cn)
      {
         cn->prevnode->nextnode = cn->prevnode;
         currnode               = cn->prevnode;
      }
      else
      {
         cn->prevnode->nextnode = cn->nextnode;
         cn->nextnode->prevnode = cn->prevnode;
         currnode               = cn->nextnode;
      }
   }
   else
   {
      if (cn->nextnode==cn)
      {
         currnode  = 0;
         firstnode = 0;
      }
      else
      {
         cn->nextnode->prevnode = cn->nextnode;
         currnode               = cn->nextnode;
         firstnode              = cn->nextnode;
      }
   }
   cn->nextnode = 0;
   cn->prevnode = 0;
   return TRUE;
}

BOOLEAN dLIST::Remove()
{
   if (dLST::Remove()!=TRUE) return FALSE;
   numelem--;
   if (currpos>numelem) { currpos=numelem; lastnode=currnode; }
   return TRUE;
}


void sLST::RemoveAll()
{
   while (currnode!=0) Remove();
   return;
}

void sLIST::RemoveAll()
{
   while (currnode!=0) Remove();
   return;
}

void dLST::RemoveAll()
{
   while (currnode!=0) Remove();
   return;
}

void dLIST::RemoveAll()
{
   while (currnode!=0) Remove();
   return;
}


void sLST::Test()
{
   *ctest << "\nclass sLST:       firstnode   = " << firstnode
          << "\n                  currnode    = " << currnode;
   sNODE* n=firstnode;
   *ctest << "\nnodes contained in the list:";
   while ( n!=0 )
   {
      n->Test();
      n=n->Next();
   }
   return;
}

void sLIST::Test()
{
   *ctest << "\nclass sLIST:      firstnode   = " << firstnode
          << "\n                  currnode    = " << currnode
          << "\n                  lastnode    = " << lastnode
          << "\n                  numelem     = " << numelem
          << "\n                  currpos     = " << currpos;
   sNODE* n=firstnode;
   *ctest << "\nnodes contained in the list:";
   while ( n != 0 )
   {
      n->Test();
      n=n->Next();
   }
   return;
}

void dLST::Test()
{
   *ctest << "\nclass dLST:       firstnode   = " << firstnode
          << "\n                  currnode    = " << currnode;
   dNODE* n=firstnode;
   *ctest << "\nnodes contained in the list:";
   while ( n!=0 )
   {
      n->Test();
      n=n->Next();
   }
   return;
}

void dLIST::Test()
{
   *ctest << "\nclass dLIST:      firstnode   = " << firstnode
          << "\n                  currnode    = " << currnode
          << "\n                  lastnode    = " << lastnode
          << "\n                  numelem     = " << numelem
          << "\n                  currpos     = " << currpos;
   dNODE* n=firstnode;
   *ctest << "\nnodes contained in the list:";
   while ( n != 0 )
   {
      n->Test();
      n=n->Next();
   }
   return;
}

//////////////////////////////////////////////////////////////////////////////

