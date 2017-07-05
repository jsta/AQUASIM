#ifndef DROBJ_W
#define DROBJ_W

#include "stdafx.h"

#include "aqcore\vectors.h"
#include "DRFRAM_W.H"
#include "DRMARK_W.H"
#include "DRTOOL_W.H"


////////////////////////////////////////////////////////////////////////
// class DrawObj
////////////////////////////////////////////////////////////////////////
// Author: J. Ruchti
// Creation: 12.03.1992
// Updated :
//
// Description:
//
// Programmer should define a PntCurve, then he may use class DrawObj
// to draw
//
////////////////////////////////////////////////////////////////////////
class DrawObj /* : public dNODE */ {

    friend class DrawCurve;
// *    friend class DrawObjList;

  protected:

    CDC* pDC;

    int        N;                               // number of points in CURVE
	
    Frame      RANGE;
    
    BOOLEAN    range_defined();

    BOOLEAN    auto_delete;                     // see Constructor/Destructor

    DrawObj*   subobject;                       // NOT in DrawObjCollection
    
    BOOLEAN    Check();                         // DeviceContext and range ok ?
    
    Frame*     drag;                            // see also DrawTools
    
    BOOLEAN    AREA;                            // set FALSE by ObjectTools
						// set TRUE  by AreaTools
						// default FALSE
    
    BOOLEAN    delete_subobjects();
    
  public:
/*  
    DrawObj*   Next() const
		  { return (DrawObj*)dNODE::Next(); }
    DrawObj*   Prev() const
		  { return (DrawObj*)dNODE::Prev(); }
*/  
    DrawObj(CDC* DeviceContext = NULL,      // Constructor: draw to DeviceContext
	    BOOLEAN delete_as_sub = FALSE);     // If a parent DrawObj is 
						// deleted, this subobject
						// should also be deleted 
						
    ~DrawObj();                                 // Destructor: deletes this
						// if this out of scope or if
						// a parent DrawObj is deleted
						// and this, was constructed
						// with delete_as_sub=TRUE
						
  
    void Set(CDC* DeviceContext) { pDC=DeviceContext; } // (re-) define DeviceContext
  
    CDC* current_DeviceContext() { return pDC; }
    
    void Set(BOOLEAN delete_as_sub = TRUE,      // (re-) define delete_as_sub
	   BOOLEAN for_all_subobjects = FALSE); // for this (and for
						// all subobjects ?)
						
    DrawObj* copy_all();                        // copies all subobjects,
						//  and then this to result
  
    /*const*/ Frame* ObjRange() { return &RANGE; }
    
    Rct GetRange();
  
    BOOLEAN contains(int x, int y)              // TRUE, if (x,y) in range
		    {return RANGE.contains(x,y); }

    virtual DrawObj* copy_object() = 0;         // result =  new DrawObj
						// copies this object to result
						// without copying subobject

    virtual void relmove(int to_x, int to_y) = 0;
    virtual void absmove(int to_x, int to_y) = 0;
  
  
// =====================================================================
// Drawtools: may be defined and set
//            externally, prior to drawing, using following Tools:
// =====================================================================
  DrawTools dt_drag_show;
  DrawTools dt_drag_hide;
  DrawTools dt_range_show;              // used by Draw_Range
  DrawTools dt_range_hide;
  DrawTools dt_object_show;             // used by Object, Draw_AllObjects
  DrawTools dt_object_hide;
/* 
  
  void DragTool(COLOR showpen_col=COLOR_BLACK,  COLOR hidepen_col=COLOR_WHITE,
	    PAT_STYLE showpen_pat=PAT_SOLID,    PAT_STYLE hidepen_pat=PAT_SOLID,
		short show_width=1,             short hide_width=1,
		COLOR showbrshcol=COLOR_WHITE,  COLOR hidebrshcol=COLOR_WHITE,
	    PAT_STYLE showbrshpat=PAT_SOLID,     PAT_STYLE hidebrshpat=PAT_NONE,
	    DRAW_MODE show_mode=M_COPY,     DRAW_MODE hide_mode=M_COPY);
  
  void RangeTool(COLOR showpen_col=COLOR_BLACK, COLOR hidepen_col=COLOR_WHITE,
	    PAT_STYLE showpen_pat=PAT_SOLID,    PAT_STYLE hidepen_pat=PAT_SOLID,
		short show_width=1,             short hide_width=1,
		COLOR showbrshcol=COLOR_WHITE,  COLOR hidebrshcol=COLOR_WHITE,
	    PAT_STYLE showbrshpat=PAT_SOLID,     PAT_STYLE hidebrshpat=PAT_NONE,
	    DRAW_MODE show_mode=M_COPY,     DRAW_MODE hide_mode=M_COPY);
  
  void ObjTool(COLOR showpen_col=COLOR_BLACK,   COLOR hidepen_col=COLOR_WHITE,
	    PAT_STYLE showpen_pat=PAT_SOLID,    PAT_STYLE hidepen_pat=PAT_SOLID,
		short show_width=1,             short hide_width=1,
		COLOR showbrshcol=COLOR_WHITE,  COLOR hidebrshcol=COLOR_WHITE,
	    PAT_STYLE showbrshpat=PAT_SOLID,     PAT_STYLE hidebrshpat=PAT_NONE,
	    DRAW_MODE show_mode=M_COPY,     DRAW_MODE hide_mode=M_COPY);


  void LineMode() { AREA=FALSE; }       // draw curve as an open polyline
  
  void AreaMode() { AREA=TRUE; }        // draw curve as a closed polygon
*/
  
// =====================================================================
// Drawing
// =====================================================================

