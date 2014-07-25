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
#ifndef DRAWING_HXX
#define DRAWING_HXX

#include "core.hxx"
#include <vector>
#include <string>

class Drawing {
public:
    enum Color {
        CYAN = 0x00FFFF,
        SALMON = 0xC08080,
        YELLOW = 0xFFFF00,
        LIME = 0x00FF00,
        WHITE = 0xFFFFFF,
        BLUE = 0x0000FF,
        SEABLUE = 0x8080C0,
        SEAGREEN = 0x80C080,
        SEAYELLOW = 0xC0C080,
        BLACK = 0x000000
    };
    
    static void SetOnMouseDown(
            void (*f)(int x, int y, int button));
    static void SetOnMouseUp(
            void (*f)(int x, int y, int button));
    static void SetOnMouseMove(
            void (*f)(int dx, int dy));
    static void SetOnKeyDown(
            void (*f)(char key));
    static void SetOnKeyUp(
            void (*f)(char key));

    static void Init(int* argc, char* argv[]);
    static void Loop(
            void (*update)(),
            void (*draw)(Drawing&));

    // -1 to disable chroma key
    static int LoadBitmapTexture(std::string const& path, int chromaKey = -1 /*0BGR*/);

    struct UtilityHelpers {
        static Point3D Translate(Point3D P, Point3D dP);
        static Point3D Rotate(Point3D P, Point3D O, float rx, float ry);
        static Point3D RotateDeltaVector(Point3D dP, float rx, float ry);
        static Point3D normalizeVector(Point3D P);
        static Point3D crossProduct(Point3D v1, Point3D v2);    
        static float dotProduct(Point3D v1, Point3D v2);
    };

private:
    static std::vector<unsigned int> textures_;
    
    int window_;
    Point3D currentPoint_;
    Point3D r_;
    int tex_;
    Color color_;
    float textScale_;
    Point2D texScale_;
    float rrx_;
    
    static void draw_();
    Drawing(int window = 1);
    ~Drawing();
public:
    void MoveCamera(Point3D O, float rx, float ry);
    
    void MoveTo(Point3D);
    void LineTo(Point3D);
    void SetColor(Color);
    void SetRotations(float rx = 0.f, float ry = 0.f, float rz = 0.f);

    void WireCube(float);
    void WireSphere(float);
    void WireQuad(float w, float h);
    void Quad(float w, float h);

    // -1 to disable
    void SetTexture(int = -1);
    // 0, 0 to disable
    void SetTextureScale(float = 0.f, float = 0.f);
    void TextureQuad(float w, float h);
    void SpriteQuad(float w, float h);

    void MoveTo(Point2D);
    void LineTo(Point2D);
    void SetTextScale(float = .9f);
    void Text(std::string const&);
};

#endif
