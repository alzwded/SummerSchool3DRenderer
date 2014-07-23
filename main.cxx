#include <cstdio>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <ctime>
#include "core.hxx"
#include "drawing.hxx"

static bool dragging = false;
static bool panningUp = false;
static bool panning = false;
static int lastX = 0, lastY = 0;
static bool moving = false, moving2 = false;
static float dlta = 0.1, dlta2 = 0.f;

float rx = 0, ry = 0;
Point3D ppos(0, 0.5, -2);

static Beam::vector beams;
static Sensor::vector sensors;
static Button::vector buttons;
static AnimationData ad;

static int animFrame = 0;
static bool animating = false;
static bool showRange = false;


static bool clickEnabled = false;


struct ButtonDrawer
{
    Drawing& dwg;
    ButtonDrawer(Drawing& drawing)
        : dwg(drawing)
    {}

    void operator()(Button const& o)
    {
        if(o.highlighted) {
            dwg.SetColor(Drawing::LIME);
        } else {
            dwg.SetColor(Drawing::WHITE);
        }

        Point2D p2(o.extent.x, o.location.y);
        Point2D p4(o.location.x, o.extent.y);

        dwg.MoveTo(o.location);
        dwg.LineTo(p2);
        dwg.LineTo(o.extent);
        dwg.LineTo(p4);
        dwg.LineTo(o.location);

        Point2D textLocation(o.location.x + 3, o.extent.y - 3);
        dwg.MoveTo(textLocation);
        dwg.SetTextScale(5);
        dwg.Text(o.text);
    }
};

static void drawScene(Drawing& dwg)
{
    dwg.MoveCamera(
        ppos,
        -rx,
        -ry);

    dwg.SetColor(Drawing::CYAN);
    dwg.MoveTo(Point3D(-5, 0, 0));
    dwg.LineTo(Point3D(5, 0, 0));
    dwg.SetColor(Drawing::YELLOW);
    dwg.MoveTo(Point3D(0, -5, 0));
    dwg.LineTo(Point3D(0, 5, 0));
    dwg.SetColor(Drawing::CYAN);
    dwg.MoveTo(Point3D(0, 0, -5));
    dwg.LineTo(Point3D(0, 0, 5));

    dwg.MoveTo(Point3D(0, 0, 0));
    dwg.SetColor(Drawing::SEABLUE);
    dwg.SetRotations(0, 0, 45);
    dwg.Quad(1, 1);
    dwg.SetColor(Drawing::CYAN);
    dwg.WireQuad(1, 1);
    
    dwg.MoveTo(Point3D(0, 0, 0));
    dwg.SetColor(Drawing::SEAGREEN);
    dwg.SetRotations(90, 0, 0);
    dwg.Quad(0.7, 0.7);
    dwg.SetColor(Drawing::LIME);
    dwg.WireQuad(0.7, 0.7);
    
    dwg.MoveTo(Point3D(0, 0, 0));
    dwg.SetColor(Drawing::SEAYELLOW);
    dwg.SetRotations(0, 90, 0);
    dwg.Quad(0.4, 0.4);
    dwg.SetColor(Drawing::YELLOW);
    dwg.WireQuad(0.4, 0.4);

    dwg.MoveTo(Point3D(0, 0, 2));
    dwg.SetTexture(1);
    dwg.SetTextureScale(0, 0);
    dwg.SetRotations(0, 90, 90);
    //dwg.SetRotations(ry, rx, 0);
    dwg.TextureQuad(10, 5);

    dwg.SetTexture(0);
    dwg.MoveTo(Point3D(0, 0, 4));
    dwg.SetTextureScale(1, 1);
    dwg.SetRotations(0, 0, 0);
    dwg.TextureQuad(10, 1);
    dwg.MoveTo(Point3D(5, 0, 4));
    dwg.SetTextureScale(1, 1);
    dwg.SetRotations(0, 0, 0);
    dwg.TextureQuad(10, 1);
    
    dwg.SetTexture(1);
    dwg.MoveTo(Point3D(10, 0, 5.5));
    dwg.SetTextureScale(1, 1);
    dwg.SetRotations(0, 0, 0);
    dwg.TextureQuad(10, 1);
    dwg.SetColor(Drawing::WHITE);
    dwg.WireQuad(10, 1);

    dwg.MoveTo(Point3D(20, 0, 5.5));
    dwg.SetTextureScale(1, 1);
    dwg.SetRotations(0, 0, 0);
    dwg.TextureQuad(10, 1);
    dwg.SetColor(Drawing::WHITE);
    dwg.WireQuad(10, 1);


    // Overlay
    dwg.SetColor(Drawing::WHITE);
    dwg.MoveTo(Point2D(10, 950));
    dwg.SetTextScale(5);
    dwg.Text("Selecteaza nodul de start:");
    std::for_each(buttons.begin(), buttons.end(), ButtonDrawer(dwg));
}

