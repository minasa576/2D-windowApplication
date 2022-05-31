#include <Windows.h>
#include <math.h>
#include <tchar.h>
#include <iostream>
#include <stack>
#include <vector>
#include <list>
#include <fstream>
#include <stdlib.h>

using namespace std;

#define FILE_MENU_CLEAR 1
#define FILE_MENU_SAVE 2
#define FILE_MENU_LOAD 3
#define FILE_MENU_EXIT 4

#define LINE_MENU_DDA 5
#define LINE_MENU_MIDPOINT 6
#define LINE_MENU_PARAMETRIC 7

#define CIRCLE_MENU_DIRECT 8
#define CIRCLE_MENU_POLAR 9
#define CIRCLE_MENU_ITERATIVE 10
#define CIRCLE_MENU_BRESENHAM 11
#define CIRCLE_MENU_FASTER_BRESENHAM 12

#define ELLIPSE_MENU_DIRECT 13
#define ELLIPSE_MENU_POLAR 14
#define ELLIPSE_MENU_MIDPOINT 15

#define CURVE_MENU_SPLINE 16

#define FILLING_QUARTER_MENU_CIRCLES 17

#define FILLING_MENU_CIR_LINE 18
#define FILLING_MENU_CIR_CIR 19
#define FILLING_MENU_SQU_HERMIT 20
#define FILLING_MENU_SQU_HERMIT 21
#define FILLING_MENU_CONVEX 22
#define FILLING_MENU_REC_BEZIR 23
#define FILLING_MENU_NONCONVEX 24
#define FILLING_MENU_RECURS 25 
#define FILLING_MENU_NONRECURS 26

#define SHAPE_COLOR_MENU_BLACK 27
#define SHAPE_COLOR_MENU_RED 28
#define SHAPE_COLOR_MENU_GREEN 29
#define SHAPE_COLOR_MENU_BLUE 30

#define Clipping_MENU_CIRCLEPOINT 31
#define Clipping_MENU_CIRCLELINE 32

#define Clipping_MENU_SQUAREPOINT 33
#define Clipping_MENU_SQUARELINE 34

#define Clipping_MENU_RECTANGLEPOINT 35
#define Clipping_MENU_RECTANGLELINE 36 
#define Clipping_MENU_RECTANGLEPOLYGON 37


LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

