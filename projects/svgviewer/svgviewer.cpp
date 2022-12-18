#include "studio.hpp"

#include "mmap.hpp"
#include "binstream.hpp"
#include "svgparser.h"
#include "svgpathviewer.hpp"


#include "svggraphic.h"




#include <iostream>
#include <string_view>

using namespace std;
using namespace svg;
using namespace ndt;
using std::string; 
using std::string_view;




struct CenterWindow : public GraphicGroup
{
    CenterWindow(maths::rectf aframe)
        :GraphicGroup(aframe) {}

    void drawBackground(IGraphics& ctx)
    {
        ctx.push();
        ctx.translate(boundsWidth() / 2, boundsHeight() / 2);
        //ctx.scale(1, -1);

        // axis lines
        ctx.stroke(255, 0, 0);
        ctx.line(0, canvasHeight / 2, 0, -canvasHeight / 2);
        ctx.line(-canvasWidth / 2, 0, canvasWidth / 2, 0);
    }


    
    void draw(IGraphics& ctx) override
    {
        static double scaling = 1.0;
        
        // Start by saving the context state
        // so we're free to mess around with it
        // while we're drawing ourself.
        ctx.push();

        // Before we do anything else, and while we're still
        // in the coordinate system of our parent, we want to setup 
        // a clip for our frame.
        // Once the clip is set, we want to transform our
        // coordinate sytem to have 0,0 be at the upper left corner.
        //ctx.clip(frame());

        ctx.scale(scaling, scaling);

        // BUGBUG - maybe perform arbitrary transform?
        //auto pt = fTransform.mapPoint(fFrame.x, fFrame.y);
        //ctx.translate(pt.x, pt.y);
        //ctx.translate(frameX()/scaling, frameY()/scaling);

        // Apply user specified transform
        //ctx.translate(fTranslation.x, fTranslation.y);


        drawBackground(ctx);
        drawSelf(ctx);
        drawForeground(ctx);

        //ctx.noClip();
        ctx.pop();

    }
};




void testCubics()
{
    //
    // https://developer.mozilla.org/en-US/docs/Web/SVG/Tutorial/Paths
    //
    string_view c1{ "M 10 10 C 20 20, 40 20, 50 10" };
    string_view c2{ "M 70 10 C 70 20, 110 20, 110 10" };
    string_view c3{ "M 130 10 C 120 20, 180 20, 170 10" };
    string_view c4{ "M 10 60 C 20 80, 40 80, 50 60" };
    string_view c5{ "M 70 60 C 70 80, 110 80, 110 60" };
    string_view c6{ "M 130 60 C 120 80, 180 80, 170 60" };
    string_view c7{ "M 10 110 C 20 140, 40 140, 50 110" };
    string_view c8{ "M 70 110 C 70 140, 110 140, 110 110" };
    string_view c9{ "M 130 110 C 120 140, 180 140, 170 110" };


    auto win1 = std::make_shared<CenterWindow>(maths::rectf{ 0,0,(float)canvasWidth,(float)canvasHeight });
    addGraphic(win1);

    win1->addGraphic(std::make_shared<SVGPathViewer>(c1, "c1"));
    win1->addGraphic(std::make_shared<SVGPathViewer>(c2, "c2"));
    win1->addGraphic(std::make_shared<SVGPathViewer>(c3, "c3"));
    win1->addGraphic(std::make_shared<SVGPathViewer>(c4, "c4"));
    win1->addGraphic(std::make_shared<SVGPathViewer>(c5, "c5"));
    win1->addGraphic(std::make_shared<SVGPathViewer>(c6, "c6"));
    win1->addGraphic(std::make_shared<SVGPathViewer>(c7, "c7"));
    win1->addGraphic(std::make_shared<SVGPathViewer>(c8, "c8"));
    win1->addGraphic(std::make_shared<SVGPathViewer>(c9, "c9"));

}

