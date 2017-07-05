/*
// #include "kscreen.h"
// #include "kpnt.h"
// #include "krct.h"
// #include "ktool.h"
*/

#include "stdafx.h"

#include "DROBJ_W.H"

/*
static void anytool(DrawTools* tool,
                    COLOR pen_col,
                    PAT_STYLE pen_pat,
                    short w,
                    COLOR brshcol,
                    PAT_STYLE brshpat,
                    DRAW_MODE amode);

*/
// =====================================================================
// DrawObj
// =====================================================================
  
 BOOLEAN DrawObj::range_defined() {

    return (RANGE.xmin() !=RANGE.xmax() && RANGE.ymin() != RANGE.ymax());
}

DrawObj::DrawObj(CDC* DeviceContext, BOOLEAN delete_as_sub) {
    pDC=DeviceContext; 
    subobject=0;
    auto_delete=delete_as_sub;
    drag=0;
    AREA=FALSE;
// *    DragTool();
// *    RangeTool();
// *    ObjTool();
}

BOOLEAN DrawObj::delete_subobjects() {
  BOOLEAN candidate=FALSE;
  
//  delete range;
  
  if (subobject) candidate=subobject->delete_subobjects();
  else return FALSE;
  
  if (candidate) {
     delete subobject;
     subobject=0;
  }
  return auto_delete;
}

DrawObj::~DrawObj() {

    delete_subobjects();
}

void DrawObj::Set(BOOLEAN delete_as_sub,
                BOOLEAN for_all_subobjects) {
    DrawObj* local=subobject;
    
    auto_delete=delete_as_sub;
    while (local && for_all_subobjects) {
       local->auto_delete=delete_as_sub;
       local=local->subobject;
    }
}

DrawObj* DrawObj::copy_all() {
    DrawObj* Copy;
    DrawObj* local=subobject;
    DrawObj* localcopy;
    
    Copy=copy_object();
    localcopy=Copy;
    while (local) {
      localcopy->subobject=local->copy_object();
      localcopy=localcopy->subobject;
      local=local->subobject;
    }

    return Copy;
}
    
Rct DrawObj::GetRange() {
     Rct r;
     
//     r.set(RANGE[0].x(),RANGE[2].y(),RANGE[2].x(),RANGE[0].y());
     r.set(RANGE.xmin(),RANGE.ymax(),RANGE.xmax(),RANGE.xmin());
     return r;
}

// =====================================================================
// Drawtools
// =====================================================================
/*
static void anytool(DrawTools* tool,
                    COLOR pen_col,
                    PAT_STYLE pen_pat,
                    short w,
                    COLOR brshcol,
                    PAT_STYLE brshpat,
                    DRAW_MODE amode) {
            
   switch (w) { // ist there an error in XVT ??? concerning width ???
     case 0: w=2;
       break;
     case 1: w=2;
       break;
     case 2: w=1;
       break;
     case 3: w=3;
       break;
     default: w=3;
       break;
   } // end of width correction
   
   Pen apen(pen_col, pen_pat, w);
   Brush abrush(brshcol, brshpat);
   
   tool->pen(apen);
   tool->brush(abrush);
   tool->mode(amode);
}

void DrawObj::DragTool(COLOR showpen_col,	    COLOR hidepen_col,
            PAT_STYLE showpen_pat,	PAT_STYLE hidepen_pat,
                short show_width,	    short hide_width,
                COLOR showbrshcol,	    COLOR hidebrshcol,
            PAT_STYLE showbrshpat,	PAT_STYLE hidebrshpat,
            DRAW_MODE show_mode,        DRAW_MODE hide_mode) {
            
   anytool(&dt_drag_show, showpen_col,showpen_pat,show_width,
                          showbrshcol,showbrshpat,
                          show_mode);   
   anytool(&dt_drag_hide, hidepen_col,hidepen_pat,hide_width,
                          hidebrshcol,hidebrshpat,
                          hide_mode);   
}
  
void DrawObj::RangeTool(COLOR showpen_col,	    COLOR hidepen_col,
            PAT_STYLE showpen_pat,	PAT_STYLE hidepen_pat,
                short show_width,	    short hide_width,
                COLOR showbrshcol,	    COLOR hidebrshcol,
            PAT_STYLE showbrshpat,	PAT_STYLE hidebrshpat,
            DRAW_MODE show_mode,        DRAW_MODE hide_mode) {
            
   anytool(&dt_range_show, showpen_col,showpen_pat,show_width,
                          showbrshcol,showbrshpat,
                          show_mode);   
   anytool(&dt_range_hide, hidepen_col,hidepen_pat,hide_width,
                          hidebrshcol,hidebrshpat,
                          hide_mode);
}
  
void DrawObj::ObjTool(COLOR showpen_col,	    COLOR hidepen_col,
            PAT_STYLE showpen_pat,	PAT_STYLE hidepen_pat,
                short show_width,	    short hide_width,
                COLOR showbrshcol,	    COLOR hidebrshcol,
            PAT_STYLE showbrshpat,	PAT_STYLE hidebrshpat,
            DRAW_MODE show_mode,        DRAW_MODE hide_mode) {
            
   anytool(&dt_object_show, showpen_col,showpen_pat,show_width,
                          showbrshcol,showbrshpat,
                          show_mode);   
   anytool(&dt_object_hide, hidepen_col,hidepen_pat,hide_width,
                          hidebrshcol,hidebrshpat,
                          hide_mode);
}
*/