  void Draw_Range();            // draw range of curve

  void Hide_Range();            // draw range of curve
  
  virtual void Draw_Object() = 0;
  
  virtual void Hide_Object() = 0;

  void Draw_AllObjects();       // draw curve, the all curves in subobject(s)

  void Hide_AllObjects();       // draw curve, the all curves in subobject(s)
/*
// Mark this curve:

    void mark(DrawMark* m, int next=1) {
       m->MarkCurve(N,next,CURVE);
    }
*/  
  
// =====================================================================
// Subobjects
// =====================================================================
  
  void Add(DrawObj* sub); // 1. appends sub at end of list of subobjects
			// 2. will correct range to contain this object and
			//    all subobjects
			// Note that sub may itself have a
			// list of subobjects
  
// =====================================================================
// Copying or Moving a DrawObj
// =====================================================================

  void RelMoveAll(int x, int y);// parallel transform on this and on
				// all subobjects

  void AbsMoveAll(int x, int y);// parallel transform on this and on
				// all subobjects
				
  void Init_Move();             // to be called e.g. on mouse down;
				// makes a copy of range,
				// sets DrawTools dt_drag_show
				// draws copy of range
				// sets mode of dt_object to XOR
  
  void Drag_Range(int x,        // to be called e.g. on mouse move
		 int y);        // sets DrawTools dt_drag_hide
				// draws (deletes) copy of range
				// moves copy of range
				// sets DrawTools dt_drag_show
				// draws copy of range 

  void Cancel_Move();           // e.g. on some event
				// sets DrawTools dt_drag_hide
				// draws (deletes) copy of range
  
  DrawObj* Do_Copy(int x, int y);       // e.g. on mouse up
				// draws copy of range = XORs it out
				// The result point to the copy at (x,y),
				// the result will be drawn on the screen,
				// this points to the old DrawObj left on
				// the screen,
				// deletes copy of range
  
  void Do_Move(int x, int y);   // e.g. on mouse up
				// draws copy of range  = XORs it out
				// the old DrawObj will be erased on screen,
				// this now points to DrawObj at (x,y),
				// this DrawObj will be drawn on the screen,
				// deletes copy of range
			
  
// =====================================================================
// Utilities
// =====================================================================


};