void testArcs()
{
    // Elliptic arcs
    string_view A0{ "M 10 315 L 110 215 A 30 50 0 0 1 162.55 162.45 L 172.55 152.45 A 30 50 -45 0 1 215.1 109.9 L 315 10" };
    string_view A1{"M 80 80 A 45,45,0,0,0,125,125 L 125 80 Z"};
    string_view A2{"M 230 80 A 45 45, 0, 1, 0, 275 125 L 275 80 Z"};
    string_view A3{"M 80 230 A 45 45, 0, 0, 1, 125 275 L 125 230 Z"};
    string_view A4{"M 230 230 A 45 45, 0, 1, 1, 275 275 L 275 230 Z"};


    addGraphic(std::make_shared<SVGPathViewer>(A1, "A1"));
    addGraphic(std::make_shared<SVGPathViewer>(A2, "A2"));
    addGraphic(std::make_shared<SVGPathViewer>(A3, "A3"));
    addGraphic(std::make_shared<SVGPathViewer>(A4, "A4"));
}

void testPathCommands()
{
    std::shared_ptr<CenterWindow> win1 = std::make_shared<CenterWindow>(maths::rectf{ 0,0,(float)canvasWidth,(float)canvasHeight });
    addGraphic(win1);

    // rounded rectangle
    string_view subject1{ "M 15,1.78125 C 7.650778,1.78125 1.78125,7.650778 1.78125,15 L 1.78125,175 C 1.78125,182.34922 7.650778,188.21875 15,188.21875 L 125,188.21875 C 132.34922,188.21875 138.21875,182.34922 138.21875,175 L 138.21875,15 C 138.21875,7.650778 132.34922,1.78125 125,1.78125 L 15,1.78125 z" };

    //std::string subject{ "M 139.21875,69.125 C 136.53463,71.311084 133.86647,73.495215 131.34375,75.28125 C 127.98336,77.660341 124.76748,79.684674 121.65625,81.4375 C 118.54502,83.190326 115.5296,84.652975 112.625,85.875 C 109.7204,87.097025 106.928,88.08831 104.1875,88.875 C 101.447,89.66169 98.775171,90.27193 96.15625,90.71875 C 93.537329,91.16557 90.977375,91.453835 88.4375,91.65625 C 83.35775,92.06108 78.393738,92.0625 73.375,92.0625 C 68.356262,92.0625 63.266706,92.047358 57.96875,92.40625 C 55.319772,92.585696 52.626325,92.868884 49.84375,93.28125 C 47.061175,93.693616 44.208701,94.259248 41.25,95 C 38.291299,95.740752 35.208606,96.647897 32.03125,97.8125 C 28.853894,98.977103 25.563539,100.37858 22.125,102.0625 C 18.686461,103.74642 15.117251,105.73255 11.375,108.03125 C 8.6369255,109.71313 5.7016242,111.78851 2.78125,113.84375 L 2.78125,176 C 2.78125,183.34922 8.650778,189.21875 16,189.21875 L 126,189.21875 C 133.34922,189.21875 139.21875,183.34922 139.21875,176 L 139.21875,69.125 z" };


    string_view club1{ "M -51.8125,49.59375 C -55.159734,49.59375 -57.874999,52.309016 -57.875,55.65625 C -57.875,58.48575 -55.490562,60.517725 -53.3125,61.53125 C -52.86816,61.738016 -53.071698,62.876219 -53.3125,63.1875 C -53.498781,63.428302 -54.444685,63.545026 -54.59375,63.1875 C -55.538776,60.920889 -57.638019,58.625 -60.46875,58.625 C -63.815984,58.625 -66.53125,61.340264 -66.53125,64.6875 C -66.53125,68.034734 -63.815986,70.750001 -60.46875,70.75 C -57.638018,70.75 -55.538775,68.45411 -54.59375,66.1875 C -54.444685,65.829974 -53.460893,65.90881 -53.3125,66.1875 C -53.03381,66.710893 -53.039679,67.85107 -53.3125,68.59375 C -54.539679,71.934403 -59.768745,78.4375 -57.8125,78.4375 C -55.773705,78.4375 -47.851385,78.4375 -45.8125,78.4375 C -43.856256,78.4375 -49.085321,71.934403 -50.3125,68.59375 C -50.585321,67.85107 -50.59119,66.710893 -50.3125,66.1875 C -50.164107,65.90881 -49.180315,65.829974 -49.03125,66.1875 C -48.086224,68.454111 -45.986981,70.750001 -43.15625,70.75 C -39.809016,70.75 -37.09375,68.034733 -37.09375,64.6875 C -37.09375,61.340266 -39.809014,58.625 -43.15625,58.625 C -45.986982,58.625 -48.086225,60.920889 -49.03125,63.1875 C -49.180315,63.545026 -50.126219,63.428302 -50.3125,63.1875 C -50.553302,62.876219 -50.75684,61.738016 -50.3125,61.53125 C -48.134438,60.517726 -45.750001,58.48575 -45.75,55.65625 C -45.75,52.309016 -48.465266,49.593751 -51.8125,49.59375 z" };
    string_view club2{ "M -38.25,62.125 C -38.555479,62.302248 -38.848309,62.597931 -39.15625,62.75 C -40.458263,63.392967 -41.787331,63.842552 -43.125,64.125 C -44.462669,64.407448 -45.796442,64.526867 -47.15625,64.59375 C -48.516058,64.660633 -49.88157,64.659979 -51.25,64.65625 C -52.61843,64.652521 -54.011463,64.648138 -55.375,64.71875 C -56.738537,64.789362 -58.092374,64.928844 -59.4375,65.21875 C -60.782626,65.508656 -62.093051,65.970847 -63.40625,66.625 C -63.956627,66.899163 -64.488166,67.393789 -65.03125,67.75 C -64.027041,69.244025 -62.404501,70.281251 -60.46875,70.28125 C -57.895514,70.28125 -55.923834,68.140833 -55.03125,66 C -54.919992,65.733152 -54.691235,65.595041 -54.5,65.53125 C -54.308765,65.467459 -54.123574,65.462274 -53.9375,65.46875 C -53.751426,65.475226 -53.580279,65.500692 -53.40625,65.5625 C -53.232221,65.624308 -53.041566,65.714619 -52.90625,65.96875 C -52.698829,66.358296 -52.624185,66.819857 -52.625,67.3125 C -52.625815,67.805143 -52.711772,68.305656 -52.875,68.75 C -53.529593,70.531946 -55.136383,72.971374 -56.34375,75 C -56.947433,76.014313 -57.448101,76.943267 -57.65625,77.53125 C -57.74655,77.786331 -57.748578,77.915131 -57.75,77.96875 C -55.67211,77.96875 -47.952978,77.96875 -45.875,77.96875 C -45.876422,77.91513 -45.87845,77.786331 -45.96875,77.53125 C -46.176899,76.943267 -46.677567,76.014313 -47.28125,75 C -48.488617,72.971374 -50.095407,70.531946 -50.75,68.75 C -50.913228,68.305656 -50.999185,67.805143 -51,67.3125 C -51.000815,66.819857 -50.926171,66.358296 -50.71875,65.96875 C -50.583434,65.714619 -50.392779,65.624308 -50.21875,65.5625 C -50.044721,65.500692 -49.873574,65.475226 -49.6875,65.46875 C -49.501426,65.462274 -49.316235,65.467459 -49.125,65.53125 C -48.933765,65.595041 -48.705008,65.733152 -48.59375,66 C -47.701165,68.140834 -45.729485,70.281251 -43.15625,70.28125 C -40.066556,70.28125 -37.5625,67.777193 -37.5625,64.6875 C -37.5625,63.752461 -37.8445,62.899594 -38.25,62.125 z" };
    string_view club3{ "M -51.8125,50.5625 C -54.642982,50.5625 -56.906249,52.825767 -56.90625,55.65625 C -56.90625,56.802588 -56.442623,57.796772 -55.6875,58.65625 C -54.932377,59.515728 -53.894751,60.196267 -52.90625,60.65625 C -52.465838,60.861188 -52.240984,61.261862 -52.15625,61.5625 C -52.071516,61.863138 -52.075449,62.123732 -52.09375,62.375 C -52.130353,62.877535 -52.176944,63.282847 -52.5625,63.78125 C -52.825679,64.121457 -53.066822,64.178247 -53.28125,64.25 C -53.495678,64.321753 -53.682113,64.363253 -53.90625,64.375 C -54.130387,64.386747 -54.385593,64.366791 -54.65625,64.28125 C -54.926907,64.195709 -55.286245,64.000229 -55.46875,63.5625 C -56.308554,61.54826 -58.154682,59.59375 -60.46875,59.59375 C -63.299233,59.59375 -65.5625,61.857015 -65.5625,64.6875 C -65.5625,67.517983 -63.299234,69.781251 -60.46875,69.78125 C -58.154681,69.78125 -56.308553,67.826739 -55.46875,65.8125 C -55.283019,65.367033 -54.9237,65.182964 -54.65625,65.09375 C -54.3888,65.004536 -54.137121,64.991964 -53.90625,65 C -53.675379,65.008036 -53.483012,65.042243 -53.25,65.125 C -53.016988,65.207757 -52.702921,65.310214 -52.46875,65.75 C -52.192812,66.268226 -52.155353,66.770198 -52.15625,67.3125 C -52.157147,67.854802 -52.21603,68.419679 -52.40625,68.9375 C -53.102112,70.83179 -54.738956,73.236199 -55.9375,75.25 C -56.481977,76.164834 -56.878826,76.938435 -57.09375,77.46875 C -54.715974,77.46875 -48.909095,77.46875 -46.53125,77.46875 C -46.746174,76.938435 -47.143023,76.164834 -47.6875,75.25 C -48.886043,73.236199 -50.522888,70.83179 -51.21875,68.9375 C -51.40897,68.419679 -51.467853,67.854802 -51.46875,67.3125 C -51.469647,66.770198 -51.432188,66.268226 -51.15625,65.75 C -50.922079,65.310214 -50.608012,65.207757 -50.375,65.125 C -50.141988,65.042243 -49.949621,65.008036 -49.71875,65 C -49.487879,64.991964 -49.2362,65.004536 -48.96875,65.09375 C -48.7013,65.182964 -48.341981,65.367033 -48.15625,65.8125 C -47.316446,67.826741 -45.470317,69.781251 -43.15625,69.78125 C -40.325768,69.78125 -38.0625,67.517981 -38.0625,64.6875 C -38.0625,61.857017 -40.325765,59.59375 -43.15625,59.59375 C -45.470319,59.59375 -47.316447,61.54826 -48.15625,63.5625 C -48.338755,64.000229 -48.698093,64.195709 -48.96875,64.28125 C -49.239407,64.366791 -49.494613,64.386747 -49.71875,64.375 C -49.942887,64.363253 -50.129322,64.321753 -50.34375,64.25 C -50.558178,64.178247 -50.799321,64.121457 -51.0625,63.78125 C -51.448056,63.282847 -51.494647,62.877535 -51.53125,62.375 C -51.549551,62.123732 -51.553484,61.863138 -51.46875,61.5625 C -51.384016,61.261862 -51.159162,60.861188 -50.71875,60.65625 C -49.730249,60.196268 -48.692623,59.515728 -47.9375,58.65625 C -47.182377,57.796772 -46.71875,56.802588 -46.71875,55.65625 C -46.71875,52.825768 -48.982017,50.562501 -51.8125,50.5625 z" };


    auto card1 = std::make_shared<SVGPathViewer>(subject1, "card");


    auto g1 = std::make_shared<SVGPathViewer>(club1, "club1");
    auto g2 = std::make_shared<SVGPathViewer>(club2, "club2");
    auto g3 = std::make_shared<SVGPathViewer>(club3, "club3");

    win1->addGraphic(card1);

    //win1->addGraphic(g1);
    //win1->addGraphic(g2);
    win1->addGraphic(g3);
}