// =====================================================================
// Drawing
// =====================================================================
  
BOOLEAN DrawObj::Check() {
    BOOLEAN ok;

    ok=(pDC && range_defined());
    if (ok) ok=(1 < N);
    return ok;
}

void DrawObj::Draw_Range() {

    if (Check()) {
// *       pDC->set_tools(dt_range_show);
//       pDC->rectangle(GetRange());
		 dt_range_show.rectangle(pDC,GetRange());
    }
    else {
#ifdef DEBUG
       cout << "\nCannot draw Range [drawobj.C: Draw_Range()]\n";
       cout.flush();
#endif
    }
}

void DrawObj::Hide_Range() {

    if (Check()) {
// *       pDC->set_tools(dt_range_hide);
//       pDC->rectangle(GetRange());
		dt_range_hide.rectangle(pDC,GetRange());
    }
    else {
#ifdef DEBUG
       cout << "\nCannot draw Range [drawobj.C: Hide_Range()]\n";
       cout.flush();
#endif
    }
}
 
void DrawObj::Draw_AllObjects() {
    DrawObj* local=subobject;
    
    Draw_Object();
    while (local) {
       local->Draw_Object();
       local=local->subobject;
    }
}
 
void DrawObj::Hide_AllObjects() {
    DrawObj* local=subobject;
    
    Hide_Object();
    while (local) {
       local->Hide_Object();
       local=local->subobject;
    }
}

// =====================================================================
// Subobjects
// =====================================================================
void DrawObj::Add(DrawObj* SubDrawObj) {
    DrawObj* local=this;
    int xMin=RANGE.xmin();
    int xMax=RANGE.xmax();
    int yMin=RANGE.ymin();
    int yMax=RANGE.ymax();
    while (local->subobject) { local=local->subobject; }
    local->subobject=SubDrawObj;
    local=this;
    while (local) {
        if (local->RANGE.xmin() < xMin)
      		xMin=local->RANGE.xmin();
        if (local->RANGE.ymin() < yMin)
      		yMin=local->RANGE.ymin();
        if (local->RANGE.xmax() > xMax)
      		xMax=local->RANGE.xmax();
        if (local->RANGE.ymax() > yMax)
      		yMax=local->RANGE.ymax();
      local=local->subobject;
     }
     RANGE.define(xMin,xMax,yMin,yMax);
}

// =====================================================================
// Copying or Moving a DrawObj
// =====================================================================

void DrawObj::RelMoveAll(int x, int y) {
    DrawObj* local=subobject;
    
    relmove(x,y);
    while (local) {
      local->relmove(x,y);
      local=local->subobject;
    }
}

void DrawObj::AbsMoveAll(int x, int y) {
    DrawObj* local=subobject;
    
    absmove(x,y);
    while (local) {
      local->absmove(x,y);
      local=local->subobject;
    }
}

