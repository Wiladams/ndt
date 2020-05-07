#pragma once

#include <windows.h>
#include <array>
#include "p5.h"

//******************************************************************************
enum TextOutFormat : unsigned
{
  TOF_LEFT      = 0x00000000,
  TOF_CENTER    = 0x00000001,
  TOF_RIGHT     = 0x00000002,
  TOF_TOP       = 0x00000000,
  TOF_MIDDLE    = 0x00000010,
  TOF_BOTTOM    = 0x00000020,
  TOF_WORDBREAK = 0x00000100
};


//******************************************************************************
enum class DRAWSTYLE : unsigned
{
  Fill,
  Stroke,
  FillAndStroke,
  StrokeAndFill
};


//******************************************************************************
//
class TextBox
{
private:
  typedef struct _line_
  {
    unsigned uiBeginOffset{};
    unsigned uiLen{};
    double dBreaksExtra{};
  } LINE;

  typedef struct _params_
  {
    std::array<unsigned> arrBreaksPos;
    std::array<unsigned> arrForcedBreaksIndex;
    std::array<double> arrSpansWidth;
    std::array<LINE> arrLines;
    BLString strText;
    size_t uiCharCount{};
    unsigned uiFormat{ TOF_LEFT | TOF_TOP };
    double dFontHeight{};
    double dStringHeight{};
    double dSpaceSize{};
    double dNLSize{};
  } PARAMS;


protected:
  PARAMS* _pParams{ new PARAMS() };


public:
  TextBox()
  {
  }

  TextBox(BLString &szText)
  {
    _pParams->strText = szText;

    _parseText();
  }

  virtual ~TextBox()
  {
    delete _pParams;
    _pParams = nullptr;
  }


public:
  void draw(BLContext& ct, BLRect& rc, BLFont& fnt, const DRAWSTYLE ds = DRAWSTYLE::Fill)
  {
    if (_pParams->strText.isEmpty())
    {
      return;
    }

    _draw(ct, rc, fnt, ds);
  }

  void cdraw(BLContext& ct, BLRect& rc, BLFont& fnt, const DRAWSTYLE ds = DRAWSTYLE::Fill)
  {
    if (_pParams->strText.isEmpty())
    {
      return;
    }

    _calcSpans(fnt);
    _fillLines(rc.w);
    _draw(ct, rc, fnt, ds);
  }

  inline void calc(BLFont& fnt)
  {
    _calcSpans(fnt);
  }

  inline void layout(const double width)
  {
    _fillLines(width);
  }





protected:
  void _draw(BLContext& ct, BLRect& rc, BLFont& fnt, const DRAWSTYLE ds)
  {
    BLPoint ptPos(rc.x, rc.y + _pParams->dFontHeight);

    switch (_pParams->uiFormat & 0x000000F0)
    {
      case TOF_MIDDLE:
        ptPos.y += (rc.h - _pParams->arrLines.getItemsCount() * _pParams->dStringHeight) * 0.5;
        break;

      case TOF_BOTTOM:
        ptPos.y += rc.h - _pParams->arrLines.getItemsCount() * _pParams->dStringHeight;
        break;

      default:
        break;
    }

    for (const LINE& line : _pParams->arrLines)
    {
      if (line.uiLen == 0)  // пустая строка
      {
        ptPos.y += _pParams->dStringHeight;
        continue;
      }

      switch (_pParams->uiFormat & 0x0000000F)
      {
        case TOF_CENTER:
          ptPos.x = rc.x + line.dBreaksExtra * 0.5;
          break;

        case TOF_RIGHT:
          ptPos.x = rc.x + line.dBreaksExtra;
          break;

        default:
          break;
      }

      BLGlyphBuffer gb;
      gb.setWCharText(_pParams->strText.str() + line.uiBeginOffset, line.uiLen);
      fnt.shape(gb);
      fnt.applyKerning(gb);
      const BLGlyphRun& gr = gb.glyphRun();

      switch (ds)
      {
        case DRAWSTYLE::FillAndStroke:
          ct.fillGlyphRun(ptPos, fnt, gr);
        case DRAWSTYLE::Stroke:
          ct.strokeGlyphRun(ptPos, fnt, gr);
          break;

        case DRAWSTYLE::StrokeAndFill:
          ct.strokeGlyphRun(ptPos, fnt, gr);
        case DRAWSTYLE::Fill:
        default:
          ct.fillGlyphRun(ptPos, fnt, gr);
          break;
      }

      ptPos.y += _pParams->dStringHeight;
    }
  }

  void _parseText(void)
  {
    _pParams->arrBreaksPos.clear();
    _pParams->arrForcedBreaksIndex.clear();

    if (_pParams->strText.isEmpty())
    {
      return;
    }

    unsigned uiPos = 0;

    while ((uiPos = _pParams->strText.findFirstOf(L" \n\t", uiPos)) != FixedString::npos)
    {
      _pParams->arrBreaksPos.append(uiPos);

      if (_pParams->strText[uiPos] == L'\n')
      {
        _pParams->arrForcedBreaksIndex.append(_pParams->arrBreaksPos.getItemsCount() - 1);
      }

      uiPos += 1;
    }

    _pParams->arrForcedBreaksIndex.append(_pParams->arrBreaksPos.getItemsCount() - 1);

    _pParams->arrBreaksPos.shrinkToFit();
    _pParams->arrForcedBreaksIndex.shrinkToFit();
  }