void testNumber()
{
	char buffer[256];
    DataChunk numchunk = make_chunk_size(buffer, 256);
    DataChunk tr = make_chunk_cstr("1.23e2");
    auto units = svg_parseNumber(tr, numchunk);

	printChunk(numchunk);
	printChunk(units);
}

void testTransform()
{
    float xform[6]{ 0 };
	auto tr = make_chunk_cstr("matrix(1 0 0 -1 0 0)");
	svg_parseTransform(xform, tr);
    
    printf("xform: [%3.2f, %3.2f, %3.2f, %3.2f, %3.2f, %3.2f]\n", xform[0], xform[1], xform[2], xform[3], xform[4], xform[5]);
}

void testChunk()
{

    auto mc = make_chunk_cstr("this is a set of words that are space delimited");

	// advance a few characters
    mc += 5;

    
    printChunk(mc);
	printf("Chunk bool: %d\n", (bool)mc);
}

void testLine()
{

    SVGParser p{};
    auto mc = make_chunk_cstr("<svg width='500' height='500'> < line x1 = '100' y1 = '50' x2 = '500' y2 = '50' stroke = 'black' / >< / svg>");
    parseSVGDocument(mc, p);
}

void testLineDisplay()
{
    auto mc = make_chunk_cstr("<svg width='500' height='500'> < line x1 = '100' y1 = '50' x2 = '500' y2 = '50' stroke = 'black' / >< / svg>");
    auto g = std::make_shared<SVGGraphic>(maths::rectf{0,0,(float)canvasWidth, (float)canvasHeight});
    g->initFromChunk(mc);
    
    addGraphic(g);
}

