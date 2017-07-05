#include "stdafx.h"
#include "drline_w.h"
#include <math.h>


AnyCurve::AnyCurve()
{
	n=0;
	p=NULL;
}


AnyCurve::~AnyCurve()
{
	if (p) delete [] p;
}


void AnyCurve::minmax(double* x, double& rmin, double& rmax)
{
	rmin=x[0]; rmax=x[0];
	for (int i=1; i < n; i++)
	{
		if(x[i] < rmin ) rmin=x[i];
		else if (rmax < x[i]) rmax=x[i];
	}
}

int AnyCurve::Set(int n, int* x, int* y, CRect target,
				int xMin,      int xMax, 
				int yMin,      int yMax)
{
	double* X=new double [n];
	double* Y=new double [n];
	for (int i=0; i < n; i++)
	{
		X[i]=x[i];
		Y[i]=y[i];
	}
	int e=Set(n, X, Y, target, xMin, xMax, yMin, yMax);
	delete [] X;
	delete [] Y;
	return e;
}

int AnyCurve::Set(int n, long  * x, long*   y, CRect target,
				long xMin,      long xMax, 
				long yMin,      long yMax)
{
	double* X=new double [n];
	double* Y=new double [n];
	for (int i=0; i < n; i++)
	{
		X[i]=x[i];
		Y[i]=y[i];
	}
	int e=Set(n, X, Y, target, xMin, xMax, yMin, yMax);
	delete [] X;
	delete [] Y;
	return e;
}

int AnyCurve::Set(int rn, double* x, double* y, CRect target,
						double xMin, double xMax, 
						double yMin, double yMax)
{
	n=rn;
	if (p) delete [] p;

	xmin=xMin; xmax=xMax; ymin=yMin; ymax=yMax;
	if (xmin == xmax || ymin == ymax)
	{
		minmax(x, xmin, xmax); 
		minmax(y, ymin, ymax); 
	}

	xdiff=xmax - xmin;
	ydiff=ymax - ymin;
	
	Xmin=target.left; Xmax=target.right;  Xdiff=Xmax - Xmin;
	Ymin=target.top;  Ymax=target.bottom; Ydiff=Ymax - Ymin;

	if (!Xdiff) return 2;
	if (!Ydiff) return 3;

	N=Xdiff+1;

	p=new CPoint[N];

	double shortmin=double(SHRT_MIN)/2;
	double shortmax=double(SHRT_MAX)/2;

	double Xfactor=Xdiff/xdiff;
	double Yfactor=Ydiff/ydiff;

	for (int I=0; I < N; I++)
	{
		double r;
		int    R;
		int    i=int(I * n / N);

		r=Xmin + Xfactor * (x[i] - xmin);
		if (r <=shortmin) R=SHRT_MIN/2;
		else if (shortmax <= r) R=SHRT_MAX/2;
		else R=int(r);
		p[I].x=R;

		r=Ymax - Yfactor * (y[i] - ymin);
		if (r <=shortmin) R=SHRT_MIN/2;
		else if (shortmax <= r) R=SHRT_MAX/2;
		else R=int(r);
		p[I].y=R;
	}

	if (rn < 2) return 1;
	return 0;
}
////////////////////////////////////////////////////////////////

void LineTool::init(CWnd* parentWindow)
{
	parent=parentWindow;
	pStyle=PS_SOLID;
	pWidth=1;

	pColor=C.GetCREF(black);
//	pColor=RGB(100,100,100);
	regiondefined=FALSE;
}

LineTool::LineTool(CWnd* parentWindow)
{
	init(parentWindow);
}

LineTool::LineTool(CRect cliprect, CWnd* parentWindow)
{
	init(parentWindow);
	region.CreateRectRgnIndirect(cliprect);
	regiondefined=TRUE;
}

LineTool::~LineTool()
{
	if (regiondefined) region.DeleteObject();
}

