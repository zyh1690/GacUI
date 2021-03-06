/*******************************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
GacUI::GDI Helper Library

Classes:
  WinRegion				：图形区域
  WinTransform			：坐标变换
  WinBrush				：图形画刷
  WinPen				：图形画笔
  WinFont				：字体
  WinDC					：设备上下文
  WinControlDC			：控件DC
  WinImageDC			：图形DC
  WinProxyDC			：代理DC
  WinDIB				：设备无关位图
  WinMetaFileBuilder	：图形元文件编辑器
  WinMetaFile			：图形元文件
  WinBitmap				：位图

Comments:
  WinDC::PolyDraw
    Points				：点数组
	Actions				：一个说明点数组中点的作用的数组，可以是如下内容：
		PT_MOVETO		：消耗1个点执行MoveTo
		PT_LINETO		：消耗1个点执行LineTo
		PT_BEZIERTO		：消耗3个点完成Bezier曲线
		其中后两个值可以跟PT_CLOSEFIGURE进行or混合，以便让这个点跟最后一个PT_MOVETO或MoveTo的点连线组成封闭曲线
	PointCount			：点数组中点的数量
  WinDC::DrawBuffer
    Format: 参见DrawText函数
  Pen：
	Style：
		PS_SOLID、PS_DASH、PS_DOT、PS_DASHDOT、PS_DASHDOTDOT、PS_USERSTYLE(Geometric画笔有效)
	EndCap：
		PS_ENDCAP_ROUND、PS_ENDCAP_SQUARE、PS_ENDCAP_FLAT
	Join：
		PS_JOIN_BEVEL、PS_JOIN_MITER、PS_JOIN_ROUND
  Brush：
	Hatch：
		HS_BDIAGONAL、HS_CROSS、HS_DIAGCROSS、HS_FDIAGONAL、HS_HORIZONTAL、HS_VERTICAL
  Region：
	Combine：
		RGN_AND、RGN_OR、RGN_XOR、RGN_DIFF、RGN_COPY
  ImageCopy：
	Draw ROP：
		BLACKNESS、DSTINVERT、MERGECOPY、MERGEPAINT、NOTSRCCOPY、NOTSRCERASE、
		PATCOPY、PATINVERT、PATPAINT、SRCAND、SRCCOPY、SRCERASE、SRCINVERT、SRCPAINT、WHITENESS
  WinDIB：
	TransformAlphaChannel()：	用于在修改非alpha图片的alpha通道后转换为alpha图片
	Generate×××()：			用于自动生成非alpha图片的alpha通道值
	被WinDC的Draw绘制的时候，alpha图片和非alpha图片会有不同的效果
*******************************************************************************/

#ifndef VCZH_PRESENTATION_WINDOWS_GDI_WINGDI
#define VCZH_PRESENTATION_WINDOWS_GDI_WINGDI

#include "..\..\..\..\Import\Vlpp.h"
#include<windows.h>

namespace vl
{
	namespace presentation
	{
		namespace windows
		{

/*********************************************************************************************************
图形
*********************************************************************************************************/

			class WinRegion : public Object
			{
			public:
				typedef Ptr<WinRegion>	Ptr;

				friend bool IsEqual(WinRegion::Ptr Region1, WinRegion::Ptr Region2);
			protected:
				HRGN					FHandle;
			public:
				WinRegion(vint Left, vint Top, vint Right, vint Bottom, bool Rectangle);
				WinRegion(RECT Rect, bool Rectangle);
				WinRegion(vint Left, vint Top, vint Right, vint Bottom, vint EllipseWidth, vint EllipseHeight);
				WinRegion(POINT* Points, vint Count, bool Alternate);
				WinRegion(WinRegion::Ptr Region);
				WinRegion(WinRegion::Ptr Region1, WinRegion::Ptr Region2, vint CombineMode);
				WinRegion(HRGN RegionHandle);
				~WinRegion();

				HRGN					GetHandle();
				bool					ContainPoint(POINT Point);
				bool					ContainRect(RECT Rect);
				RECT					GetBoundRect();
				void					Move(vint OffsetX, vint OffsetY);
			};