void testRect()
{
    SVGParser p{};
    auto mc = make_chunk_cstr("<svg width='500' height='500'> <rect x='10' y='10' width='50' height='50' /> <rect x='70' y='10' width='50' height='50' rx='10' ry='10'/><rect x='130' y='10' width='50' height='50' rx='15' ry='15'/>< / svg>");

    auto g = std::make_shared<SVGGraphic>(maths::rectf{ 0,0,(float)canvasWidth, (float)canvasHeight });
    g->initFromChunk(mc);

    addGraphic(g);

}

void testPolyLine()
{

    SVGParser p{};
    auto mc = make_chunk_cstr("<svg width='500' height='500'> <polyline points='0,0 30,0 15,30' />< / svg>");
    auto g = std::make_shared<SVGGraphic>(maths::rectf{ 0,0,(float)canvasWidth, (float)canvasHeight });
    g->initFromChunk(mc);

    addGraphic(g);
}

void testPath()
{
    SVGParser p{};
    auto mc = make_chunk_cstr("<svg width='500' height='500'> <path d='M50,50 A30,30 0 0,1 35,20 L100,100 M110,110 L100,0' />< / svg>");
    auto g = std::make_shared<SVGGraphic>(maths::rectf{ 0,0,(float)canvasWidth, (float)canvasHeight });
    g->initFromChunk(mc);

    addGraphic(g);

}