void LineTool::ClipRct(CRect cliprect)
{
	if (regiondefined) region.SetRectRgn(cliprect);
	else region.CreateRectRgnIndirect(cliprect);
	regiondefined=TRUE;
}

void LineTool::attachToWindow(CWnd* parentWindow)
{
	parent=parentWindow;
}

void LineTool::SetPen(int width, COLORREF color, int style)
{
	if (style != PS_SOLID) width=1;
	pWidth=width;
	if (1 < width) style=PS_SOLID;
	pColor=color;
	pStyle=style;
}

void LineTool::SetPen(int width, Color color, int style)
{
	if (style != PS_SOLID) width=1;
	pWidth=width;
	if (1 < width) style=PS_SOLID;
	pColor=C.GetCREF(color);
	pStyle=style;
}

void LineTool::SetStyle(int style)
{
	pStyle=style;
}

void LineTool::SetWidth(int width)
{
	pWidth=width;
}

void LineTool::SetColor(COLORREF foreground, COLORREF background)
{
	pColor=foreground;
	bColor=background;
}

void LineTool::SetColor(Color foreground, Color background)
{
	pColor=C.GetCREF(foreground);
	bColor=C.GetCREF(background);
}

BOOLEAN LineTool::curve(AnyCurve& curve)
{
	if (!curve.p) return FALSE;

	polyline(curve.p,curve.N);

	return TRUE;
}

BOOLEAN LineTool::customcurve(AnyCurve& curve, const char* pattern, double period)
{
	if (!curve.p) return FALSE;

	return custompolyline(curve.p,curve.N, pattern, period);

}

BOOLEAN LineTool::line(CPoint  p,   CPoint P)
{
	ASSERT(parent != NULL);
	CClientDC dc(parent);

	CPen pen;
	if (!pen.CreatePen(pStyle, pWidth, pColor))	return FALSE;
	oldpen=dc.SelectObject(&pen);
	if (regiondefined) dc.SelectClipRgn(&region);
	COLORREF oldb=dc.SetBkColor(bColor);
	
	dc.MoveTo(p.x,p.y);
	dc.LineTo(P.x,P.y);
	dc.SetBkColor(oldb);
	dc.SelectObject(oldpen);
	return TRUE;
}

static double dist_to_next(int delx, int dely) {
  if (delx) return sqrt((double)(delx*delx+dely*dely));
  else return dely;
}
static void find_next(int& delx, int& dely, double target, double limit,
                      int& Delx, int& Dely) {
  double f;
  f=target/limit;
  Delx=int(f*delx);
  Dely=int(f*dely);
}


