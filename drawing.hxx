#ifndef DRAWING_HXX
#define DRAWING_HXX

#include "core.hxx"
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
    } color_;
    Point3D currentPoint_;
    Point3D r_;
    int window_;
    float textScale;
    static std::vector<unsigned int> textures_;
    int tex_;
    Point2D texScale_;
    
    void LookAt(Point3D O, Point3D P);
public:
    Drawing(int window);
    ~Drawing();
    void MoveCamera(Point3D O, float rx, float ry);

    void SetColor(Color);
    void SetFill(Color);
    void MoveTo(Point3D);
    void LineTo(Point3D);
    void WireCube(float);
    void WireSphere(float);
    void SetRotations(float rx, float ry, float rz);
    void Quad(float w, float h);
    void WireQuad(float w, float h);

    void SetTexture(int);
    void SetTextureScale(float, float);
    void TextureQuad(float w, float h);

    void MoveTo(Point2D);
    void LineTo(Point2D);
    void SetTextScale(float);
    void Text(std::string const&);

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

    static int LoadBitmapTexture(char const* path, int chromaKey = -1 /*0BGR*/);

    struct UtilityHelpers {
        static Point3D Translate(Point3D P, Point3D dP);
        static Point3D Rotate(Point3D P, Point3D O, float rx, float ry);
        static Point3D RotateDeltaVector(Point3D dP, float rx, float ry);
    };
};

#endif
