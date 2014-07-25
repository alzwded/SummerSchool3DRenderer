SummerSchool3DRenderer
======================

3d renderer for kids

Building on Windows
===================

You will need the following:
* Visual C++ 11 compiler (Visual Studio 2012)
* freeglut distribution (from over here http://files.transmissionzero.co.uk/software/development/GLUT/older/ , get `freeglut-MSVC-2.8.0-1.mp.zip`)

After you have everything, extract/copy the `freeglut` folder as it is in the project's root directory (`/`, alongside `sample.bmp` and `drawing.cxx`).

Your project folder should look like this:
```
/
|- freeglut/            <-- put the freeglut folder as shown here
|  |- bin/
|  |  \- freeglut.dll
|  |- include/
|  |  \- GL/
|  |     |- freeglut.h
|  |     |- freeglut_std.h
|  |     |- freeglut_ext.h
|  |     \- glut.h
|  \- lib/
|     \- freeglut.lib   <-- visual studio/c++ static library (!)
|- renderer/
|  |- renderer.sln
|  \- renderer.vcxproj
|- bitmap.h
|- core.hxx
|- drawing.cxx
|- drawing.hxx
|- main.cxx
|- main.hxx
|- sample.bmp
|- LICENSE
\- README.md
```

Then, open up the visual studio solution and compile. There you go!

Usage
=====

```C++
#include "drawing.hxx"

// internal state
static float rx, ry;
static Point3D ppos(1, 1, 1);

// draw callback
static void drawScene(Drawing& dwg)
{
    dwg.MoveCamera(                         // position the camera
        ppos,
        -ry,
        -rx);

    // draw a cube
    dwg.SetColor(Drawing::WHITE);           // pick the pen color,
    dwg.SetRotations(jx, jy, jz);           // set rotations,
    dwg.MoveTo(Point3D(0, 1, -5));          // set location,
    dwg.WireCube(1);                        // and draw a wireframe cube

    // draw a quad
    dwg.SetColor(Drawing::CYAN);
    dwg.SetTexture(1);                      // pick a texture
    dwg.MoveTo(Point3D(5, 0, 4));
    dwg.SetTextureScale(1, 1);
    dwg.SetRotations(0, 0, 0);
    dwg.TextureQuad(10, 1);                 // draw a textured quad
    dwg.WireQuad(10, 1);                    // and its outline

    // draw crosshairs
    dwg.SetColor(Drawing::YELLOW);
    dwg.MoveTo(Point2D(495, 500));          // in overlay context...
    dwg.LineTo(Point2D(505, 500));
    dwg.MoveTo(Point2D(500, 495));
    dwg.LineTo(Point2D(500, 505));          // ...draw crosshairs

    // show some HUD text
    dwg.MoveTo(Point2D(70, 70));
    dwg.SetTextScale(5);
    dwg.Text("Hello World!");               // show up some HUD text

    // camera facing sprite
    dwg.MoveTo(Point3D(-10, 1, -1));
    dwg.SetTextureScale(0, 0);
    dwg.SetTexture(0);
    dwg.SpriteQuad(1, 1);

    // cylindrical billboard
    dwg.MoveTo(Point3D(-10, 1, 3));
    dwg.SetTexture(0);
    dwg.SetRotations(90, 0, -ry);
    dwg.TextureQuad(1, 1);
}

// pre-draw callback
static void updateScene()
{
    // update internal state...
    rx += 1.f;
    ry += 1.f;
}

static void onkeyup(char key)
{
    if(key == 27) exit(0);
}

int main(int argc, char* argv[])
{
    Drawing::Init(&argc, argv);             // initialize the engine
    Drawing::SetOnMouseDown(onmousedown);   // set up input handlers
    Drawing::SetOnMouseUp(onmouseup);
    Drawing::SetOnMouseMove(onmousemove);
    Drawing::SetOnKeyUp(onkeyup);
    Drawing::SetOnKeyDown(onkeydown);

    Drawing::LoadBitmapTexture("sample.bmp"); // load a texture
    Drawing::LoadBitmapTexture("sample.bmp", // load a texture with an
    //  R          G                 B          RGB chroma key
        34 + 256 * 177 + 256 * 256 * 76);
    
    Drawing::Loop(updateScene, drawScene);  // run the engine's main loop

    return 0;
}
```