BOOLEAN LineTool::custompolyline (CPoint* line, int N,  const char* pattern, double period)
{
	BOOLEAN results=TRUE;
	int saveStyle=pStyle;
 int n=N;
 int ndraw=0;
 int i;		// workpoint; also aux var;
 BOOLEAN newi;
 int iout;	// used for sensing linefeeds
 int j;		// aux var
 int patlen=strlen(pattern);    	// symbolic representation of pattern
 double* PAT=new double[patlen+1];	// to contain lengths of pattern
 int ipat;	// pointer to PAT
 int iaction;	// (if ipat % 2) then pendown else penup...
 double limit,	// distance to next point
      limitsum, // for debugging purposes
      target;	// distance to end of pattern
 double eps=period;
 BOOLEAN newpat;
 int Xdel,Ydel;// distance from workpoint to next neighbour
 int X0,Y0;	// workpoint <= local origin 
 int X, Y;	// coordinates of target
 CPoint PIECE[2];
 CPoint* P=&PIECE[0];
  
 // analyse pattern
    double sum=0.0;
    for (i=0; i < patlen; i++) 
	{
       j=pattern[i] - '0';
//       if (j < 0) paterror(pattern);
       sum=sum+j;
       PAT[i]=j;
	}
    for (i=0; i < patlen; i++) 
	{
       PAT[i]=period * PAT[i]/sum;
       if (PAT[i] < eps) eps=PAT[i];
    }
    if (eps < 1.0 || patlen == 0) 
	{
		delete PAT;
        return polyline(line, N);
    }
	pStyle=PS_SOLID;
    eps=0.00001 * eps;		// you would'nt believe it!
    
// draw
    i=0;
    X0=line[i].x; Xdel=line[i+1].x - X0; PIECE[0].x=X0;
    Y0=line[i].y; Ydel=line[i+1].y - Y0; PIECE[0].y=Y0;
    limit=dist_to_next(Xdel,Ydel);
    limitsum=0;
    newi=FALSE;
    ipat=0; iout=0; iaction=ipat % 2;
    target=PAT[ipat];
    find_next(Xdel,Ydel,target,limit,X,Y);
    newpat=FALSE;
//
    while (TRUE) 			// (i + 1 < n)
	{	 
      if (limit <= target) 
	  {
         target=target - limit;
         if (target < eps) newpat=TRUE;
         newi=TRUE;
      }         // end limit < target
      else 	  	// target < limit
	  {	
         if (limit < eps) newi=TRUE; 
         else 
		 {
            find_next(Xdel,Ydel,target,limit,X,Y);
            if (iaction == 0) 
			{
				PIECE[1].x=X0 + X;
                PIECE[1].y=Y0 + Y;
                ndraw++;
                results=results && polyline(P,2);
                PIECE[0]=PIECE[1];
            }
            else 
			{
                PIECE[0].x=X0 + X;
                PIECE[0].y=Y0 + Y;
            }
            X0=X0+X; Xdel=line[i+1].x - X0;
            Y0=Y0+Y; Ydel=line[i+1].y - Y0;
            limit=dist_to_next(Xdel,Ydel);
            newpat=TRUE;
         }
      } // end target <= limit
      if (newi == TRUE) 
	  {
         i++;
         if (i < n) 
		 {
            X0=line[i].x; Xdel=line[i+1].x - X0;
            Y0=line[i].y; Ydel=line[i+1].y - Y0;
            if (iaction == 0) 
			{
                PIECE[1].x=X0;
                PIECE[1].y=Y0;
                ndraw++;
                results=results && polyline(P,2);
                PIECE[0]=PIECE[1];
            }
            else 
			{
                PIECE[0].x=X0;
                PIECE[0].y=Y0;
            }
            limit=dist_to_next(Xdel,Ydel);
            newi=FALSE;
            if (i + 1 == n) break;
         }	// (i < n)
         else break;
      }
      if (newpat == TRUE) 
	  {
         ipat=++ipat % patlen;
         target=PAT[ipat];
         iaction=ipat % 2;
         limitsum=0;
         newpat=FALSE;
      }
    } // end while i + 1 < n

	pStyle=saveStyle;
	delete PAT;
	return results;
}

BOOLEAN LineTool::polyline(CPoint* line, int N)
{
	ASSERT(parent != NULL);
	CClientDC dc(parent);

	CPen pen;
	if (!pen.CreatePen(pStyle, pWidth, pColor))	return FALSE;
	oldpen=dc.SelectObject(&pen);
	
	COLORREF oldb=dc.SetBkColor(bColor);

	if (regiondefined) dc.SelectClipRgn(&region);
	
	dc.MoveTo(line[0].x,line[0].y);
	for (int n=1; n < N; n++)
	{
		dc.LineTo(line[n].x,line[n].y);
	}
	dc.SetBkColor(oldb);
	dc.SelectObject(oldpen);
	return TRUE;
}

BOOLEAN LineTool::polygon(CPoint* line, int N, BOOLEAN fill)
{
	ASSERT(parent != NULL);
	CClientDC dc(parent);

	CPen pen;
	if (!pen.CreatePen(pStyle, pWidth, pColor))	return FALSE;
	oldpen=dc.SelectObject(&pen);
	
	COLORREF oldb=dc.SetBkColor(bColor);
	if (regiondefined) dc.SelectClipRgn(&region);

	dc.MoveTo(line[0].x,line[0].y);
	for (int n=1; n < N; n++)
	{
		dc.LineTo(line[n].x,line[n].y);
	}
	dc.LineTo(line[0].x,line[0].y);

	dc.SetBkColor(oldb);
	dc.SelectObject(oldpen);
	return TRUE;
}

