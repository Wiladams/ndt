#pragma once

#include "blend2d.h"
#include "elements/gconsole.h"
//#include "vt100stream.h"

struct FontFaceConsole : public GConsole
{
	BLFontFace fFontFace{};
	BLFontFaceInfo fFaceInfo{};
	BLFontDesignMetrics designMetrics{};

	FontFaceConsole(size_t cols, size_t rows, const char* facename)
		: GConsole(60, 40, facename)
	{
	}

	void setFace(BLFontFace& fontFace)
	{
		fFontFace = fontFace;

		//printf("displayFace\n");


		if (!fontFace.isValid())
		{
			putsln("invalid font face");
			return;
		}


		fFaceInfo = fontFace.faceInfo();

		// BLFontDesignMetrics, give you the raw metrics from 
		// when the fontface was parsed.
		designMetrics = fontFace.designMetrics();


		newLine();
		puts(" Font Full Name: ");

		printf("%s\n", fontFace.fullName().data());
		printf("         family: %s\n", fontFace.familyName().data());
		printf("     sub-family: %s\n", fontFace.subfamilyName().data());
		printf("postscript name: %s\n", fontFace.postScriptName().data());

		newLine();

		putsln(" == DESIGN METRICS ==");
		newLine();

		printf("          units per EM: %d\n", designMetrics.unitsPerEm);
		printf("           lowest PPEM: %d\n", designMetrics.lowestPPEM);
		printf("              line gap: %d\n", designMetrics.lineGap);
		printf("             mean line: %d\n", designMetrics.xHeight);

		newLine();
		putsln("      Horizontal Layout");
		printf("                ascent: %d\n", designMetrics.ascent);
		printf("               descent: %d\n", designMetrics.descent);
		printf("   min leading bearing: %d\n", designMetrics.hMinLSB);
		printf("  min trailing bearing: %d\n", designMetrics.hMinTSB);
		printf("           max advance: %d\n", designMetrics.hMaxAdvance);
		newLine();

		printf("            cap height: %d\n", designMetrics.capHeight);

		int gbWidth = designMetrics.glyphBoundingBox.x1 - designMetrics.glyphBoundingBox.x0;
		int gbHeight = designMetrics.glyphBoundingBox.y1 - designMetrics.glyphBoundingBox.y0;

		printf("    glyph bounding box: [%d,%d,%d,%d]\n",
			designMetrics.glyphBoundingBox.x0, designMetrics.glyphBoundingBox.y0,
			designMetrics.glyphBoundingBox.x1, designMetrics.glyphBoundingBox.y1);
		printf("                      : %dx%d\n",
			gbWidth, gbHeight);

		printf("     underlinePosition: %d\n", designMetrics.underlinePosition);
		printf("    underlineThickness: %d\n", designMetrics.underlineThickness);
		printf(" strikethroughPosition: %d\n", designMetrics.strikethroughPosition);
		printf("strikethroughThickness: %d\n", designMetrics.strikethroughThickness);

		newLine();
		newLine();

		putsln(" == FONT INFORMATION == ");

		BLFont fon;
		BLFontMatrix fontMatrix;
		fon.createFromFace(fontFace, 72);
		blFontGetMatrix(&fon, &fontMatrix);

		putsln("  Font Matrix ");
		printf(" %f  %f\n", fontMatrix.m00, fontMatrix.m01);
		printf(" %F  %f\n", fontMatrix.m10, fontMatrix.m11);

	}

	void keyReleased(const KeyboardEvent& e) override
	{
		switch (e.keyCode)
		{

		case VK_DOWN: 
		{
			scrollUp();
		}break;

		}
	}
};