void DrawObj::Init_Move() {	// to be called e.g. on mouse down;
//COLOR clr,
//                 PAT_STYLE  pat,
//                 short      width) {
//     Brush b(COLOR_WHITE,PAT_HOLLOW);
//     Pen   p(clr,pat,width);
     
     if (drag) { return;  // error
     }
     drag=new Frame;
     drag->copy_from(RANGE.frame());
//     pDC->set_tools(dt_drag_show);
//     pDC->rectangle(drag->GetFrame());
	   dt_drag_show.rectangle(pDC,drag->GetFrame());
     
}
  
void DrawObj::Drag_Range(int x,		// to be called e.g. on mouse move
                  int y) {
                  
     if (!drag) { return;  // error
     }
     x=x - drag->xmin();		// add relative
     y=y - drag->ymin();
//     pDC->set_tools(dt_drag_hide);
//     pDC->rectangle(drag->GetFrame());
	 dt_drag_hide.rectangle(pDC,drag->GetFrame());
     drag->add(x,y);
//     pDC->set_tools(dt_drag_show);
//     pDC->rectangle(drag->GetFrame());
	 dt_drag_show.rectangle(pDC,drag->GetFrame());
}

void DrawObj::Cancel_Move() {		// e.g. on some event

     if (!drag) { return;  // error
     }
//     pDC->set_tools(dt_drag_hide);		// finish dragging
//     pDC->rectangle(drag->GetFrame());
	 dt_drag_hide.rectangle(pDC,drag->GetFrame());
     delete drag; drag=0;
}
  
DrawObj* DrawObj::Do_Copy(int x, int y) {	// e.g. on mouse up
     DrawObj* Copy;
     
     if (!drag) { return 0;		// error
     }
     Cancel_Move();			// finish dragging

     Copy=copy_all();
     Copy->AbsMoveAll(x, y);
     Copy->Draw_AllObjects();
     
     return Copy;
}
  
void DrawObj::Do_Move(int x, int y) {	// e.g. on mouse up

     if (!drag) { return;  // error
     }
     Cancel_Move();			// finish dragging
     Hide_AllObjects();
     AbsMoveAll(x,y);
     Draw_AllObjects();
}

// =====================================================================
// Utilities
// =====================================================================



////////////////////////////////////////////////////////////////////////
// class DrawObjList
////////////////////////////////////////////////////////////////////////
/*
void DrawObjList::update_area() {
   DrawObj* local=First();
   
   if (!local) return;
   area.copy_from(local->RANGE.frame());
   local=local->Next();
   while (local) {
      area.surround(local->RANGE.frame());
      local=local->Next();
   }
}



void DrawObjList::add_as_last(DrawObj* object) {

    InsertLast(object);
    update_area();
}

void DrawObjList::add_as_first(DrawObj* object) {

    InsertFirst(object);
    update_area();
}

DrawObjList::DrawObjList(DrawObj* first) {

    add_as_first(first);
}

void DrawObjList::move_to_last(DrawObj* object) {

    Remove(object);
    InsertLast(object);
}

void DrawObjList::move_to_first(DrawObj* object) {
    
    Remove(object);
    InsertFirst(object);
}

void DrawObjList::remove(DrawObj* object) {
    
    Remove(object);
    update_area();
}

DrawObj*  DrawObjList::Get(const char* string) const { // NOT YET IMPLEMENTED
    strlen(string);
    return 0;
} // there is implementation missing ! 29.10.93

DrawObj* DrawObjList::topmost(int x, int y) {
    DrawObj* found=0;
    DrawObj* current=First();

    while (current) {
      if (current->RANGE.contains(x, y)) found=current;
      current=current->Next();
    }
    return found;
}

BOOLEAN DrawObjList::PntInArea(int x, int y) {

    return area.contains(x, y);
}

void DrawObjList::draw() {
    DrawObj* current=First();

    while (current) {
      current->Draw_AllObjects();
      current=current->Next();
    }
}

void DrawObjList::RelMoveAll(int x, int y) {
    DrawObj* current=First();

    while (current) {
      current->RelMoveAll(x, y);
      current=current->Next();
    }
}

void DrawObjList::AbsMoveAll(int x, int y) {
    DrawObj* current=First();

    while (current) {
      current->AbsMoveAll(x, y);
      current=current->Next();
    }
}

*/