			class WinTransform : public Object
			{
			protected:
				XFORM					FTransform;
			public:
				WinTransform(XFORM Transform);
				WinTransform(const WinTransform& Transform);

				WinTransform&			operator=(const WinTransform& Transform);
				WinTransform			operator*(const WinTransform& Transform);
				const XFORM*			GetHandle()const;

				static WinTransform		Translate(float OffsetX, float OffsetY);
				static WinTransform		Scale(float ScaleX, float ScaleY);
				static WinTransform		Rotate(float Angle);
				static WinTransform		Rotate(float Cos, float Sin);
				static WinTransform		ReflectX();
				static WinTransform		ReflectY();
				static WinTransform		Reflect(float VectorX, float VectorY);
				static WinTransform		Reflect(float OriginX, float OriginY, float VectorX, float VectorY);
				static WinTransform		AxisV(float Xx, float Xy, float Yx, float Yy);
				static WinTransform		AxisA(float AngleX, float LenX, float AngleY, float LenY);
			};
		
/*********************************************************************************************************
图像
*********************************************************************************************************/

			class WinDC;
			class WinControlDC;
			class WinProxyDC;
			class WinImageDC;

			class WinMetaFileBuilder : public Object
			{
				friend class WinMetaFile;
			protected:
				vint						FWidth;
				vint						FHeight;
				WinProxyDC*				FDC;

				void					Create(vint Width, vint Height);
				void					Draw(HENHMETAFILE Handle);
				void					Destroy();
			public:

				WinMetaFileBuilder(vint Width, vint Height);
				~WinMetaFileBuilder();

				void					LoadFrom(WinMetaFile* File);
				void					SaveTo(WinMetaFile* File);
				void					LoadFrom(WString FileName);
				void					SaveTo(WString FileName);
				WinDC*					GetWinDC();
				vint						GetWidth();
				vint						GetHeight();
			};

			class WinMetaFile : public Object
			{
				friend class WinMetaFileBuilder;
			protected:
				HENHMETAFILE			FHandle;
				vint						FWidth;
				vint						FHeight;
			public:
				WinMetaFile(WString FileName);
				WinMetaFile(WinMetaFileBuilder* Builder);
				~WinMetaFile();

				HENHMETAFILE			GetHandle();
				vint						GetWidth();
				vint						GetHeight();
			};

			class WinBitmap : public Object
			{
			public:
				typedef Ptr<WinBitmap>		Ptr;
				enum BitmapBits
				{
					vbb2Bits,
					vbb24Bits,
					vbb32Bits
				};
			protected:
				BitmapBits				FBits;
				vint						FWidth;
				vint						FHeight;
				WinImageDC*				FDC;
				HBITMAP					FHandle;
				BYTE**					FScanLines;
				bool					FAlphaChannelBuilt;

				vint						GetBitsFromBB(BitmapBits BB);
				vint						GetLineBytes(vint Width, BitmapBits BB);
				void					FillBitmapInfoHeader(vint Width, vint Height, BitmapBits Bits, BITMAPINFOHEADER* Header);
				HBITMAP					CreateDDB(vint Width, vint Height, BitmapBits Bits);
				HBITMAP					CreateDIB(vint Width, vint Height, BitmapBits Bits, BYTE**& ScanLines);
				void					Constructor(vint Width, vint Height, BitmapBits Bits, bool DIBSections);
			public:
				WinBitmap(vint Width, vint Height, BitmapBits Bits, bool DIBSections);
				WinBitmap(WString FileName, bool Use32Bits, bool DIBSections);
				~WinBitmap();

				void					SaveToFile(WString FileName);

				WinDC*					GetWinDC();
				vint						GetWidth();
				vint						GetHeight();
				vint						GetLineBytes();
				BYTE**					GetScanLines();
				HBITMAP					GetBitmap();
				BitmapBits				GetBitmapBits();
				void					FillCompatibleHeader(BITMAPINFOHEADER* Header);