void testParseStyle()
{
    SVGParser p{};
    DataChunk mc = make_chunk_cstr("stop-color:#99bc00;stop-opacity:0");
    
	DataChunk style = svg::svg_parseStyle(p, mc);

	printf("style parsed\n");
}

void testParseDoc()
{
    //constexpr const char* filename = "resources\\bowls.svg";
    //constexpr const char* filename = "resources\\example_cubic1.svg";
    //constexpr const char* filename = "resources\\example_elliptic_arc.svg";
    //constexpr const char* filename = "resources\\grapes.svg";
    //constexpr const char* filename = "resources\\Ghostscript_Tiger.svg";
    //constexpr const char* filename = "resources\\floppy-disk.svg";
    //constexpr const char* filename = "resources\\headphones.svg";
    //constexpr const char* filename = "resources\\house.svg";
    //constexpr const char* filename = "resources\\example_quad01.svg";
    //constexpr const char* filename = "resources\\tango.svg";
    //constexpr const char* filename = "resources\\trysvg_linear.svg";
    constexpr const char* filename = "resources\\svgexample_quad.svg";
    //constexpr const char* filename = "resources\\example_moveby.svg";
    //constexpr const char* filename = "resources\\example_rect.svg";
    //constexpr const char* filename = "resources\\e-path-012.svg";
    
    auto fmap = mmap::create_shared(filename);
    if (fmap == nullptr || !fmap->isValid())
    {
        printf("Could not load file: %s\n", filename);
        halt();
    }

    // We have the file, now try to parse the svg
    SVGParser p{};
    auto mc = fmap->getChunk();

    auto win1 = std::make_shared<CenterWindow>(maths::rectf{ 0,0,(float)canvasWidth,(float)canvasHeight });
    addGraphic(win1);
    
    //parseSVGDocument(chunk, p);
    auto g = std::make_shared<SVGGraphic>(maths::rectf{ 0,0,(float)canvasWidth, (float)canvasHeight });
    g->initFromChunk(mc);

    win1->addGraphic(g);
    
    // close the mapped file
    fmap->close();

    
}