static void onmousedown(int x, int y, int btn)
{
    for(Button::vector::iterator i = buttons.begin();
            i != buttons.end(); ++i)
    {
        Button& btn = *i;
        if(!dragging && !panning && !panningUp
                && btn.location.x < x && btn.location.y < y
                && btn.extent.x > x && btn.extent.y > y)
        {
            clickEnabled = true;
            return;
        }
    }

    lastX = x;
    lastY = y;
    if(btn == 1) {
        dragging = true;
    }
    if(btn == 2) {
        panningUp = true;
    }
    if(btn == 3) {
        panning = true;
    }
}

static void onmouseup(int x, int y, int btn)
{
    for(Button::vector::iterator i = buttons.begin();
            clickEnabled && i != buttons.end();
            ++i)
    {
        Button& btn = *i;
        if(!dragging && !panning && !panningUp
                && btn.location.x < x && btn.location.y < y
                && btn.extent.x > x && btn.extent.y > y)
        {
            if(btn.clicked) btn.clicked();
        }
    }

    if(clickEnabled) {
        clickEnabled = false;
        return;
    }

    if(btn == 1) {
        dragging = false;
    }
    if(btn == 2) {
        panningUp = false;
    }
    if(btn == 3) {
        panning = false;
    }
}

static void onmousemove(int dx, int dy)
{
    rx += dx * 0.1;
    ry += dy * 0.1;
    if(ry > 90.f) ry = 90.f;
    if(ry < -90.f) ry = -90.f;
}

static void PrecClicked()
{
   
}

static void UrmClicked()
{

}

static void sensordeselect(Sensor& s)
{
    s.selected = false;
}

static void AnimClicked()
{
   
}

static void updateScene()
{
#define ACTUAL_FRAMES_PER_ANIMATION_FRAME 30
    static size_t frameCounter = 0;

    if(moving || moving2) {
        Point3D dir(dlta2, 0, dlta);
        dir = Drawing::UtilityHelpers::RotateDeltaVector(dir, -rx, -ry);
        ppos = Drawing::UtilityHelpers::Translate(ppos, dir);
    }
}

static void RangeClicked()
{
    showRange = !showRange;
    if(showRange) printf("showing sensor range as a sphere\n");
    else printf("showing only connections\n");
}

static void addButtons()
{
    buttons.push_back(Button(
                Point2D(10, 970),
                Point2D(60, 990),
                "Prec",
                PrecClicked));
    buttons.push_back(Button(
                Point2D(110, 970),
                Point2D(160, 990),
                "Urm",
                UrmClicked));
    buttons.push_back(Button(
                Point2D(700, 970),
                Point2D(980, 990),
                "Porneste/Opreste animatia",
                AnimClicked));
    buttons.push_back(Button(
                Point2D(700, 945),
                Point2D(980, 965),
                "Show range",
                RangeClicked));
}

static void onkeyup(char key)
{
    if(key == 27) exit(0);
    switch(key) {
    case 'd':
    case 'a':
        moving2 = false;
        dlta2 = 0.f;
        break;
    case 'w':
    case 's':
        moving = false;
        dlta = 0.f;
        break;
    }
}

static void onkeydown(char key)
{
    switch(key) {
    case 'w':
        moving = true;
        dlta = 0.1;
        break;
    case 's':
        moving = true;
        dlta = -0.1;
        break;
    case 'a':
        moving2 = true;
        dlta2 = -0.1;
        break;
    case 'd':
        moving2 = true;
        dlta2 = 0.1;
    }
}

int main(int argc, char* argv[])
{

    addButtons();

    Drawing::Init(&argc, argv);
    Drawing::SetOnMouseDown(onmousedown);
    Drawing::SetOnMouseUp(onmouseup);
    Drawing::SetOnMouseMove(onmousemove);
    Drawing::SetOnKeyUp(onkeyup);
    Drawing::SetOnKeyDown(onkeydown);

    Drawing::LoadBitmapTexture("sample.bmp");
    
    Drawing::LoadBitmapTexture("sample.bmp", 34 + 256 * 177 + 256 * 256 * 76);
    
    Drawing::Loop(updateScene, drawScene);

    return 0;
}