				bool					CanBuildAlphaChannel();
				bool					IsAlphaChannelBuilt();
				void					BuildAlphaChannel(bool autoPremultiply);
				void					GenerateTrans(COLORREF Color);
				void					GenerateAlpha(BYTE Alpha);
				void					GenerateTransAlpha(COLORREF Color, BYTE Alpha);
				void					GenerateLuminance();
				void					GenerateGrayLevel();
				void					Generate(BYTE(*Function)(COLORREF));
			};

/*********************************************************************************************************
材料
*********************************************************************************************************/

			class WinBrush : public Object
			{
			public:
				typedef Ptr<WinBrush>		Ptr;
			protected:
				HBRUSH					FHandle;
				unsigned char*			FDIBMemory;
			public:
				WinBrush();
				WinBrush(COLORREF Color);
				WinBrush(vint Hatch, COLORREF Color);
				WinBrush(WinBitmap::Ptr Bitmap);
				~WinBrush();

				HBRUSH					GetHandle();
			};

			class WinPen : public Object
			{
			public:
				typedef Ptr<WinPen>		Ptr;
			protected:
				HPEN					FHandle;
				unsigned char*			FDIBMemory;
			public:
				WinPen(vint Style, vint Width, COLORREF Color);
				WinPen(vint Style, vint EndCap, vint Join, vint Width, COLORREF Color);
				WinPen(vint Style, vint EndCap, vint Join, vint Hatch, vint Width, COLORREF Color);
				WinPen(WinBitmap::Ptr DIB, vint Style, vint EndCap, vint Join, vint Width);
				~WinPen();

				HPEN					GetHandle();
			};

			class WinFont : public Object
			{
			public:
				typedef Ptr<WinFont>		Ptr;
			protected:
				LOGFONT					FFontInfo;
				HFONT					FHandle;
			public:
				WinFont(WString Name, vint Height, vint Width, vint Escapement, vint Orientation, vint Weight, bool Italic, bool Underline, bool StrikeOut, bool Antialise);
				WinFont(LOGFONT* FontInfo);
				~WinFont();

				HFONT					GetHandle();
				LOGFONT*				GetInfo();
			};

/*********************************************************************************************************
设备上下文
*********************************************************************************************************/

			extern WinBrush::Ptr		CreateDefaultBrush();
			extern WinPen::Ptr			CreateDefaultPen();
			extern WinFont::Ptr			CreateDefaultFont();

			class IWinResourceService : public Interface
			{
			public:
				virtual WinPen::Ptr		GetDefaultPen()=0;
				virtual WinBrush::Ptr	GetDefaultBrush()=0;
				virtual WinFont::Ptr	GetDefaultFont()=0;
			};
			extern IWinResourceService*	GetDefaultResourceService();
			extern void					SetDefaultResourceService(IWinResourceService* Service);

			class WinDC : public Object
			{
			protected:
				HDC						FHandle;

				WinPen::Ptr				FPen;
				WinBrush::Ptr			FBrush;
				WinFont::Ptr			FFont;

				HPEN					FOldPen;
				HBRUSH					FOldBrush;
				HFONT					FOldFont;

				void					Init();
			public:
				WinDC();
				~WinDC();

				HDC						GetHandle();

				WinPen::Ptr				GetPen();
				WinBrush::Ptr			GetBrush();
				WinFont::Ptr			GetFont();
				void					SetPen(WinPen::Ptr Pen);
				void					SetBrush(WinBrush::Ptr Brush);
				void					SetFont(WinFont::Ptr Font);
				COLORREF				GetBackColor();
				void					SetBackColor(COLORREF Color);
				COLORREF				GetTextColor();
				void					SetTextColor(COLORREF Color);
				bool					GetBackTransparent();
				void					SetBackTransparent(bool Transparent);
				POINT					GetBrushOrigin();
				void					SetBrushOrigin(POINT Point);

				void					DrawBuffer(vint X, vint Y, const wchar_t* Text, vint CharCount);
				void					DrawBuffer(vint X, vint Y, const wchar_t* Text, vint CharCount, vint TabWidth, vint TabOriginX);
				void					DrawBuffer(RECT Rect, const wchar_t* Text, vint CharCount, UINT Format);
				void					DrawString(vint X, vint Y, WString Text);
				void					DrawString(vint X, vint Y, WString Text, vint TabWidth, vint TabOriginX);
				void					DrawString(RECT Rect, WString Text, UINT Format);