BOOLEAN LineTool::rectangle(CRect rect, BOOLEAN fill)
{
	return rectangle(rect.left,rect.top,rect.right,rect.bottom);
}

BOOLEAN LineTool::rectangle(CPoint p0, CPoint p, BOOLEAN fill)
{
	return rectangle(p0.x, p0.y, p.x, p.y);
}

BOOLEAN LineTool::rectangle(int x0, int y0, int x, int y, BOOLEAN fill)
{
	ASSERT(parent != NULL);
	CClientDC dc(parent);
//	if (fill) pColor=dc.GetNearestColor(pColor);
	CPen pen;
	if (!pen.CreatePen(pStyle, pWidth, pColor))	return FALSE;
	oldpen=dc.SelectObject(&pen);

	COLORREF oldb=dc.SetBkColor(bColor);
	if (regiondefined) dc.SelectClipRgn(&region);

	dc.MoveTo(x0,y0);

	dc.LineTo(x0,y);
	dc.LineTo(x,y);
	dc.LineTo(x,y0);
	dc.LineTo(x0,y0);

	dc.SetBkColor(oldb);
	dc.SelectObject(oldpen);
	return TRUE;
}

BOOLEAN LineTool::circle(CPoint center,  int radius)
{
	return circle(center.x, center.y,  radius);
}

BOOLEAN LineTool::circle(int cx, int cy, int radius)
{
	ASSERT(parent != NULL);
	CClientDC dc(parent);

	CPen pen;
	if (!pen.CreatePen(pStyle, pWidth, pColor))	return FALSE;
	oldpen=dc.SelectObject(&pen);
	
	COLORREF oldb=dc.SetBkColor(bColor);
	if (regiondefined) dc.SelectClipRgn(&region);

	dc.Arc(cx-radius,cy-radius,cx+radius,cy+radius,
		cx-radius,cy,cx-radius,cy);
/*
	dc.MoveTo(line[0].x,line[0].y);
	for (int n=1; n < N; n++)
	{
		dc.LineTo(line[n].x,line[n].y);
	}
	dc.LineTo(line[0].x,line[0].y);
*/
	dc.SetBkColor(oldb);
	dc.SelectObject(oldpen);
	return TRUE;
}

void LineTool::InvertLine(CPoint p0, int p1, 
						BOOLEAN horizontal,int skips)
{
	CClientDC dc(parent);
	COLORREF base=RGB(255,255,255);
	COLORREF pix;

	int dx, dy, len;
	int	x=p0.x;
	int	y=p0.y;

	if (horizontal)
	{
		len=p1 - x + 1;
		if (0 <= len) dx=1; else { dx=-1; len=-len; }
		dy=0;
	}
	else // vertical
	{
		len=p1 - y + 1;
		if (0 <= len) dy=1; else { dy=-1; len=-len; }
		dx=0;
	}
	if (20 < len)
	{
		int z;
		z=p1;
	}
	for (int i=0; i < len; i++)
	{
		if ((i % skips) == 0)
		{
			pix=dc.GetPixel(x,y);
			dc.SetPixel(x,y,base - pix);
		}
		x=x+dx;
		y=y+dy;
	}
}

void LineTool::InvertRct(CPoint a, CPoint b, int skips)
{
	InvertLine(a,b.x,TRUE,skips);
	InvertLine(b,a.x,TRUE,skips);
	InvertLine(a,b.y,FALSE,skips);
	InvertLine(b,a.y,FALSE,skips);
	/*
	InvertLine(a,b.x,TRUE,skips);
	InvertLine(b,a.x,TRUE,skips);
	InvertLine(a,b.y,FALSE,skips);
	InvertLine(b,a.y,FALSE,skips);
	*/
}