////////////////////////////////////////////////////////////////////////
// class DrawObjList
////////////////////////////////////////////////////////////////////////
// Author: J. Ruchti
// Creation: 19.03.1992
// Updated :
//
// Description:
//
// 
// 
//
////////////////////////////////////////////////////////////////////////
/*
class DrawObjList : private dLIST {

      DrawObj*  Get(CARDINAL pos) const
		  { return (DrawObj*)Node(pos); }
      DrawObj*  Get(const char* string) const;
		   // there is implementation missing ! 29.10.93

      CARDINAL Position() const
		  { return dLIST::Position(); }
      CARDINAL Position(DrawObj* sym) const
		  { return dLIST::Position(sym); }
      CARDINAL Position(const char* string)
		  { return Position(Get(string)); }
		  
      BOOLEAN  Goto(DrawObj* sym)
		  { return dLIST::Goto(sym); }
      BOOLEAN  Goto(const char* string)
		  { return Goto(Get(string)); }
      BOOLEAN  Goto(CARDINAL pos)
		  { return dLIST::Goto(pos); }
      BOOLEAN  GotoFirst()
		  { return dLIST::GotoFirst(); }
      BOOLEAN  GotoLast()
		  { return dLIST::GotoLast(); }
      BOOLEAN  GotoNext()
		  { return dLIST::GotoNext(); }
      BOOLEAN  GotoPrev()
		  { return dLIST::GotoPrev(); }
      BOOLEAN  GotoNextOrFirst()
		  { return dLIST::GotoNextOrFirst(); }
      BOOLEAN  GotoPrevOrLast()
		  { return dLIST::GotoPrevOrLast(); }

      BOOLEAN  InsertFirst(DrawObj* sym)
		  { return dLIST::InsertFirst(sym); }
      BOOLEAN  InsertLast(DrawObj* sym)
		  { return dLIST::InsertLast(sym); }
      BOOLEAN  Insert(DrawObj* sym, CARDINAL pos)
		  { return dLIST::Insert(sym,pos); }
      BOOLEAN  Insert(DrawObj* sym)
		  { return dLIST::Insert(sym); }
      BOOLEAN  InsertAfter(DrawObj* sym)
		  { return dLIST::InsertAfter(sym); }
//      BOOLEAN  InsertSorted(DrawObj* sym);
      BOOLEAN  Replace(DrawObj* sym)
		  { return dLIST::Replace(sym); }
      BOOLEAN  RemoveFirst()
		  { return dLIST::RemoveFirst(); }
      BOOLEAN  RemoveLast()
		  { return dLIST::RemoveLast(); }
      BOOLEAN  Remove(DrawObj* sym)
		  { return dLIST::Remove(sym); }
      BOOLEAN  Remove(CARDINAL pos)
		  { return dLIST::Remove(pos); }
      BOOLEAN  Remove()
		  { return dLIST::Remove(); }
//      BOOLEAN  Remove(const char* string)
//                  { return Remove(Get(string)); }
      void     RemoveAll()
		  { dLIST::RemoveAll(); }
      // ---------------------------------------------------------

	
	Frame   area;                           // rectangle enveloping
						// all objects in
						// collection
	
	

  public:
      // ---------------------------------------------------------
      BOOLEAN  Empty() const
		  { return dLIST::Empty(); }
      CARDINAL Size() const
		  { return dLIST::Size(); }

      DrawObj*  First() const
		  { return (DrawObj*)dLIST::First(); }
      DrawObj*  Last() const
		  { return (DrawObj*)dLIST::First(); }
      DrawObj*  Current() const
		  { return (DrawObj*)Node(); }
      DrawObj*  Next() const
		  { return (DrawObj*)dLIST::Next(); }
      DrawObj*  Prev() const
		  { return (DrawObj*)dLIST::Prev(); }
      // ---------------------------------------------------------

	DrawObjList(DrawObj* first = 0);
	
	~DrawObjList() {}                       // disconnects DrawObjs
						// withoud deleting them

	void add_as_last(DrawObj* object);
	void add_as_first(DrawObj* object);
	
	void move_to_last(DrawObj* object);
	void move_to_first(DrawObj* object);
	
	void remove(DrawObj* object);           // remove from Collection,
						// no deletion of object
	
	DrawObj* topmost(int x, int y);         // find topmost object
						// containing (x,y)
						// if not found: result = 0
	
	void update_area();                     // updates area
						// on add_as_last,
						// on add_as_first and
						// on remove; programmer
						// should call update_area
						// after moving a object
						// within a collection
						
	BOOLEAN PntInArea(int x, int y);        // TRUE if (x,y) in rectangle
						// enveloping all objects of
						// collection
	
	void draw();                            // draws all Object in
						// collection,
						// starting at bottom

	void RelMoveAll(int x, int y);          // parallel transform on all
						// Objects in collection

	void AbsMoveAll(int x, int y);          // parallel transform on all
						// Objects in collection
};
*/

#endif // DRAWOBJ