				SIZE					MeasureString(WString Text, vint TabSize=-1);
				SIZE					MeasureBuffer(const wchar_t* Text, vint CharCount, vint TabSize=-1);
				SIZE					MeasureBuffer(const wchar_t* Text, vint TabSize=-1);
				SIZE					MeasureWrapLineString(WString Text, vint MaxWidth);
				SIZE					MeasureWrapLineBuffer(const wchar_t* Text, vint CharCount, vint MaxWidth);
				SIZE					MeasureWrapLineBuffer(const wchar_t* Text, vint MaxWidth);

				void					FillRegion(WinRegion::Ptr Region);
				void					FrameRegion(WinRegion::Ptr Region, vint BlockWidth, vint BlockHeight);

				void					MoveTo(vint X, vint Y);
				void					LineTo(vint X, vint Y);
				void					Rectangle(vint Left, vint Top, vint Right, vint Bottom);
				void					Rectangle(RECT Rect);
				void					FocusRectangle(vint Left, vint Top, vint Right, vint Bottom);
				void					FocusRectangle(RECT Rect);
				void					FillRect(vint Left, vint Top, vint Right, vint Bottom);
				void					FillRect(RECT Rect);
				void					Ellipse(vint Left, vint Top, vint Right, vint Bottom);
				void					Ellipse(RECT Rect);
				void					RoundRect(vint Left, vint Top, vint Right, vint Bottom, vint EllipseWidth, vint EllipseHeight);
				void					RoundRect(RECT Rect, vint EllipseWidth, vint EllipseHeight);
				void					PolyLine(const POINT* Points, vint Count);
				void					PolyLineTo(const POINT* Points, vint Count);
				void					PolyGon(const POINT* Points, vint Count);
				void					PolyBezier(const POINT* Points, vint Count);
				void					PolyBezierTo(const POINT* Points, vint Count);
				void					PolyDraw(const POINT* Points, const BYTE* Actions, vint PointCount);
				void					Arc(RECT Bound, POINT Start, POINT End);
				void					Arc(vint Left, vint Top, vint Right, vint Bottom, vint StartX, vint StartY, vint EndX, vint EndY);
				void					ArcTo(RECT Bound, POINT Start, POINT End);
				void					ArcTo(vint Left, vint Top, vint Right, vint Bottom, vint StartX, vint StartY, vint EndX, vint EndY);
				void					AngleArc(vint X, vint Y, vint Radius, float StartAngle, float SweepAngle);
				void					AngleArc(vint X, vint Y, vint Radius, double StartAngle, double SweepAngle);
				void					Chord(RECT Bound, POINT Start, POINT End);
				void					Chord(vint Left, vint Top, vint Right, vint Bottom, vint StartX, vint StartY, vint EndX, vint EndY);
				void					Pie(RECT Bound, POINT Start, POINT End);
				void					Pie(vint Left, vint Top, vint Right, vint Bottom, vint StartX, vint StartY, vint EndX, vint EndY);
				void					GradientTriangle(TRIVERTEX* Vertices, vint VerticesCount, GRADIENT_TRIANGLE* Triangles, vint TriangleCount);

				void					BeginPath();
				void					EndPath();
				void					ClosePath();
				void					WidenPath();
				void					DiscardPath();
				void					DrawPath();
				void					FillPath();
				void					DrawAndFillPath();
				WinRegion::Ptr			RegionFromPath();

				bool					PointInClip(POINT Point);
				bool					RectInClip(RECT Rect);
				void					ClipPath(vint CombineMode);
				void					ClipRegion(WinRegion::Ptr Region);
				void					RemoveClip();
				void					MoveClip(vint OffsetX, vint OffsetY);
				void					CombineClip(WinRegion::Ptr Region, vint CombineMode);
				void					IntersetClipRect(RECT Rect);
				void					ExcludeClipRect(RECT Rect);
				WinRegion::Ptr			GetClipRegion();
				RECT					GetClipBoundRect();

				WinTransform			GetTransform();
				void					SetTransform(const WinTransform& Transform);