void addMenus(HWND);
HMENU hmenu;
COLORREF color;
void load(HWND, HDC&);
void save(HWND&);
//Start of save & load
bool HDCToFile(const char* FilePath, HDC Context, RECT Area, uint16_t BitsPerPixel = 24)
{
    uint32_t Width = Area.right - Area.left;
    uint32_t Height = Area.bottom - Area.top;
    BITMAPINFO Info;
    BITMAPFILEHEADER Header;
    memset(&Info, 0, sizeof(Info));
    memset(&Header, 0, sizeof(Header));
    Info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    Info.bmiHeader.biWidth = Width;
    Info.bmiHeader.biHeight = Height;
    Info.bmiHeader.biPlanes = 1;
    Info.bmiHeader.biBitCount = BitsPerPixel;
    Info.bmiHeader.biCompression = BI_RGB;
    Info.bmiHeader.biSizeImage = Width * Height * (BitsPerPixel > 24 ? 4 : 3);
    Header.bfType = 0x4D42;
    Header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    char* Pixels = NULL;
    HDC MemDC = CreateCompatibleDC(Context);
    HBITMAP Section = CreateDIBSection(Context, &Info, DIB_RGB_COLORS, (void**)&Pixels, 0, 0);
    DeleteObject(SelectObject(MemDC, Section));
    BitBlt(MemDC, 0, 0, Width, Height, Context, Area.left, Area.top, SRCCOPY);
    DeleteDC(MemDC);
    std::fstream hFile(FilePath, std::ios::out | std::ios::binary);
    if (hFile.is_open())
    {
        hFile.write((char*)&Header, sizeof(Header));
        hFile.write((char*)&Info.bmiHeader, sizeof(Info.bmiHeader));
        hFile.write(Pixels, (((BitsPerPixel * Width + 31) & ~31) / 8) * Height);
        hFile.close();
        DeleteObject(Section);
        return true;
    }
    DeleteObject(Section);
    return false;
}
void save(HWND& hWnd)
{
    HDC hdc = GetDC(hWnd);
    string fileName = "picture.bmp";
    if (fileName == "")
        return;
    int windowWidth;
    int windowHeight;
    RECT rect;
    if (GetWindowRect(hWnd, &rect))
    {
        windowWidth = rect.right - rect.left;
        windowHeight = rect.bottom - rect.top;
    }
    RECT rect1 = { 0, 0, windowWidth, windowHeight };
    HDCToFile(fileName.c_str(), hdc, rect1);
}
void load(HWND hWnd, HDC& hdc)
{
    string fileName = "picture.bmp";
    if (fileName == "")
        return;
    HBITMAP hBitmap;
    hBitmap = (HBITMAP)::LoadImage(NULL, L"picture.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    HDC hLocalDC;
    hLocalDC = CreateCompatibleDC(hdc);
    BITMAP qBitmap;
    int iReturn = GetObject(reinterpret_cast<HGDIOBJ>(hBitmap), sizeof(BITMAP), reinterpret_cast<LPVOID>(&qBitmap));
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hLocalDC, hBitmap);
    BOOL qRetBlit = BitBlt(hdc, 0, 0, qBitmap.bmWidth, qBitmap.bmHeight, hLocalDC, 0, 0, SRCCOPY);
    SelectObject(hLocalDC, hOldBmp);
    DeleteDC(hLocalDC);
    DeleteObject(hBitmap);
}
//End of save & load
void CreateConsole()
{
    if (!AllocConsole()) {
        return;
    }

    FILE* fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    std::cout.clear();
    std::clog.clear();
    std::cerr.clear();
    std::cin.clear();

    HANDLE hConOut = CreateFile(_T("CONOUT$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE hConIn = CreateFile(_T("CONIN$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
    SetStdHandle(STD_ERROR_HANDLE, hConOut);
    SetStdHandle(STD_INPUT_HANDLE, hConIn);
    std::wcout.clear();
    std::wclog.clear();
    std::wcerr.clear();
    std::wcin.clear();
}
void swap(int& x1, int& y1, int& x2, int& y2)
{
    int tmp = x1;
    x1 = x2;
    x2 = tmp;
    tmp = y1;
    y1 = y2;
    y2 = tmp;
}

int Round(double x)
{
    return (int)(x + 0.5);
}
//Start Of Line Algorithm
int xs, ys, xe, ye;
void SimpleDDA(HDC hdc, int xs, int ys, int xe, int ye, COLORREF color) {
    int dx = xe - xs;
    int dy = ye - ys;
    SetPixel(hdc, xs, ys, color);
    if (abs(dx) >= abs(dy))
    {
        int x = xs, xinc = dx > 0 ? 1 : -1;
        double y = ys, yinc = (double)dy / dx * xinc;
        while (x != xe)
        {
            x += xinc;
            y += yinc;
            SetPixel(hdc, x, round(y), color);
        }
    }
    else
    {
        int y = ys, yinc = dy > 0 ? 1 : -1;
        double x = xs, xinc = (double)dx / dy * yinc;
        while (y != ye)
        {
            x += xinc;
            y += yinc;
            SetPixel(hdc, round(x), y, color);
        }
    }
}
void DrawLineMidPoint(HDC hdc, int xs, int ys, int xe, int ye, COLORREF color) {
    int dx = xe - xs;
    int dy = ye - ys;
    int x = xs;
    int y = ys;
    int d;
    SetPixel(hdc, x, y, color);
    while (x < xe) {
        d = ((y + 1 - ys) * dx - (x + 0.5 - xs) * dy);
        if (d > 0) {
            x++;
        }
        y++;
        SetPixel(hdc, x, y, color);
    }
}
void Parametric_Line(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color)
{
    double dx = x2 - x1;
    double dy = y2 - y1;
    for (double t = 0; t < 1; t += 0.001)
    {
        int x = x1 + (dx * t);
        int y = y1 + (dy * t);
        SetPixel(hdc, x, y, color);
    }
}
//End Of Line Algorithm


// srart circle algorithms

void Draw8Points(HDC hdc, int xc, int yc, int a, int b, COLORREF color)
{
    SetPixel(hdc, xc + a, yc + b, color);
    SetPixel(hdc, xc - a, yc + b, color);
    SetPixel(hdc, xc - a, yc - b, color);
    SetPixel(hdc, xc + a, yc - b, color);
    SetPixel(hdc, xc + b, yc + a, color);
    SetPixel(hdc, xc - b, yc + a, color);
    SetPixel(hdc, xc - b, yc - a, color);
    SetPixel(hdc, xc + b, yc - a, color);
}
void DirectCircle(HDC hdc, int xc, int yc, int R, COLORREF color)
{
    int x = 0, y = R;
    int R2 = R * R;
    Draw8Points(hdc, xc, yc, x, y, color);
    while (x < y)
    {
        x++;
        y = round(sqrt((double)(R2 - x * x)));
        Draw8Points(hdc, xc, yc, x, y, color);
    }

}
void PolarCircle(HDC hdc, int xc, int yc, int R, COLORREF color)
{
    int x = R, y = 0;
    double theta = 0, dtheta = 1.0 / R;
    Draw8Points(hdc, xc, yc, x, y, color);
    while (x > y)
    {
        theta += dtheta;
        x = round(R * cos(theta));
        y = round(R * sin(theta));
        Draw8Points(hdc, xc, yc, x, y, color);
    }
}
void IterativePolarCircle(HDC hdc, int xc, int yc, int R, COLORREF color)
{
    double x = R, y = 0;
    double dtheta = 1.0 / R;
    double cdtheta = cos(dtheta), sdtheta = sin(dtheta);
    Draw8Points(hdc, xc, yc, R, 0, color);
    while (x > y)
    {
        double x1 = x * cdtheta - y * sdtheta;
        y = x * sdtheta + y * cdtheta;
        x = x1;
        Draw8Points(hdc, xc, yc, round(x), round(y), color);
    }
}
void BresenhamCircle(HDC hdc, int xc, int yc, int R, COLORREF color)
{
    int x = 0, y = R;
    int d = 1 - R;
    Draw8Points(hdc, xc, yc, x, y, color);
    while (x < y)
    {
        if (d < 0)
            d += 2 * x + 2;
        else
        {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
        Draw8Points(hdc, xc, yc, x, y, color);
    }
}
void FasterBresenhamCircle(HDC hdc, int xc, int yc, int R, COLORREF color)
{
    int x = 0, y = R;
    int d = 1 - R;
    int c1 = 3, c2 = 5 - 2 * R;
    Draw8Points(hdc, xc, yc, x, y, color);
    while (x < y)
    {
        if (d < 0)
        {
            d += c1;
            c2 += 2;
        }
        else
        {
            d += c2;
            c2 += 4;
            y--;
        }
        c1 += 2;
        x++;
        Draw8Points(hdc, xc, yc, x, y, color);
    }

}
// end of circle algorithms

//start of Ellipse algorithms
void draw4Points(HDC hdc, int xc, int yc, int x, int y, COLORREF color)
{
    SetPixel(hdc, xc + x, yc + y, color);
    SetPixel(hdc, xc - x, yc - y, color);
    SetPixel(hdc, xc - x, yc + y, color);
    SetPixel(hdc, xc + x, yc - y, color);
}
void DirectEllipse(HDC hdc, int xc, int yc, int A, int R, COLORREF color)
{
    int x = 0;
    double y = R;
    draw4Points(hdc, xc, yc, 0, R, color);
    while (x * (R * R) < y * (A * A))
    {
        x++;
        y = R * sqrt(1.0 - (double)x * x / (A * A));
        draw4Points(hdc, xc, yc, x, Round(y), color);
    }
    int y1 = 0;
    double x1 = A;
    draw4Points(hdc, xc, yc, A, 0, color);
    while (x1 * (R * R) > y1 * (A * A))
    {
        y1++;
        x1 = A * sqrt(1.0 - (double)y1 * y1 / (R * R));
        draw4Points(hdc, xc, yc, Round(x1), Round(y1), color);
    }
}
void PolarEllipse(HDC hdc, int xc, int yc, int A, int R, COLORREF color)
{
    double x = A, y = 0, dtheta = 1.0 / max(A, R);

    for (double theta = 0; theta < 3.1415; theta += dtheta)
    {
        x = A * cos(theta);
        y = R * sin(theta);
        draw4Points(hdc, xc, yc, Round(x), Round(y), color);
    }
}
void MidpointEllipse(HDC hdc, int xc, int yc, int a, int b, COLORREF color)
{
    double p = b * b - a * a * b + a * a / 4;
    double x = 0, y = b;
    while (2.0 * b * b * x <= 2.0 * a * a * y)
    {
        if (p < 0)
        {
            x++;
            p = p + 2 * b * b * x + b * b;
        }
        else
        {
            x++;
            y--;
            p = p + 2 * b * b * x - 2 * a * a * y - b * b;
        }
        SetPixel(hdc, xc + x, yc + y, color);
        SetPixel(hdc, xc + x, yc - y, color);
        SetPixel(hdc, xc - x, yc + y, color);
        SetPixel(hdc, xc - x, yc - y, color);
    }
    p = b * b * (x + 0.5) * (x + 0.5) + a * a * (y - 1) * (y - 1) - a * a * b * b;
    while (y > 0)
    {
        if (p <= 0)
        {
            x++;
            y--;
            p = p + 2 * b * b * x - 2 * a * a * y + a * a;
        }
        else
        {
            y--;
            p = p - 2 * a * a * y + a * a;
        }
        SetPixel(hdc, xc + x, yc + y, color);
        SetPixel(hdc, xc + x, yc - y, color);
        SetPixel(hdc, xc - x, yc + y, color);
        SetPixel(hdc, xc - x, yc - y, color);
    }
}
// end of Ellipse algorithms

// start circle with lines
int rrF;
bool isInside(int circleX, int circleY, int rad, int x, int y)
{
    if ((x - circleX) * (x - circleX) + (y - circleY) * (y - circleY) <= rad * rad)
        return true;
    else
        return false;
}
void drawLines(HDC hdc, int x, int y, int x_c, int y_c, int qNum)
{
    if (qNum == 1)
    {
        Parametric_Line(hdc, x_c + x, y_c - y, x_c, y_c, color);
        Parametric_Line(hdc, x_c + y, y_c - x, x_c, y_c, color);
    }
    else if (qNum == 2)
    {
        Parametric_Line(hdc, x_c - x, y_c + y, x_c, y_c, color);
        Parametric_Line(hdc, x_c - y, y_c + x, x_c, y_c, color);
    }
    else if (qNum == 3)
    {
        Parametric_Line(hdc, x_c - x, y_c - y, x_c, y_c, color);
        Parametric_Line(hdc, x_c - y, y_c - x, x_c, y_c, color);
    }
    else if (qNum == 4)
    {
        Parametric_Line(hdc, x_c + x, y_c + y, x_c, y_c, color);
        Parametric_Line(hdc, x_c + y, y_c + x, x_c, y_c, color);
    }
}
void filling(HDC hdc, int x_c, int y_c, int rr, int qNum)
{

    int x = 0, y = rr, d = 1 - rr, c1 = 3, c2 = 5 - (2 * rr);
    drawLines(hdc, x, y, x_c, y_c, qNum);
    while (x < y)
    {
        if (d < 0)
        {
            d += c1;
            c2 += 2;
        }
        else
        {
            d += c2;
            c2 += 4;
            y--;
        }
        c1 += 2;
        x++;
        drawLines(hdc, x, y, x_c, y_c, qNum);
    }
}
int quarterNum(HDC hdc, int x, int y, int rr, int xc, int yc)
{
    int qNum = 0;
    if (isInside(x, y, rr, xc, yc))
    {
        if (x > xc && y < yc)
        {
            qNum = 1;
        }
        else if (x < xc && y > yc)
        {
            qNum = 2;
        }
        else if (x < xc && y < yc)
        {
            qNum = 3;
        }
        else if (x > xc && y > yc)
        {
            qNum = 4;
        }
    }
    return qNum;

}
// end circle with lines


// statrt circle with other circles
void DrawPointsQuad(HDC hdc, int xc, int yc, int x, int y, COLORREF color, int qNum, int r, int i)
{
    if (qNum == 1)
    {
        if (xc + x >= (xc + i))
        {
            SetPixel(hdc, xc + x, yc - y, color);
        }
        if (xc + y >= (xc + i))
        {
            SetPixel(hdc, xc + y, yc - x, color);
        }

    }

    else if (qNum == 2)
    {
        if (yc + y >= (yc + i))
        {
            SetPixel(hdc, xc - x, yc + y, color);//quad three
        }
        if (yc + x >= (yc + i))
        {
            SetPixel(hdc, xc - y, yc + x, color);//quad three
        }

    }
    else if (qNum == 3)
    {
        if (yc - y <= (yc - i))
        {
            SetPixel(hdc, xc - x, yc - y, color);//quad two
        }
        if (yc - x <= (yc - i))
        {
            SetPixel(hdc, xc - y, yc - x, color);//quad two
        }
    }
    else if (qNum == 4)
    {
        if (xc + y >= (xc + i))
        {
            SetPixel(hdc, xc + y, yc + x, color);//quad four

        }
        if (xc + x >= (xc + i))
        {
            SetPixel(hdc, xc + x, yc + y, color);//quad four

        }

    }

}
int i = 0;
void DrawQuarterCircle(HDC hdc, int xc, int yc, int r, COLORREF color, int qNum, int i)
{
    int x = 0;
    double y = r;

    int index = 0;
    DrawPointsQuad(hdc, xc, yc, 0, r, color, qNum, r, i);
    while (x < y)
    {
        x++;
        y = sqrt((double)r * r - x * x);
        if (qNum == 4)
        {
            DrawPointsQuad(hdc, xc - i, yc, x, Round(y), color, qNum, r, i);
        }
        else if (qNum == 3)
        {
            DrawPointsQuad(hdc, xc, yc + i, x, Round(y), color, qNum, r, i);
        }
        else if (qNum == 2)
        {
            DrawPointsQuad(hdc, xc, yc - i, x, Round(y), color, qNum, r, i);
        }
        else  if (qNum == 1)
        {
            DrawPointsQuad(hdc, xc - i, yc, x, Round(y), color, qNum, r, i);
        }

        index++;

    }
}
void fillWithCircles(HDC hdc, int qNum, int xc, int yc, int rr, COLORREF color)
{
    while (i < rr)
    {
        i++;
        DrawQuarterCircle(hdc, xc, yc, rr, color, qNum, i);
    }
}
// end circle with other circles

// flood fill algorithms
void floodFillRecursive(HDC hdc, int x, int y, COLORREF bc, COLORREF fc)
{
    int c = GetPixel(hdc, x, y);
    if (c == bc || c == fc)
        return;

    SetPixel(hdc, x, y, fc);

    floodFillRecursive(hdc, x + 1, y, bc, fc);
    floodFillRecursive(hdc, x - 1, y, bc, fc);
    floodFillRecursive(hdc, x, y + 1, bc, fc);
    floodFillRecursive(hdc, x, y - 1, bc, fc);
}
void floodFill_NonRecursive(HDC hdc, int x, int y, COLORREF bc, COLORREF fc)
{
    struct point
    {
        int x, y;
    };
    stack<point> s;
    point p;
    p.x = x; p.y = y;
    s.push(p);
    while (!s.empty())
    {
        point pnt = s.top();
        s.pop();
        COLORREF c = GetPixel(hdc, pnt.x, pnt.y);
        if (c == bc || c == fc)
        {
            continue;
        }
        SetPixel(hdc, pnt.x, pnt.y, fc);
        p.x = pnt.x - 1;
        p.y = pnt.y;
        s.push(p);

        p.x = pnt.x + 1;
        p.y = pnt.y;
        s.push(p);

        p.x = pnt.x;
        p.y = pnt.y - 1;
        s.push(p);

        p.x = pnt.x;
        p.y = pnt.y + 1;
        s.push(p);

    }
}
// end of flood fill algorithms


// ConvexFill Algorithm   
#define MAXENTRIES 800
struct Edge
{
    int xleft, xright;
};
void InitEdgeTable(Edge table[])
{
    for (int i = 0; i < MAXENTRIES; i++)
    {
        table[i].xleft = INT_MAX;
        table[i].xright = -INT_MAX;
    }
}
void ScanEdge(POINT v1, POINT v2, Edge table[])
{
    if (v1.y == v2.y)
    {
        return;
    }
    if (v1.y > v2.y)
    {
        swap(v1, v2);

    }
    double minv = (double)(v2.x - v1.x) / (v2.y - v1.y);
    double x = v1.x;
    int y = v1.y;
    while (y < v2.y) {
        if (x < table[y].xleft)
        {
            table[y].xleft = (int)ceil(x);
        }
        if (x > table[y].xright)
        {
            table[y].xright = (int)floor(x);
        }
        y++;
        x += minv;
    }
}
void DrawSanLines(HDC hdc, Edge table[], COLORREF color)
{
    for (int y = 0; y < MAXENTRIES; y++)
    {
        if (table[y].xleft < table[y].xright)
        {
            for (int x = table[y].xleft; x <= table[y].xright; x++)
            {
                SetPixel(hdc, x, y, color);
            }
        }
    }

}
void FillPolygonConvex(HDC hdc, POINT p[], int n, COLORREF color)
{
    Edge* table = new Edge[MAXENTRIES];
    InitEdgeTable(table);
    POINT v1 = p[n - 1];
    for (int i = 0; i < n; i++)
    {
        POINT v2 = p[i];
        ScanEdge(v1, v2, table);
        v1 = p[i];
    }
    DrawSanLines(hdc, table, color);
    delete table;
}
// end of convexfill 

// Non ConvexFill Algorithm 
struct EdgeRec
{
    double x;
    double minv;
    int ymax;
    bool operator<(EdgeRec r)
    {
        return x < r.x;
    }
};
typedef list<EdgeRec> EdgeList;
EdgeRec InitEdgeRec(POINT& v1, POINT& v2)
{
    if (v1.y > v2.y)
    {
        swap(v1, v2);
    }
    EdgeRec rec;
    rec.x = v1.x;
    rec.ymax = v2.y;
    rec.minv = (double)(v2.x - v1.x) / (v2.y - v1.y);
    return rec;
}
void InitEdgeTable(POINT* polygon, int n, EdgeList table[])
{
    POINT v1 = polygon[n - 1];
    for (int i = 0; i < n; i++)
    {
        POINT v2 = polygon[i];
        if (v1.y == v2.y)
        {
            v1 = v2;
            continue;
        }
        EdgeRec rec = InitEdgeRec(v1, v2);
        table[v1.y].push_back(rec);
        v1 = polygon[i];
    }
}
void GeneralPolygonFill(HDC hdc, POINT* polygon, int n, COLORREF color)
{
    EdgeList* table = new EdgeList[MAXENTRIES];
    InitEdgeTable(polygon, n, table);
    int y = 0;
    while (y < MAXENTRIES && table[y].size() == 0)
    {
        y++;
    }
    if (y == MAXENTRIES)
    {
        return;
    }
    EdgeList ActiveList = table[y];
    while (ActiveList.size() > 0)
    {
        ActiveList.sort();
        for (EdgeList::iterator it = ActiveList.begin(); it != ActiveList.end(); it++)
        {
            int x1 = (int)ceil(it->x);
            it++;
            int x2 = (int)floor(it->x);
            for (int x = x1; x <= x2; x++)
            {
                SetPixel(hdc, x, y, color);
            }
        }
        y++;
        EdgeList::iterator it = ActiveList.begin();
        while (it != ActiveList.end())
        {
            if (y == it->ymax)
            {
                it = ActiveList.erase(it);
            }
            else
                it++;

        }
        for (EdgeList::iterator it = ActiveList.begin(); it != ActiveList.end(); it++)
        {
            it->x += it->minv;
        }
        ActiveList.insert(ActiveList.end(), table[y].begin(), table[y].end());
    }
    delete[] table;
}
// End non ConvexFill Algorithm

// spline Curves
struct Vector {
    double v[2];
    Vector(double x = 0, double y = 0)
    {
        v[0] = x; v[1] = y;
    }
    double& operator[](int i) {
        return v[i];
    }
    Vector& operator-(Vector  v1) {
        Vector v2;
        v2[0] = v[0] - v1[0];
        v2[1] = v[1] - v1[1];
        double res = v[2];
        return v2;
    }

};
void DrawHermiteCurve(HDC hdc, Vector& p1, Vector& T1, Vector& p2, Vector& T2, COLORREF color)
{
    double a0 = p1[0], a1 = T1[0],
        a2 = -3 * p1[0] - 2 * T1[0] + 3 * p2[0] - T2[0],
        a3 = 2 * p1[0] + T1[0] - 2 * p2[0] + T2[0];
    double b0 = p1[1], b1 = T1[1],
        b2 = -3 * p1[1] - 2 * T1[1] + 3 * p2[1] - T2[1],
        b3 = 2 * p1[1] + T1[1] - 2 * p2[1] + T2[1];
    for (double t = 0; t <= 1; t += 0.001)
    {
        double t2 = t * t, t3 = t2 * t;
        double x = a0 + a1 * t + a2 * t2 + a3 * t3;
        double y = b0 + b1 * t + b2 * t2 + b3 * t3;
        SetPixel(hdc, Round(x), Round(y), color);
    }
}
void DrawCardinalSpline(HDC hdc, Vector p[], int n, double c, COLORREF color)
{
    Vector* T = new Vector(n);
    for (int i = 1; i < n - 1; i++)
    {
        T[i].v[0] = c / 2 * (p[i + 1].v[0] - p[i - 1].v[0]);
        T[i].v[1] = c / 2 * (p[i + 1].v[1] - p[i - 1].v[1]);
    }
    T[0].v[0] = c / 2 * (p[1].v[0] - p[0].v[0]);
    T[0].v[1] = c / 2 * (p[1].v[1] - p[0].v[1]);
    T[n - 1].v[0] = c / 2 * (p[n - 1].v[0] - p[n - 2].v[0]);
    T[n - 1].v[1] = c / 2 * (p[n - 1].v[1] - p[n - 2].v[1]);
    for (int i = 0; i < n - 1; i++)
    {
        DrawHermiteCurve(hdc, p[i], T[i], p[i + 1], T[i + 1], color);
    }
}
//End of curves


// start rectangle with bezier 
static int x_left, x_right, y_top, y_bottom;
class Vector4
{
    double v[4];
public:
    Vector4(double a = 0, double b = 0, double c = 0, double d = 0)
    {
        v[0] = a; v[1] = b; v[2] = c; v[3] = d;
    }
    Vector4(double a[])
    {
        memcpy(v, a, 4 * sizeof(double));
    }
    double& operator[](int i)
    {
        return v[i];
    }
};
class Matrix4
{
    Vector4 M[4];
public:
    Matrix4(double A[])
    {
        memcpy(M, A, 16 * sizeof(double));
    }
    Vector4& operator[](int i)
    {
        return M[i];
    }
};

Vector4 operator*(Matrix4 M, Vector4& b) // right multiplication of M by b
{
    Vector4 res;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            res[i] += M[i][j] * b[j];
    return res;
}
double DotProduct(Vector4& a, Vector4& b) //multiplying a raw vector by a column vector
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
}
void DrawHermiteCurve1(HDC hdc, POINT& p1, POINT& T1, POINT& p2, POINT& T2, int numpoints, COLORREF color)
{
    double a0 = p1.x, a1 = T1.x,
        a2 = -3 * p1.x - 2 * T1.x + 3 * p2.x - T2.x,
        a3 = 2 * p1.x + T1.x - 2 * p2.x + T2.x;
    double b0 = p1.y, b1 = T1.y,
        b2 = -3 * p1.y - 2 * T1.y + 3 * p2.y - T2.y,
        b3 = 2 * p1.y + T1.y - 2 * p2.y + T2.y;
    for (double t = 0; t <= 1; t += 0.001)
    {
        double t2 = t * t, t3 = t2 * t;
        double x = a0 + a1 * t + a2 * t2 + a3 * t3;
        double y = b0 + b1 * t + b2 * t2 + b3 * t3;
        SetPixel(hdc, Round(x), Round(y), color);
    }
}
void DrawBezierCurve(HDC hdc, POINT& P0, POINT& P1, POINT& P2, POINT& P3, int numpoints, COLORREF color)
{
    POINT T0, T1;
    T0.x = 0.3 * (P1.x - P0.x);
    T0.y = 0.3 * (P1.y - P0.y);
    T1.x = 0.3 * (P3.x - P2.x);
    T1.y = 0.3 * (P3.y - P2.y);
    DrawHermiteCurve1(hdc, P0, T0, P3, T1, numpoints, color);
}
void RectangleDraw(HDC hdc, POINT p0, POINT p1, COLORREF color)
{
    SimpleDDA(hdc, p0.x, p0.y, p1.x, p0.y, color);
    SimpleDDA(hdc, p1.x, p0.y, p1.x, p1.y, color);
    SimpleDDA(hdc, p1.x, p1.y, p0.x, p1.y, color);
    SimpleDDA(hdc, p0.x, p1.y, p0.x, p0.y, color);
    x_left = min(p0.x, p1.x);
    x_right = max(p0.x, p1.x);
    y_top = max(p0.y, p1.y);
    y_bottom = min(p0.y, p1.y);
}
// end rectangle with bezier 

// square with hermite 
void HermiteCurve(HDC hdc, POINT& P0, POINT& P1, POINT& P2, POINT& P3, int numpoints, COLORREF color)
{
    POINT T0, T1;
    T0.x = 0.3 * (P1.x - P0.x);
    T0.y = 0.3 * (P1.y - P0.y);
    T1.x = 0.3 * (P3.x - P2.x);
    T1.y = 0.3 * (P3.y - P2.y);
    DrawHermiteCurve1(hdc, P0, T0, P3, T1, numpoints, color);
}
// end square with hermite

///***************************
//Start of Clipping rectangle  Point
void RecPointClipping(HDC hdc, int x, int y, int xleft, int ytop, int xright, int ybottom, COLORREF color)
{
    if (x >= xleft && x <= xright && y >= ytop && y <= ybottom)
        SetPixel(hdc, x, y, color);
}
//End of Clipping Point
///***************************
//Start of  rectangle Clipping Line
union OutCode
{
    unsigned All : 4;
    struct { unsigned left : 1, top : 1, right : 1, bottom : 1; };
};
OutCode GetOutCode(double x, double y, int xleft, int ytop, int xright, int ybottom)
{
    OutCode out;
    out.All = 0;
    if (x < xleft)out.left = 1; else if (x > xright)out.right = 1;
    if (y < ytop)out.top = 1; else if (y > ybottom)out.bottom = 1;
    return out;
}
void VIntersect(double xs, double ys, double xe, double ye, int x, double* xi, double* yi)
{
    *xi = x;
    *yi = ys + (x - xs) * (ye - ys) / (xe - xs);
}
void HIntersect(double xs, double ys, double xe, double ye, int y, double* xi, double* yi)
{
    *yi = y;
    *xi = xs + (y - ys) * (xe - xs) / (ye - ys);
}

void RecLineClipping(HDC hdc, int xs, int ys, int xe, int ye, int xleft, int ytop, int xright, int ybottom, COLORREF c)
{
    double x1 = xs, y1 = ys, x2 = xe, y2 = ye;
    OutCode out1 = GetOutCode(x1, y1, xleft, ytop, xright, ybottom);
    OutCode out2 = GetOutCode(x2, y2, xleft, ytop, xright, ybottom);

    while ((out1.All || out2.All) && !(out1.All & out2.All))
    {
        double xi, yi;
        if (out1.All)
        {
            if (out1.left)VIntersect(x1, y1, x2, y2, xleft, &xi, &yi);
            else if (out1.top)HIntersect(x1, y1, x2, y2, ytop, &xi, &yi);
            else if (out1.right)VIntersect(x1, y1, x2, y2, xright, &xi, &yi);
            else HIntersect(x1, y1, x2, y2, ybottom, &xi, &yi);
            x1 = xi;
            y1 = yi;
            out1 = GetOutCode(x1, y1, xleft, ytop, xright, ybottom);
        }
        else
        {
            if (out2.left)VIntersect(x1, y1, x2, y2, xleft, &xi, &yi);
            else if (out2.top)HIntersect(x1, y1, x2, y2, ytop, &xi, &yi);
            else if (out2.right)VIntersect(x1, y1, x2, y2, xright, &xi, &yi);
            else HIntersect(x1, y1, x2, y2, ybottom, &xi, &yi);
            x2 = xi;
            y2 = yi;
            out2 = GetOutCode(x2, y2, xleft, ytop, xright, ybottom);
        }
    }

    if (!out1.All && !out2.All)
    {
        Parametric_Line(hdc, x1, y1, x2, y2, c);
    }
}

//End of Clipping Line
///***************************
//Start of rectangle Clipping Polygon
struct Vertex
{
    double x, y;
    Vertex(int x1 = 0, int y1 = 0)
    {
        x = x1;
        y = y1;
    }
};
typedef vector<Vertex> VertexList;
typedef bool (*IsInFunc)(Vertex& v, int edge);
typedef Vertex(*IntersectFunc)(Vertex& v1, Vertex& v2, int edge);
VertexList ClipWithEdge(VertexList p, int edge, IsInFunc In, IntersectFunc Intersect)
{
    VertexList OutList;
    Vertex v1 = p[p.size() - 1];
    bool v1_in = In(v1, edge);
    for (int i = 0; i < (int)p.size(); i++)
    {
        Vertex v2 = p[i];
        bool v2_in = In(v2, edge);
        if (!v1_in && v2_in)
        {
            OutList.push_back(Intersect(v1, v2, edge));
            OutList.push_back(v2);
        }
        else if (v1_in && v2_in) OutList.push_back(v2);
        else if (v1_in) OutList.push_back(Intersect(v1, v2, edge));
        v1 = v2;
        v1_in = v2_in;
    }
    return OutList;
}
bool InLeft(Vertex& v, int edge)
{
    return v.x >= edge;
}
bool InRight(Vertex& v, int edge)
{
    return v.x <= edge;
}
bool InTop(Vertex& v, int edge)
{
    return v.y >= edge;
}
bool InBottom(Vertex& v, int edge)
{
    return v.y <= edge;
}
Vertex VIntersect(Vertex& v1, Vertex& v2, int xedge)
{
    Vertex res;
    res.x = xedge;
    res.y = v1.y + (xedge - v1.x) * (v2.y - v1.y) / (v2.x - v1.x);
    return res;
}
Vertex HIntersect(Vertex& v1, Vertex& v2, int yedge)
{
    Vertex res;
    res.y = yedge;
    res.x = v1.x + (yedge - v1.y) * (v2.x - v1.x) / (v2.y - v1.y);
    return res;
}

void PolygonClip(HDC hdc, POINT* p, int n, int xleft, int ytop, int xright, int ybottom)
{
    VertexList vlist;
    for (int i = 0; i < n; i++)vlist.push_back(Vertex(p[i].x, p[i].y));
    vlist = ClipWithEdge(vlist, xleft, InLeft, VIntersect);
    vlist = ClipWithEdge(vlist, ytop, InTop, HIntersect);
    vlist = ClipWithEdge(vlist, xright, InRight, VIntersect);
    vlist = ClipWithEdge(vlist, ybottom, InBottom, HIntersect);
    Vertex v1 = vlist[vlist.size() - 1];
    for (int i = 0; i < (int)vlist.size(); i++)
    {
        Vertex v2 = vlist[i];
        MoveToEx(hdc, Round(v1.x), Round(v1.y), NULL);
        LineTo(hdc, Round(v2.x), Round(v2.y));
        v1 = v2;
    }
}
//End of Clipping Polygon
///*********************************
/* END OF FILLING ALGO*/


//-------------------------------------------------------------------------------------------------------------------------------------

//statt clipping with square
void drawSquareClipping(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color)
{
    Parametric_Line(hdc, x1, y1, x2, y1, color);
    Parametric_Line(hdc, x1, y1, x1, y2, color);

    Parametric_Line(hdc, x2, y2, x1, y2, color);
    Parametric_Line(hdc, x2, y2, x2, y1, color);
}
void ClippingSquareLine(HDC hdc, int xs, int ys, int xe, int ye, int xleft, int ytop, int xright, int ybottom, COLORREF c)
{
    double x1 = xs, y1 = ys, x2 = xe, y2 = ye;
    OutCode out1 = GetOutCode(x1, y1, xleft, ytop, xright, ybottom);
    OutCode out2 = GetOutCode(x2, y2, xleft, ytop, xright, ybottom);

    while ((out1.All || out2.All) && !(out1.All & out2.All))
    {
        double xi, yi;
        if (out1.All)
        {
            if (out1.left)VIntersect(x1, y1, x2, y2, xleft, &xi, &yi);
            else if (out1.top)HIntersect(x1, y1, x2, y2, ytop, &xi, &yi);
            else if (out1.right)VIntersect(x1, y1, x2, y2, xright, &xi, &yi);
            else HIntersect(x1, y1, x2, y2, ybottom, &xi, &yi);
            x1 = xi;
            y1 = yi;
            out1 = GetOutCode(x1, y1, xleft, ytop, xright, ybottom);
        }
        else
        {
            if (out2.left)VIntersect(x1, y1, x2, y2, xleft, &xi, &yi);
            else if (out2.top)HIntersect(x1, y1, x2, y2, ytop, &xi, &yi);
            else if (out2.right)VIntersect(x1, y1, x2, y2, xright, &xi, &yi);
            else HIntersect(x1, y1, x2, y2, ybottom, &xi, &yi);
            x2 = xi;
            y2 = yi;
            out2 = GetOutCode(x2, y2, xleft, ytop, xright, ybottom);
        }
    }

    if (!out1.All && !out2.All)
    {
        Parametric_Line(hdc, x1, y1, x2, y2, c);
    }
}
void ClippingSquarePoint(HDC hdc, int x, int y, int xl, int yt, int xr, int yb, COLORREF color)
{
    if (x >= xl && x <= x && y >= yt && y <= yb)
    {
        SetPixel(hdc, x, y, color);
    }
}
//end clipping with square


// start clipping with circles
void ClippingCirclePoint(HDC hdc, int X, int Y, int xc, int yc, int x, int y, COLORREF color)
{
    double radius = sqrt((xc - x) * (xc - x) + (yc - y) * (yc - y));
    double dist_cent = sqrt((xc - X) * (xc - X) + (yc - Y) * (yc - Y));
    if (dist_cent <= radius)
        SetPixel(hdc, X, Y, color);
}
void ClippingCircleLine(HDC hdc, int xs, int ys, int xe, int ye, int xc, int yc, int x, int y, COLORREF color)
{
    double radius = sqrt((xc - x) * (xc - x) + (yc - y) * (yc - y));
    int dx = xe - xs;
    int dy = ye - ys;
    double slope = (double)dy / dx;
    if (abs(dy) < abs(dx)) {
        if (xs > xe) {
            swap(xs, xe);
            swap(ys, ye);
        }
        double x = xs;
        double y = ys;
        double dist_cent = sqrt((xc - x) * (xc - x) + (yc - y) * (yc - y));
        if (dist_cent <= radius)
            SetPixel(hdc, x, y, color);
        while (x < xe) {
            x = x + 1;
            y = y + slope;
            int Y = round(y);
            double dist_cent = sqrt((xc - x) * (xc - x) + (yc - Y) * (yc - Y));
            if (dist_cent <= radius)
                SetPixel(hdc, x, Y, color);
        }
    }
    else {
        int inv_slope = 1;
        if (dy != 0)
            inv_slope = dx / dy;
        if (ys > ye) {
            swap(xs, xe);
            swap(ys, ye);
        }
        double x = xs;
        double y = ys;
        double dist_cent = sqrt((xc - x) * (xc - x) + (yc - y) * (yc - y));
        if (dist_cent <= radius)
            SetPixel(hdc, x, y, color);
        while (y < ye) {
            y = y + 1;
            x = x + 1 / slope;
            int Y = round(y);
            double dist_cent = sqrt((xc - x) * (xc - x) + (yc - Y) * (yc - Y));
            if (dist_cent <= radius)
                SetPixel(hdc, x, Y, color);
        }
    }

}
// end clipping with circles

int choice = 0;
int qNum = 0;

int counter = 0;
static int index = 0;
POINT points[6];
int R = 50;
int y_2 = 0;
POINT P[100];
POINT* tmpC;
int NumberOfPoints = 0;
LRESULT CALLBACK MyWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static  vector<Vector> p;
    static  vector<Vector> fpq;  // for filling circles with quarter
    static  Vector obj;

    HDC hdc = GetDC(hwnd);
    switch (message)                  /* handle the messages */
    {
    case WM_LBUTTONDOWN:
    {
        switch (choice)
        {



        case 17:
            tmpC = new POINT[5];
            for (int i = 0; i < 5; i++)
            {
                tmpC[i].x = p[i].v[0];
                tmpC[i].y = p[i].v[1];
            }
            FillPolygonConvex(hdc, tmpC, 5, color);
            choice = 0;
            p.clear();
            break;
        case 18:
            GeneralPolygonFill(hdc, P, NumberOfPoints, color);
            choice = 0;
            p.clear();
            break;
        }
    }
    break;
    case WM_RBUTTONDOWN:

        obj.v[0] = LOWORD(lParam);
        obj.v[1] = HIWORD(lParam);

        switch (choice)
        {
        case 1: // DDA
            p.push_back(obj);
            if (p.size() == 2) {
                SimpleDDA(hdc, p[0].v[0], p[0].v[1], p[1].v[0], p[1].v[1], color);
                choice = 0;
                p.clear();
                cout << "Line DDA \n";
            }

            break;
        case 2: // midpoint line
            p.push_back(obj);
            if (p.size() == 2) {
                DrawLineMidPoint(hdc, p[0].v[0], p[0].v[1], p[1].v[0], p[1].v[1], color);
                choice = 0;
                p.clear();
                cout << "midpoint line \n";
            }
            break;
        case 3: // parametric line
            p.push_back(obj);
            if (p.size() == 2) {
                Parametric_Line(hdc, p[0].v[0], p[0].v[1], p[1].v[0], p[1].v[1], color);
                choice = 0;
                p.clear();
                cout << "Parametric line \n";
            }
            break;
        case 4:// Direct circle
            p.push_back(obj);
            if (p.size() == 2) {
                fpq.push_back(p[0]);
                rrF = sqrt(pow((p[1].v[0] - p[0].v[0]), 2) + pow(p[1].v[1] - p[0].v[1], 2));
                DirectCircle(hdc, p[0].v[0], p[0].v[1], rrF, color);
                choice = 0;
                p.clear();
                cout << "Direct circle \n";
            }
            break;

        case 5: //polar circle
            p.push_back(obj);
            if (p.size() == 2) {
                fpq.push_back(p[0]); // center of circle
                rrF = sqrt(pow((p[1].v[0] - p[0].v[0]), 2) + pow(p[1].v[1] - p[0].v[1], 2));
                PolarCircle(hdc, p[0].v[0], p[0].v[1], rrF, color);
                choice = 0;
                p.clear();
                cout << "Polar circle \n";

            }
            break;
        case 6: // iterative polar circle
            p.push_back(obj);
            if (p.size() == 2) {
                fpq.push_back(p[0]);
                rrF = sqrt(pow((p[1].v[0] - p[0].v[0]), 2) + pow(p[1].v[1] - p[0].v[1], 2));
                IterativePolarCircle(hdc, p[0].v[0], p[0].v[1], rrF, color);
                choice = 0;
                p.clear();
                cout << "iterative polar  circle \n";

            }
            break;
        case 7: // Bresenham Circle
            p.push_back(obj);
            if (p.size() == 2) {
                fpq.push_back(p[0]);
                rrF = sqrt(pow((p[1].v[0] - p[0].v[0]), 2) + pow(p[1].v[1] - p[0].v[1], 2));
                BresenhamCircle(hdc, p[0].v[0], p[0].v[1], rrF, color);
                choice = 0;
                p.clear();
                cout << "Bresenham Circle \n";

            }
            break;
        case 8: // Faster Bresenham Circle
            p.push_back(obj);
            if (p.size() == 2) {
                fpq.push_back(p[0]);
                rrF = sqrt(pow((p[1].v[0] - p[0].v[0]), 2) + pow(p[1].v[1] - p[0].v[1], 2));
                FasterBresenhamCircle(hdc, p[0].v[0], p[0].v[1], rrF, color);
                choice = 0;
                p.clear();
                cout << "Faster Bresenham Circle \n";
            }
            break;
        case 9: // Direct Ellipse
            p.push_back(obj);
            if (p.size() == 2) {
                int A_E = fabs(p[0].v[0] - p[1].v[0]);
                int R_E = fabs(p[0].v[1] - p[1].v[1]);
                DirectEllipse(hdc, p[1].v[0], p[0].v[1], A_E, R_E, color);
                choice = 0;
                p.clear();
                cout << "Direct Ellipse \n";

            }

            break;
        case 10: // polar ellipse
            p.push_back(obj);
            if (p.size() == 2) {
                int A_E = fabs(p[0].v[0] - p[1].v[0]);
                int R_E = fabs(p[0].v[1] - p[1].v[1]);
                PolarEllipse(hdc, p[1].v[0], p[0].v[1], A_E, R_E, color);
                choice = 0;
                p.clear();
                cout << "polar Ellipse \n";
            }
            break;
        case 11: // midpoint ellipse
            p.push_back(obj);
            if (p.size() == 2) {
                int A_E = fabs(p[0].v[0] - p[1].v[0]);
                int R_E = fabs(p[0].v[1] - p[1].v[1]);
                MidpointEllipse(hdc, p[1].v[0], p[0].v[1], A_E, R_E, color);
                choice = 0;
                p.clear();
                cout << "midpoint Ellipse \n";

            }
            break;
        case 12: // cardinal splines
            p.push_back(obj);
            if (p.size() == 4)
            {
                Vector* tmp = new Vector[4];
                for (int i = 0; i < 4; i++)
                {
                    tmp[i].v[0] = p[i].v[0];
                    tmp[i].v[1] = p[i].v[1];
                }
                DrawCardinalSpline(hdc, tmp, p.size(), 1, color);
                choice = 0;
                p.clear();
                cout << "Cardinal Ellipse \n";

            }
            break;
        case 13: // filling circle with lines 
            p.push_back(obj); // quarter point
            if (p.size() == 1 && fpq.size() == 1)
            {
                qNum = quarterNum(hdc, p[0].v[0], p[0].v[1], rrF, fpq[0].v[0], fpq[0].v[1]);
                filling(hdc, fpq[0].v[0], fpq[0].v[1], rrF, qNum);
                choice = 0;
                p.clear();
                fpq.clear();
                cout << "filling circle with lines  \n";

            }
            choice = 0;
            break;
        case 14: // filling circle with circles 
            p.push_back(obj); // quarter point
            if (fpq.size() == 1)
            {
                qNum = quarterNum(hdc, p[0].v[0], p[0].v[1], rrF, fpq[0].v[0], fpq[0].v[1]);
                fillWithCircles(hdc, qNum, fpq[0].v[0], fpq[0].v[1], rrF, color);

                choice = 0;
                p.clear();
                fpq.clear();
                cout << "filling circle with other circles  \n";
            }
            break;
        case 15: // square with hermite
            points[counter].x = LOWORD(lParam);
            points[counter].y = HIWORD(lParam);
            counter++;
            if (counter == 2)
            {
                drawSquareClipping(hdc, points[0].x, points[0].y, points[1].x, points[1].y, color);
                POINT tmp1, tmp2, tmp3, tmp4;
                tmp1.x = points[0].x + 10;
                tmp1.y = points[0].y + 10;

                tmp2.x = points[1].x - 10;
                tmp2.y = points[1].y + 10;

                tmp3.x = points[2].x - 10;
                tmp3.y = points[2].y - 10;

                tmp4.x = points[3].x + 10;
                tmp4.y = points[3].y - 10;
                for (int i = points[0].y; i < points[3].y; i++)
                {
                    HermiteCurve(hdc, points[3], tmp4, tmp1, points[0], 4, color);
                    HermiteCurve(hdc, points[2], tmp3, tmp2, points[1], 4, color);
                    tmp1.x += 32;
                    tmp1.y -= 32;

                    tmp2.x -= 22;
                    tmp2.y -= 22;

                    tmp3.x -= 32;
                    tmp3.y += 32;

                    tmp4.x += 22;
                    tmp4.y += 22;

                    points[0].y += 1;
                    points[1].y += 1;
                    points[2].y -= 1;
                    points[3].y -= 1;
                }
            }
            break;
        case 16: // Rectangle with Bezier
            points[counter].x = LOWORD(lParam);
            points[counter].y = HIWORD(lParam);
            counter++;
            if (counter == 2)
            {
                RectangleDraw(hdc, points[0], points[1], color);
                POINT p1, p2, p3, p4;
                p1.x = min(points[0].x, points[1].x);
                p1.y = min(points[0].y, points[1].y);

                p2.x = max(points[0].x, points[1].x);
                p2.y = min(points[0].y, points[1].y);

                p3.x = max(points[0].x, points[1].x);
                p3.y = max(points[0].y, points[1].y);

                p4.x = min(points[0].x, points[1].x);
                p4.y = max(points[0].y, points[1].y);
                POINT tmp1, tmp2, tmp3, tmp4;
                tmp1.x = p1.x + 10;
                tmp1.y = p1.y + 10;

                tmp2.x = p2.x - 10;
                tmp2.y = p2.y + 10;

                tmp3.x = p3.x - 10;
                tmp3.y = p3.y - 10;

                tmp4.x = p4.x + 10;
                tmp4.y = p4.y - 10;
                for (int i = p1.y; i < p4.y; i++)
                {
                    DrawBezierCurve(hdc, p1, tmp1, tmp2, p2, 4, color);
                    DrawBezierCurve(hdc, p4, tmp4, tmp3, p3, 4, color);
                    tmp1.x += 10;
                    tmp1.y += 10;

                    tmp2.x -= 10;
                    tmp2.y += 10;

                    tmp3.x -= 10;
                    tmp3.y -= 10;

                    tmp4.x += 10;
                    tmp4.y -= 10;

                    p1.y += 1;
                    p2.y += 1;
                    p3.y -= 1;
                    p4.y -= 1;

                }
                counter = 0;
                cout << " Rectangle with Bezier \n";
            }
            break;
        case 17: //convex
            p.push_back(obj);
            if (p.size() == 5) {
                POINT* tmp = new POINT[5];
                for (int i = 0; i < 5; i++)
                {
                    tmp[i].x = p[i].v[0];
                    tmp[i].y = p[i].v[1];
                }
                FillPolygonConvex(hdc, tmp, 5, color);
                choice = 0;
                p.clear();
                cout << " Convex Filling \n";

            }
            break;
        case 18: //Non convex
            p.push_back(obj);
            P[NumberOfPoints].x = p[p.size() - 1].v[0];
            P[NumberOfPoints].y = p[p.size() - 1].v[1];
            NumberOfPoints++;
            SelectObject(hdc, GetStockObject(DC_PEN));
            SetDCPenColor(hdc, color);
            Polygon(hdc, P, NumberOfPoints);
            cout << "Non Convex Filling \n";
            p.clear();
            choice = 0;

            break;
        case 19:
            floodFillRecursive(hdc, obj.v[0], obj.v[1], color, color);
            choice = 0;
            cout << "flood Fill Recursive\n";
            p.clear();
            break;
        case 20:
            floodFill_NonRecursive(hdc, obj.v[0], obj.v[1], RGB(0, 0, 0), color);
            choice = 0;
            cout << "flood Fill Non Recursive\n";

            p.clear();
            break;
        case 21:// clipping circle line
            points[index].x = LOWORD(lParam);
            points[index].y = HIWORD(lParam);
            index++;
            if (index == 2)
            {

                R = sqrt((points[0].x - points[1].x) * (points[0].x - points[1].x) + (points[0].y - points[1].y) * (points[0].y - points[1].y));
                PolarCircle(hdc, points[0].x, points[0].y, R, color);
            }
            else if (index == 4)
            {
                ClippingCircleLine(hdc, points[2].x, points[2].y, points[3].x, points[3].y, points[0].x, points[0].y, points[1].x, points[1].y, color);
                index = 0;
                cout << "Clipping circle Line \n";

            }
            break;
        case 22: // clipping circle point
            points[index].x = LOWORD(lParam);
            points[index].y = HIWORD(lParam);
            index++;
            if (index == 2)
            {
                R = sqrt((points[0].x - points[1].x) * (points[0].x - points[1].x) + (points[0].y - points[1].y) * (points[0].y - points[1].y));
                PolarCircle(hdc, points[0].x, points[0].y, R, color);
            }
            else if (index == 5)
            {
                ClippingCirclePoint(hdc, points[4].x, points[4].y, points[0].x, points[0].y, points[1].x, points[1].y, color);
                index = 0;
                cout << "Clipping circle Point \n";
            }
            break;
        case 23:  //Clipping Square Point
            points[index].x = LOWORD(lParam);
            points[index].y = HIWORD(lParam);

            index++;

            if (index == 2)
            {
                y_2 = (points[1].x - points[0].x) + points[0].y;
                drawSquareClipping(hdc, points[0].x, points[0].y, points[1].x, y_2, color);
            }
            else if (index == 5) {
                ClippingSquarePoint(hdc, points[4].x, points[4].y, points[0].x, points[0].y, points[1].x, y_2, color);
                index = 0;
                cout << "Clipping Square Point \n";

            }
            break;
        case 24: // Clipping Square Line   

            points[index].x = LOWORD(lParam);
            points[index].y = HIWORD(lParam);
            index++;

            if (index == 2)
            {

                y_2 = (points[1].x - points[0].x) + points[0].y;
                drawSquareClipping(hdc, points[0].x, points[0].y, points[1].x, y_2, color);

            }

            else if (index == 4)
            {

                ClippingSquareLine(hdc, points[2].x, points[2].y, points[3].x, points[3].y, points[0].x, points[0].y, points[1].x, y_2, color);
                index = 0;
                cout << "Clipping Square Line \n";

            }
            break;
        case 25: // clipping rectangle point
            points[index].x = LOWORD(lParam);
            points[index].y = HIWORD(lParam);
            index++;

            if (index == 2)
            {

                RectangleDraw(hdc, points[0], points[1], color);
            }
            else if (index == 3)
            {
                //RecPointClipping(hdc, points[2].x, points[2].y, x_left, y_top, x_right, y_bottom, color);
                RecPointClipping(hdc, points[2].x, points[2].y, points[0].x, points[0].y, points[1].x, points[1].y, color);
                index = 0;
            }
            break;
        case 26:
            points[index].x = LOWORD(lParam);
            points[index].y = HIWORD(lParam);
            index++;

            if (index == 2)
            {

                RectangleDraw(hdc, points[0], points[1], color);
            }
            else if (index == 4)
            {

                RecLineClipping(hdc, points[2].x, points[2].y, points[3].x, points[3].y, points[0].x, points[0].y, points[1].x, points[1].y, color);
                index = 0;


            }
            break;
        case 27:
            points[index].x = LOWORD(lParam);
            points[index].y = HIWORD(lParam);
            index++;

            if (index == 2)
            {

                RectangleDraw(hdc, points[0], points[1], color);
            }
            else if (index == 6)
            {
                POINT p[4] = { points[2] , points[3], points[4], points[5] };
                PolygonClip(hdc, p, 4, points[0].x, points[0].y, points[1].x, points[1].y);
                index = 0;
            }
            break;

        }
        break;
    case WM_COMMAND:
        switch (wParam)
        {
        case FILE_MENU_CLEAR:
            InvalidateRect(hwnd, NULL, TRUE);
            cout << "clear screen \n";
            break;
        case FILE_MENU_SAVE:
            save(hwnd);
            cout << "save\n";
            break;
        case FILE_MENU_LOAD:
            load(hwnd, hdc);
            cout << "load \n";
            break;
        case FILE_MENU_EXIT:
            DestroyWindow(hwnd);
            cout << "bye bye !\n";
            break;
        case  LINE_MENU_DDA:
            choice = 1;
            break;
        case LINE_MENU_MIDPOINT:
            choice = 2;
            break;
        case LINE_MENU_PARAMETRIC:
            choice = 3;
            break;
        case CIRCLE_MENU_DIRECT:
            choice = 4;
            break;
        case CIRCLE_MENU_POLAR:
            choice = 5;
            break;
        case CIRCLE_MENU_ITERATIVE:
            choice = 6;
            break;
        case CIRCLE_MENU_BRESENHAM:
            choice = 7;
            break;
        case CIRCLE_MENU_FASTER_BRESENHAM:
            choice = 8;
            break;
        case ELLIPSE_MENU_DIRECT:
            choice = 9;
            break;
        case ELLIPSE_MENU_POLAR:
            choice = 10;
            break;
        case ELLIPSE_MENU_MIDPOINT:
            choice = 11;
            break;
        case CURVE_MENU_SPLINE:
            choice = 12;
            break;
        case FILLING_MENU_CIR_LINE:
            choice = 13;
            break;
        case FILLING_MENU_CIR_CIR:
            choice = 14;
            break;
        case FILLING_MENU_SQU_HERMIT:
            choice = 15;
            break;
        case FILLING_MENU_REC_BEZIR:
            choice = 16;
            break;
        case FILLING_MENU_CONVEX:
            choice = 17;
            break;
        case FILLING_MENU_NONCONVEX:
            choice = 18;
            break;
        case FILLING_MENU_RECURS:
            choice = 19;
            break;
        case FILLING_MENU_NONRECURS:
            choice = 20;
            break;
        case Clipping_MENU_CIRCLELINE:
            choice = 21;
            break;
        case Clipping_MENU_CIRCLEPOINT:
            choice = 22;
            break;
        case Clipping_MENU_SQUAREPOINT:
            choice = 23;
            break;
        case Clipping_MENU_SQUARELINE:
            choice = 24;
            break;
        case Clipping_MENU_RECTANGLEPOINT:
            choice = 25;
            break;
        case Clipping_MENU_RECTANGLELINE:
            choice = 26;
            break;
        case Clipping_MENU_RECTANGLEPOLYGON:
            choice = 27;
            break;


        case SHAPE_COLOR_MENU_BLACK:
            color = RGB(0, 0, 0);
            break;
        case SHAPE_COLOR_MENU_RED:
            color = RGB(255, 0, 0);
            break;
        case SHAPE_COLOR_MENU_GREEN:
            color = RGB(0, 255, 0);
            break;
        case SHAPE_COLOR_MENU_BLUE:
            color = RGB(0, 0, 255);
            break;
        }
        break;
    case WM_CREATE:
        addMenus(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
        break;
    default:                      /* for messages that we don't deal with */
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

int APIENTRY WinMain(HINSTANCE hinst, HINSTANCE pinst, LPSTR cmd, int nsh)
{
    WNDCLASS wc;
    wc.cbClsExtra = wc.cbWndExtra = 0;
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hInstance = hinst;
    wc.lpfnWndProc = MyWndProc;
    wc.lpszClassName = L"MyClass";
    wc.lpszMenuName = NULL;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hCursor = LoadCursor(NULL, IDC_HAND);
    RegisterClass(&wc);
    CreateConsole();
    HWND hwnd = CreateWindow(L"MyClass", L"My First Window", WS_OVERLAPPEDWINDOW, 0, 0, 1200, 720, NULL, NULL, hinst, 0);
    ShowWindow(hwnd, nsh);
    UpdateWindow(hwnd);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
void addMenus(HWND hwnd)
{
    hmenu = CreateMenu();
    HMENU hFileMenu = CreateMenu();
    HMENU hLineMenu = CreateMenu();
    HMENU hCircleMenu = CreateMenu();
    HMENU hEllipseMenu = CreateMenu();
    HMENU hCurveMenu = CreateMenu();
    HMENU hFillingQuarterMenu = CreateMenu();
    HMENU hFillingMenu = CreateMenu();
    HMENU hColorMenu = CreateMenu();
    HMENU hClippingMenu = CreateMenu();

    AppendMenu(hmenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");
    AppendMenu(hFileMenu, MF_STRING, FILE_MENU_CLEAR, L"Clear Screen");
    AppendMenu(hFileMenu, MF_STRING, FILE_MENU_SAVE, L"Save");
    AppendMenu(hFileMenu, MF_STRING, FILE_MENU_LOAD, L"Load");
    AppendMenu(hFileMenu, MF_SEPARATOR, NULL, NULL);
    AppendMenu(hFileMenu, MF_STRING, FILE_MENU_EXIT, L"Exit");

    AppendMenu(hmenu, MF_POPUP, (UINT_PTR)hLineMenu, L"Line");
    AppendMenu(hLineMenu, MF_STRING, LINE_MENU_DDA, L"DDA");
    AppendMenu(hLineMenu, MF_STRING, LINE_MENU_MIDPOINT, L"MidPoint");
    AppendMenu(hLineMenu, MF_STRING, LINE_MENU_PARAMETRIC, L"Parametric");


    AppendMenu(hmenu, MF_POPUP, (UINT_PTR)hCircleMenu, L"Circle");
    AppendMenu(hCircleMenu, MF_STRING, CIRCLE_MENU_DIRECT, L"Direct");
    AppendMenu(hCircleMenu, MF_STRING, CIRCLE_MENU_POLAR, L"Polar");
    AppendMenu(hCircleMenu, MF_STRING, CIRCLE_MENU_ITERATIVE, L"Iterative");
    AppendMenu(hCircleMenu, MF_STRING, CIRCLE_MENU_BRESENHAM, L"Bresenham");
    AppendMenu(hCircleMenu, MF_STRING, CIRCLE_MENU_FASTER_BRESENHAM, L"Faster Bresenham");

    AppendMenu(hmenu, MF_POPUP, (UINT_PTR)hEllipseMenu, L"Ellipse");
    AppendMenu(hEllipseMenu, MF_STRING, ELLIPSE_MENU_DIRECT, L"Direct");
    AppendMenu(hEllipseMenu, MF_STRING, ELLIPSE_MENU_POLAR, L"Polar");
    AppendMenu(hEllipseMenu, MF_STRING, ELLIPSE_MENU_MIDPOINT, L"Midpoint");

    AppendMenu(hmenu, MF_POPUP, (UINT_PTR)hCurveMenu, L"Curve");
    AppendMenu(hCurveMenu, MF_STRING, CURVE_MENU_SPLINE, L"Spline curve");

    AppendMenu(hmenu, MF_POPUP, (UINT_PTR)hFillingQuarterMenu, L"Filling Quarter");
    AppendMenu(hFillingQuarterMenu, MF_STRING, FILLING_MENU_CIR_LINE, L"With lines");
    AppendMenu(hFillingQuarterMenu, MF_STRING, FILLING_MENU_CIR_CIR, L"With cicrcles");

    AppendMenu(hmenu, MF_POPUP, (UINT_PTR)hFillingMenu, L"Filling");
    AppendMenu(hFillingMenu, MF_STRING, FILLING_MENU_SQU_HERMIT, L"Square with hermit[vertical]");
    AppendMenu(hFillingMenu, MF_STRING, FILLING_MENU_REC_BEZIR, L"Rectangle with bezier[horizontal]");
    AppendMenu(hFillingMenu, MF_STRING, FILLING_MENU_CONVEX, L"Convex");
    AppendMenu(hFillingMenu, MF_STRING, FILLING_MENU_NONCONVEX, L"NonConvex");
    AppendMenu(hFillingMenu, MF_STRING, FILLING_MENU_RECURS, L"Recursive FloodFill");
    AppendMenu(hFillingMenu, MF_STRING, FILLING_MENU_NONRECURS, L"NonRecursive FloodFill");

    AppendMenu(hmenu, MF_POPUP, (UINT_PTR)hColorMenu, L"Shape's Color");
    AppendMenu(hColorMenu, MF_STRING, SHAPE_COLOR_MENU_BLACK, L"Black");
    AppendMenu(hColorMenu, MF_STRING, SHAPE_COLOR_MENU_RED, L"Red");
    AppendMenu(hColorMenu, MF_STRING, SHAPE_COLOR_MENU_GREEN, L"Green");
    AppendMenu(hColorMenu, MF_STRING, SHAPE_COLOR_MENU_BLUE, L"Blue");

    AppendMenu(hmenu, MF_POPUP, (UINT_PTR)hClippingMenu, L"Clipping ");
    AppendMenu(hClippingMenu, MF_STRING, Clipping_MENU_RECTANGLEPOINT, L"Rectangle Clipping With Point");
    AppendMenu(hClippingMenu, MF_STRING, Clipping_MENU_RECTANGLELINE, L"Rectangle Clipping With Line");
    AppendMenu(hClippingMenu, MF_STRING, Clipping_MENU_RECTANGLEPOLYGON, L"Rectangle Clipping With Polygon");
    AppendMenu(hClippingMenu, MF_STRING, Clipping_MENU_SQUAREPOINT, L"Square Clipping With Point");
    AppendMenu(hClippingMenu, MF_STRING, Clipping_MENU_SQUARELINE, L"Square Clipping With Line");
    AppendMenu(hClippingMenu, MF_STRING, Clipping_MENU_CIRCLEPOINT, L"Circle Clipping With Point");
    AppendMenu(hClippingMenu, MF_STRING, Clipping_MENU_CIRCLELINE, L"Circle Clipping With Line");

    SetMenu(hwnd, hmenu);
}
