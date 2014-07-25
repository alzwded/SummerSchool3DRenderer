/*
Copyright (c) 2014, Vlad Mesco
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <iostream>
#include "core.hxx"
#include "drawing.hxx"

static bool moving = false, moving2 = false;
static float dlta = 0.1, dlta2 = 0.f;

static float jx = 0, jy = 0, jz = 0;

float rx = 0, ry = 0;
Point3D ppos(0, 0.5, -2);

static void drawScene(Drawing& dwg)
{
    dwg.MoveCamera(
        ppos,
        -ry,
        -rx);

    dwg.SetColor(Drawing::WHITE);
    dwg.SetRotations(jx, jy, jz);
    dwg.MoveTo(Point3D(0, 1, -5));
    dwg.WireCube(1);
    dwg.SetRotations(-jx, -jy, -jz);
    dwg.SetColor(Drawing::YELLOW);
    dwg.WireCube(0.5);

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

    // camera facing sprites
    dwg.MoveTo(Point3D(-10, 1, -1));
    dwg.SetTextureScale(0, 0);
    dwg.SetTexture(0);
    dwg.SpriteQuad(1, 1);
    
    dwg.MoveTo(Point3D(-10, 1, -3));
    dwg.SetTexture(1);
    dwg.SpriteQuad(1, 1);

    // cylindrical billboard
    dwg.MoveTo(Point3D(-10, 1, 3));
    dwg.SetTexture(0);
    dwg.SetRotations(90, 0, -ry);
    dwg.TextureQuad(1, 1);

    // in case texture's missing
    dwg.MoveTo(Point3D(-10, 1, -5));
    dwg.SetTexture(-1);
    dwg.SpriteQuad(1, 2);
}

static void onmousedown(int x, int y, int btn)
{
    if(btn == 1) {
    }
    if(btn == 2) {
    }
    if(btn == 3) {
    }
}

static void onmouseup(int x, int y, int btn)
{
}

static void onmousemove(int dx, int dy)
{
    ry += dx * 0.1;
    rx += dy * 0.1;
    if(rx > 90.f) rx = 90.f;
    if(rx < -90.f) rx = -90.f;
}

static void updateScene()
{
#define ACTUAL_FRAMES_PER_ANIMATION_FRAME 30
    static size_t frameCounter = 0;

    if(moving || moving2) {
        Point3D dir(dlta2, 0, dlta);
        dir = Drawing::UtilityHelpers::normalizeVector(dir);
        dir.x /= 10.f;
        dir.y /= 10.f;
        dir.z /= 10.f;
        dir = Drawing::UtilityHelpers::RotateDeltaVector(dir, -ry, -rx);
        ppos = Drawing::UtilityHelpers::Translate(ppos, dir);
    }

    jx += 5;
    jy += 3;
    jz += 2;
    if(jx > 360) jx -= 360;
    if(jy > 360) jy -= 360;
    if(jz > 360) jz -= 360;
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
