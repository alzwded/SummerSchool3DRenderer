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
    /** Supported colors */
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
    
    /** Set the callback for the click event */
    static void SetOnMouseDown(
            void (*f)(int x, int y, int button));
    /** Set the callback for the mouse button release event */
    static void SetOnMouseUp(
            void (*f)(int x, int y, int button));
    /** Set the callback for mouse movement */
    static void SetOnMouseMove(
            void (*f)(int dx, int dy));
    /** Set the callback for the key press event */
    static void SetOnKeyDown(
            void (*f)(char key));
    /** Set the callback for the key release event */
    static void SetOnKeyUp(
            void (*f)(char key));

    /** Initialize the engine. This must be called ASAP */
    static void Init(int* argc, char* argv[]);
    /** Enter the engine's main loop. Does not usually return,
      * so you should call this last.
      * the update callback is called before the draw callback
      * every 17ms or so.
      */
    static void Loop(
            void (*update)(),
            void (*draw)(Drawing&));

    /** Resize the window to w x h */
    static void SetWindowSize(int w, int h);

    /** Load a texture from a bitmap file.
      * You can opt to use a chroma key (transparent colour) by giving
      * a 24bit integer (padded with 0s in the most significant byte)
      * The chroma can be set to default by passing in -1
      * You can use the returned index with the SetTexture instruction
      */
    static int LoadBitmapTexture(std::string const& path, int chromaKey = -1 /*0BGR*/);

    /** Utility functions */
    struct UtilityHelpers {
        /** Translate a point by a direction vector */
        static Point3D Translate(Point3D P, Point3D dP);
        /** Rotate a point around an origin point */
        static Point3D Rotate(Point3D P, Point3D O, float rx, float ry);
        /** Rotate a direction vector around an origin point */
        static Point3D RotateDeltaVector(Point3D dP, float rx, float ry);
        /** Normalize a vector */
        static Point3D normalizeVector(Point3D P);
        /** Perform a cross product between two vectors */
        static Point3D crossProduct(Point3D v1, Point3D v2);    
        /** Perform a dot product between two vectors */
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
    Point2D rr_;
    
    static void draw_();
    Drawing(int window = 1);
    ~Drawing();
public:
    /** Set the camera position and rotations */
    void MoveCamera(Point3D O, float ry, float rx);
    
    /** Set the color used by the pen */
    void SetColor(Color);
    /** Position the drawing wand */
    void MoveTo(Point3D);
    /** Draw a line from the wand's current position to this point */
    void LineTo(Point3D);

    /** Set the rotations used by the Cube, Sphere and Quad instructions */
    void SetRotations(float rx = 0.f, float ry = 0.f, float rz = 0.f);
    /** Draw a wireframe cube with a given length */
    void WireCube(float);
    /** Draw a wireframe sphere with a given length */
    void WireSphere(float);
    /** Draw a wireframe quad with given dimensions */
    void WireQuad(float w, float h);
    /** Draw a color filled quad with given dimensions */
    void Quad(float w, float h);

    /** Set the currently active texture for the TextureQuad and 
      * SpriteQuad instructions. The index is number returned by
      * LoadBitmapTexture. Pass in -1 to use no texture.
      */
    void SetTexture(int = -1);
    /** Configure texture scaling (repeat)
      * Pass in 0.f, 0.f to disable texture scaling (in this
      * case the texture fills an entire quad
      */
    void SetTextureScale(float = 0.f, float = 0.f);
    /** Draw a textured quad with given dimensions */
    void TextureQuad(float w, float h);
    /** Draw a textured quad with given dimensions that always faces
      * the camera
      */
    void SpriteQuad(float w, float h);

    /** Overlay drawing instructions */
    void MoveTo(Point2D);
    /** Overlay drawing instructions. Line from current point to point */
    void LineTo(Point2D);
    /** Set the font size */
    void SetTextScale(float = .9f);
    /** Draw text at the current location */
    void Text(std::string const&);
};

#endif