  void _calcSpans(BLFont& fnt)
  {
    _pParams->arrSpansWidth.clear();

    const double& m00 = fnt.matrix().m00;

    for (unsigned& len : _pParams->arrBreaksPos)
    {
      BLGlyphBuffer gb;
      gb.setWCharText(_pParams->strText.str(), len);
      fnt.shape(gb);
      fnt.applyKerning(gb);

      BLGlyphPlacement* pgp = gb.placementData();
      const size_t gcnt     = gb.size();
      int width             = 0;
      for (size_t i = 0; i < gcnt; ++i)
      {
        width += pgp[i].advance.x;
      }
      _pParams->arrSpansWidth.append(width * m00);
    }

    _pParams->arrSpansWidth.shrinkToFit();

    const BLFontMatrix& fmx = fnt.matrix();
    const BLFontMetrics& fm = fnt.metrics();
    BLGlyphBuffer gb;

    //**************************************************************************
    gb.setWCharText(L" ", 1);
    fnt.shape(gb);
    _pParams->dSpaceSize = (double)gb.placementData()[0].advance.x * fmx.m00;

    //**************************************************************************
    gb.setWCharText(L"\n", 1);
    fnt.shape(gb);
    //fnt.applyKerning(gb); // ???
    _pParams->dNLSize = (double)gb.placementData()[0].advance.x * fmx.m00;

    //**************************************************************************
    _pParams->dFontHeight   = (double)fm.ascent;
    _pParams->dStringHeight = (double)fm.ascent + (double)fm.descent + (double)fm.lineGap;
  }

  void _fillLines(const double dWidth)
  {
    if ((_pParams->uiFormat & TOF_WORDBREAK) != 0)
    {
      const unsigned uiCharCount = (unsigned)((float)dWidth / ((float)_pParams->arrSpansWidth.getLast() / (float)_pParams->arrBreaksPos.getLast() + 0.5f));
      unsigned uiPrevForcedIndex = 0;

      for (unsigned& uiForcedIndex : _pParams->arrForcedBreaksIndex)
      {
        const unsigned uiStartPos = (uiPrevForcedIndex == 0 ? 0 : (_pParams->arrBreaksPos[uiPrevForcedIndex] + 1));

        if (_pParams->arrBreaksPos[uiForcedIndex] == uiStartPos)
        {
          _pParams->arrLines.append(LINE{ 0,
                                          0,
                                          dWidth });
          uiPrevForcedIndex = uiForcedIndex;
          continue;
        }

        FixedString strText(_pParams->strText.getSubString(uiStartPos, _pParams->arrBreaksPos[uiForcedIndex] - uiStartPos));

        unsigned uiLen = strText.findLastOf(L" \t", uiCharCount);
        if (uiLen == FixedString::npos)
        {
          uiLen = strText.findFirstOf(L" \t", uiCharCount);
        }

        unsigned uiBreakIndex = (unsigned)_pParams->arrBreaksPos.find(uiStartPos + uiLen);
        _pParams->arrLines.append(LINE{ uiStartPos,
                                        uiLen,
                                        dWidth - _pParams->arrSpansWidth[uiBreakIndex] + (uiPrevForcedIndex == 0 ? 0 : (_pParams->arrSpansWidth[uiPrevForcedIndex] + _pParams->dNLSize)) });

        uiPrevForcedIndex = uiForcedIndex;

        while (uiForcedIndex > uiBreakIndex)
        {
          const unsigned uiPrevLen = uiLen + 1;
          uiLen                    = strText.findLastOf(L" \t", uiPrevLen + uiCharCount);
          if (uiLen == FixedString::npos || uiPrevLen > uiLen)
          {
            uiLen = strText.findFirstOf(L" \t", uiPrevLen + uiCharCount);
          }

          const unsigned uiPrevBreakIndex = uiBreakIndex;
          uiBreakIndex                    = (unsigned)_pParams->arrBreaksPos.find(uiStartPos + uiLen);
          _pParams->arrLines.append(LINE{ uiStartPos + uiPrevLen,
                                          uiLen - uiPrevLen,
                                          dWidth - _pParams->arrSpansWidth[uiBreakIndex] + _pParams->arrSpansWidth[uiPrevBreakIndex] + _pParams->dSpaceSize });
        }
      }

      return;
    }

    unsigned uiPrevForcedIndex = 0;

    for (unsigned& uiForcedIndex : _pParams->arrForcedBreaksIndex)
    {
      const unsigned uiStartPos = (uiPrevForcedIndex == 0 ? 0 : (_pParams->arrBreaksPos[uiPrevForcedIndex] + 1));

      if (_pParams->arrBreaksPos[uiForcedIndex] == uiStartPos)
      {
        _pParams->arrLines.append(LINE{ 0,
                                        0,
                                        dWidth });
        uiPrevForcedIndex = uiForcedIndex;
        continue;
      }

      _pParams->arrLines.append(LINE{ uiStartPos,
                                      _pParams->arrBreaksPos[uiForcedIndex] - uiStartPos,
                                      dWidth - _pParams->arrSpansWidth[uiForcedIndex] + (uiPrevForcedIndex == 0 ? 0 : (_pParams->arrSpansWidth[uiPrevForcedIndex] + _pParams->dNLSize)) });

      uiPrevForcedIndex = uiForcedIndex;
    }
  }


public:
  inline void setText(PCWSTR szText)
  {
    _pParams->strText = szText;

    _parseText();
  }

  inline void setFormat(const unsigned fmt)
  {
    _pParams->uiFormat = fmt;
  }

  inline const FixedString& getText(void)
  {
    return _pParams->strText;
  }

  inline const unsigned& getFormat(void)
  {
    return _pParams->uiFormat;
  }

};  // class TextBox