void testPathParse()
{
    // pathological number sequences, '-' terminating, multiple sequenced commands
    //auto mc = make_chunk_cstr("m299.72,80.245c0.62,0.181,2.83,1.305,4.08,2.955,0,0,6.8,10.8,1.6-7.6,0,0-9.2-28.8-0.4-17.6,0,0,6,7.2,2.8-6.4-3.86-16.427-6.4-22.8-6.4-22.8s11.6,4.8-15.2-34.8l8.8,3.6s-19.6-39.6-41.2-44.8l-8-6s38.4-38,25.6-74.8c0,0-6.8-5.2-16.4,4,0,0-6.4,4.8-12.4,3.2,0,0-30.8,1.2-32.8,1.2s-36.8-37.2-102.4-19.6c0,0-5.2,2-9.599,0.8,0,0-18.401-16-67.201,6.8,0,0-10,2-11.6,2s-4.4,0-12.4,6.4-8.4,7.2-10.4,8.8c0,0-16.4,11.2-21.2,12,0,0-11.6,6.4-16,16.4l-3.6,1.2s-1.6,7.2-2,8.4c0,0-4.8,3.6-5.6,9.2,0,0-8.8,6-8.4,10.4,0,0-1.6,5.2-2.4,10,0,0-7.2,4.8-6.4,7.6,0,0-7.6,14-6.4,20.8,0,0-6.4-0.4-9.2,2,0,0-0.8,4.8-2.4,5.2,0,0-2.8,1.2-0.4,5.2,0,0-1.6,2.8-2,4.4,0,0,0.8,2.8-3.6,8.4,0,0-6.4,18.8-4.4,24,0,0,0.4,4.8-2.4,6.4,0,0-3.6-0.4,4.8,11.6,0,0,0.8,1.2-2.4,3.6,0,0-17.2,3.6-19.6,20,0,0-13.6,14.8-13.6,20,0,2.305,0.27,5.452,0.97,10.06,0,0-0.57,8.34,27.03,9.14s402.72-31.355,402.72-31.355z");
    
    // floppy-disk.svg
    //auto mc = make_chunk_cstr("M6,21h10v-6H6V21z M16,4h-3v3h3V4z M19,1H3C1.895,1,1,1.895,1,3v16 c0,1.105,0.895,2,2,2h1v-6c0-1.105,0.895-2,2-2h10c1.105,0,2,0.895,2,2v6h1c1.105,0,2-0.895,2-2V3C21,1.895,20.105,1,19,1z M17,8 H5V3h12V8z");
    //auto mc = make_chunk_cstr("c0-1.105,0.895-2,2-2h10");

	// headphones.svg
	//auto mc = make_chunk_cstr("M19,11.184V9c0-4.418-3.582-8-8-8S3,4.582,3,9v2.184C1.837,11.597,1,12.696,1,14 v4c0,1.657,1.343,3,3,3s3-1.343,3-3v-4c0-1.304-0.837-2.403-2-2.816V9c0-3.314,2.686-6,6-6s6,2.686,6,6v2.184 c-1.163,0.413-2,1.512-2,2.816v4c0,1.657,1.343,3,3,3s3-1.343,3-3v-4C21,12.696,20.163,11.597,19,11.184z");
    
    auto mc = make_chunk_cstr("m-122.3,84.285s0.1,1.894-0.73,1.875c-0.82-0.019-17.27-48.094-37.8-45.851,0,0,17.78-7.353,38.53,43.976z");
    
    
    std::vector<PathSegment> commands{};
    ndt::tokenizePath(mc, commands);
    BLPath apath{};
    blPathFromSegments(commands, apath);

    printf("commands: %zd\n", commands.size());
}

void setup()
{
    //testTransform();
    //testChunk();
    //testLine();
    //testLineDisplay();
    //testNumber();
    //testParse();
    testParseDoc();
    //testParseStyle();
    //testPath();
    //testPathParse();
    //testPolyLine();
    //testRect();
}