				void					Copy(vint dstX, vint dstY, vint dstW, vint dstH, WinDC* Source, vint srcX, vint srcY, DWORD DrawROP=SRCCOPY);
				void					Copy(RECT dstRect, WinDC* Source, POINT srcPos, DWORD DrawROP=SRCCOPY);
				void					Copy(vint dstX, vint dstY, vint dstW, vint dstH, WinDC* Source, vint srcX, vint srcY ,vint srcW, vint srcH, DWORD DrawROP=SRCCOPY);
				void					Copy(RECT dstRect, WinDC* Source, RECT srcRect, DWORD DrawROP=SRCCOPY);
				void					Copy(POINT UpperLeft, POINT UpperRight, POINT LowerLeft, WinDC* Source, vint srcX, vint srcY, vint srcW, vint srcH);
				void					Copy(POINT UpperLeft, POINT UpperRight, POINT LowerLeft, WinDC* Source, RECT srcRect);
				void					CopyTrans(vint dstX, vint dstY, vint dstW, vint dstH, WinDC* Source, vint srcX, vint srcY ,vint srcW, vint srcH, COLORREF Color);
				void					CopyTrans(RECT dstRect, WinDC* Source, RECT srcRect, COLORREF Color);

				void					Draw(vint dstX, vint dstY, WinMetaFile* MetaFile);
				void					Draw(POINT Pos, WinMetaFile* MetaFile);
				void					Draw(vint dstX, vint dstY, vint dstW, vint dstH, WinMetaFile* MetaFile);
				void					Draw(RECT Rect, WinMetaFile* MetaFile);

				void					Draw(vint dstX, vint WinBitmap, WinBitmap::Ptr Bitmap);
				void					Draw(POINT Pos, WinBitmap::Ptr Bitmap);
				void					Draw(vint dstX, vint dstY, vint dstW, vint dstH, WinBitmap::Ptr Bitmap);
				void					Draw(RECT Rect, WinBitmap::Ptr Bitmap);
				void					Draw(vint dstX, vint dstY, vint dstW, vint dstH, WinBitmap::Ptr Bitmap, vint srcX, vint srcY);
				void					Draw(RECT Rect, WinBitmap::Ptr Bitmap, POINT Pos);
				void					Draw(vint dstX, vint dstY, vint dstW, vint dstH, WinBitmap::Ptr Bitmap, vint srcX, vint srcY, vint srcW, vint srcH);
				void					Draw(RECT dstRect, WinBitmap::Ptr Bitmap, RECT srcRect);

				void					Draw(vint dstX, vint dstY, WinBitmap::Ptr Bitmap, unsigned char Alpha);
				void					Draw(POINT Pos, WinBitmap::Ptr Bitmap, unsigned char Alpha);
				void					Draw(vint dstX, vint dstY, vint dstW, vint dstH, WinBitmap::Ptr Bitmap, unsigned char Alpha);
				void					Draw(RECT Rect, WinBitmap::Ptr Bitmap, unsigned char Alpha);
				void					Draw(vint dstX, vint dstY, vint dstW, vint dstH, WinBitmap::Ptr Bitmap, vint srcX, vint srcY, unsigned char Alpha);
				void					Draw(RECT Rect, WinBitmap::Ptr Bitmap, POINT Pos, unsigned char Alpha);
				void					Draw(vint dstX, vint dstY, vint dstW, vint dstH, WinBitmap::Ptr Bitmap, vint srcX, vint srcY, vint srcW, vint srcH, unsigned char Alpha);
				void					Draw(RECT dstRect, WinBitmap::Ptr Bitmap, RECT srcRect, unsigned char Alpha);
			};

			class WinControlDC : public WinDC
			{
			protected:
				HWND					FControlHandle;
			public:
				WinControlDC(HWND Handle);
				~WinControlDC();
			};

			class WinProxyDC : public WinDC
			{
			public:
				WinProxyDC();
				~WinProxyDC();

				void					Initialize(HDC Handle);
			};

			class WinImageDC : public WinDC
			{
			public:
				WinImageDC();
				~WinImageDC();
			};
		}
	}
}

#endif