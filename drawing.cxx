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
#include "drawing.hxx"
#include "bitmap.h"
#include <GL/freeglut.h>
#include <cmath>

#define COS(U) cosf( U / 180.f * 3.14159 )
#define SIN(U) sinf( U / 180.f * 3.14159 )
#define SETFUCKINGCOLOR(R, G, B) glColor3f( (R) / 255.f , (G) / 255.f , (B) / 255.f )

static int window;
static void (*drawFunc_)(Drawing&) = NULL;
static void (*updateFunc_)() = NULL;

static void (*onmousedown)(int, int, int) = NULL;
static void (*onmouseup)(int, int, int) = NULL;
static void (*onmousemove)(int, int) = NULL;
static void (*onkeydown)(char) = NULL;
static void (*onkeyup)(char) = NULL;

static int windowW = 1000, windowH = 1000;

// multiply a GL 4x4 matrix with a vec4 => vec4
static void multMat4Vec4(GLfloat* mat, GLfloat* vec, GLfloat* ret)
{
    for(size_t i = 0; i < 4; ++i) {
        ret[i] = 0.f
               + mat[i + 0] * vec[0]
               + mat[i + 4] * vec[1]
               + mat[i + 8] * vec[2]
               + mat[i + 12] * vec[3];
    }
}

Point3D Drawing::UtilityHelpers::crossProduct(Point3D v1, Point3D v2)
{
    return Point3D(
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x);
}

float Drawing::UtilityHelpers::dotProduct(Point3D v1, Point3D v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

static void setPerspective()
{
	gluPerspective(72.3, (double)windowW / (double)windowH, 0.1, 30.0); 
}

// this is by far the single function I'm most proud of
// It's the first time I can get opengl to rotate a direction vector
//     according to the camera for me
static Point3D rotateDirectionVector(Point3D dP, float rx, float ry)
{
    glMatrixMode(GL_PROJECTION); // work in GL_PROJECTION_MATRIX
    glPushMatrix(); // save previou transforms

    // build the transformation matrix
    // - start with I
    glLoadIdentity();
    // - apply current camera rotations
    glRotatef(rx, 0.f, 1.f, 0.f);
    glRotatef(ry, 1.f, 0.f, 0.f);
    // retrieve transformation matrix
    GLfloat mat[16] = {
        1.f, 0.f, 0.f, 0.f, 
        0.f, 1.f, 0.f, 0.f, 
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f,
    };
    glGetFloatv(GL_PROJECTION_MATRIX, mat);

    // apply the camera transformation to our direction vector
    GLfloat d[4] = { dP.x , dP.y , dP.z , 1.f };
    GLfloat rez[4] = { 0.f, 0.f, 0.f, 0.f };
    multMat4Vec4(mat, d, rez);
    
    // restore GL_PROJECTION_MATRIX
    glPopMatrix();

    // return result
    return Point3D(rez[0], rez[1], rez[2]);
}

void Drawing::draw_()
{
    Drawing dwg(window);
    if(drawFunc_) drawFunc_(dwg);
}

static void update_(int)
{
    if(updateFunc_) updateFunc_();
    
    glutPostRedisplay();
    glutTimerFunc(17, update_, 0);
}

void Drawing::SetOnMouseUp( void (*f)(int x, int y, int button))
{
    onmouseup = f;
}
void Drawing::SetOnMouseDown( void (*f)(int x, int y, int button))
{
    onmousedown = f;
}
void Drawing::SetOnMouseMove( void (*f)(int dx, int dy))
{
    onmousemove = f;
}
void Drawing::SetOnKeyDown( void (*f)(char key))
{
    onkeydown = f;
}
void Drawing::SetOnKeyUp( void (*f)(char key))
{
    onkeyup = f;
}

static void initGL()
{
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ALPHA);
    windowW = 500;
    windowH = 500;
	glutInitWindowSize(windowW, windowH); 

    window = glutCreateWindow("3dRendering");
}

static void initRendering()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

static void handleResize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION); 

	glLoadIdentity(); 
    setPerspective();

    windowW = w;
    windowH = h;
}

static void handleKeyRelease(unsigned char key, int x, int y)
{
    if(onkeyup) onkeyup(key);
}

static void handleKeyPress(unsigned char key, int x, int y)
{
    if(onkeydown) onkeydown(key);
}

static void handleMouse(int button, int state, int X, int Y)
{
    int btn = 0;
    switch(button) {
    case GLUT_LEFT_BUTTON:
        btn = 1;
        break;
    case GLUT_MIDDLE_BUTTON:
        btn = 2;
        break;
    case GLUT_RIGHT_BUTTON:
        btn = 3;
        break;
    }

    int x = 0, y = 0;
    x = (int)((float)X / windowW * 1000.f);
    y = (int)((float)Y / windowH * 1000.f);

    switch(state) {
    case GLUT_UP:
        if(onmouseup) onmouseup(x, y, btn);
        break;
    case GLUT_DOWN:
        if(onmousedown) onmousedown(x, y, btn);
        break;
    }
}

static void resetPointer() {
    glutWarpPointer(windowW/2, windowH/2);
}

static void handleMouseMotion(int X, int Y)
{
    int x = 0, y = 0;
    x = (int)((float)X / windowW * 1000.f);
    y = (int)((float)Y / windowH * 1000.f);
    int dx = 500 - x;
    int dy = 500 - y;
    
    if(abs(dx) > 5 || abs(dy) > 5) {
        if(onmousemove) onmousemove(500 - x, 500 - y);
    
        resetPointer();
    }
}

static void initGL2()
{
	glutKeyboardFunc(handleKeyPress);
	glutMouseFunc(handleMouse);
	glutPassiveMotionFunc(handleMouseMotion);
	glutMotionFunc(handleMouseMotion);
	glutKeyboardUpFunc(handleKeyRelease);
	glutReshapeFunc(handleResize);
}

void Drawing::Init(int* argc, char* argv[])
{
    glutInit(argc, argv);
    initGL();
    initRendering();
    initGL2();
    
    resetPointer();
    glutSetCursor(GLUT_CURSOR_NONE);
}

void Drawing::Loop(
        void (*update)(),
        void (*draw)(Drawing&))
{
    drawFunc_ = draw;
    updateFunc_ = update;
    glutDisplayFunc(draw_);
    glutTimerFunc(17, update_, 0);
    glutMainLoop();
}

std::vector<unsigned int> Drawing::textures_;

Drawing::Drawing(int window)
    : window_(window)
    , currentPoint_(0.f, 0.f, 0.f)
    , r_(0, 0, 0)
    , tex_(-1)
    , color_(YELLOW)
    , textScale_(0.9f)
    , texScale_(0, 0)
    , rrx_(0.f)
{
    // setup
	glClearColor(.0, .0, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    setPerspective();

	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity(); 
	glEnable(GL_DEPTH_TEST);

    SETFUCKINGCOLOR((color_ >> 16) % 256, (color_ >> 8) % 256, color_ % 256);
}

Drawing::~Drawing()
{
	glutSwapBuffers(); 
}

void Drawing::SetColor(Color c)
{
    color_ = c;
    SETFUCKINGCOLOR((color_ >> 16) % 256, (color_ >> 8) % 256, color_ % 256);
}

void Drawing::MoveTo(Point3D p)
{
    currentPoint_ = p;
}

void Drawing::MoveTo(Point2D p)
{
    currentPoint_.x = p.x / 10.f;
    currentPoint_.y = p.y / 10.f;
}

void Drawing::LineTo(Point3D p)
{
    glLineWidth(3.0);
    glBegin(GL_LINES);
    glVertex3f(currentPoint_.x, currentPoint_.y, -currentPoint_.z);
    glVertex3f(p.x, p.y, -p.z);
    glEnd();
    MoveTo(p);
}

static void EnterOverlayContext()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 100.0f, 100.0f, 0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
}

static void LeaveOverlayContext()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void Drawing::LineTo(Point2D p)
{
    EnterOverlayContext();

    glLineWidth(3.0);

    glBegin(GL_LINES);
    glVertex2f(currentPoint_.x, currentPoint_.y);
    glVertex2f(p.x / 10.f, p.y / 10.f);
    glEnd();

    LeaveOverlayContext();

    MoveTo(p);
}

void Drawing::SetTextScale(float s)
{
    textScale_ = s / 100.f / 5.f;
}

void Drawing::Text(std::string const& s)
{
    EnterOverlayContext();

    glPushMatrix();
    glLineWidth(0.5);
    glTranslatef(currentPoint_.x, currentPoint_.y, 0.f);
	glScalef(textScale_, -textScale_, 0.0f);
	for(int i = 0; i < s.size(); ++i) {
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, s[i]);
	}
    glPopMatrix();

    LeaveOverlayContext();
}

void Drawing::WireCube(float l)
{
    glPushMatrix();
    glTranslatef(currentPoint_.x, currentPoint_.y, -currentPoint_.z);
    glRotatef(r_.x, 1, 0, 0);
    glRotatef(r_.y, 0, 1, 0);
    glRotatef(r_.z, 0, 0, 1);
    glLineWidth(2);
    glutWireCube(l);
    glPopMatrix();
}

void Drawing::WireSphere(float r)
{
    glPushMatrix();
    glTranslatef(currentPoint_.x, currentPoint_.y, -currentPoint_.z);
    glRotatef(r_.x, 1, 0, 0);
    glRotatef(r_.y, 0, 1, 0);
    glLineWidth(0.2);
    glutWireSphere(r, 10, 10);
    glPopMatrix();
}

Point3D Drawing::UtilityHelpers::normalizeVector(Point3D P)
{
	float l = sqrtf(P.x * P.x + P.y * P.y + P.z * P.z);
	if(l > -1.0e-5 && l < 1.0e-5) return P;
    return Point3D(P.x / l, P.y / l, P.z / l);
}

void Drawing::SetRotations(float rx, float ry, float rz)
{
    r_ = Point3D(rx, ry, rz);
}

void Drawing::WireQuad(float w, float h)
{
    // in XZ: p1, p1 + (w, 0), p1 + (w, h), p1 + (0, h)
    // I have p1 and N
    Point3D p1 = Point3D(-w/2.f, 0, -h/2.f);
    Point3D p2 = Point3D(w/2.f, 0, -h/2.f);
    Point3D p3 = Point3D(w/2.f, 0, h/2.f);
    Point3D p4 = Point3D(-w/2.f, 0, h/2.f);

    Point3D OP = currentPoint_;
    
    glPushMatrix();
    glLineWidth(2);

    glTranslatef(currentPoint_.x, currentPoint_.y, -currentPoint_.z);
    glRotatef(r_.x, 1, 0, 0);
    glRotatef(r_.y, 0, 1, 0);
    glRotatef(r_.z, 0, 0, 1);

    glBegin(GL_LINE_STRIP); {
        glVertex3f(p1.x, p1.y, p1.z);
        glVertex3f(p2.x, p2.y, p2.z);
        glVertex3f(p3.x, p3.y, p3.z);
        glVertex3f(p4.x, p4.y, p4.z);
        glVertex3f(p1.x, p1.y, p1.z);
    } glEnd();

    glPopMatrix();
}

void Drawing::SpriteQuad(float w, float h)
{
    if(tex_ < 0) {
        Point3D bak = r_;
        SetRotations(0, rrx_, 0);
        WireSphere((w + h) / 4.f);
        SetRotations(bak.x, bak.y, bak.z);
        return;
    }

    Point3D p1 = Point3D(-w/2.f, 0, -h/2.f);
    Point3D p2 = Point3D(w/2.f, 0, -h/2.f);
    Point3D p3 = Point3D(w/2.f, 0, h/2.f);
    Point3D p4 = Point3D(-w/2.f, 0, h/2.f);

    Point3D OP = currentPoint_;
    
    glPushMatrix();
    
    glTranslatef(currentPoint_.x, currentPoint_.y, -currentPoint_.z);
    glRotatef(rrx_, 0, 1, 0);
    glRotatef(90, 1, 0, 0);
    
    glAlphaFunc(GL_GREATER, 0.5);
    glEnable(GL_ALPHA_TEST);

    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);//GL_DECAL);
    glBindTexture(GL_TEXTURE_2D, textures_[tex_]);    

    glBegin(GL_QUADS); {
        float tw = 1.0, th = 1.0;
        if(texScale_.x > 0.f) {
            tw = w / texScale_.x;
        }
        if(texScale_.y >0.f) {
            th = h / texScale_.y;
        }
        glTexCoord2f(0.0, 0.0); glVertex3f(p1.x, p1.y, p1.z);
        glTexCoord2f(0.0, tw); glVertex3f(p2.x, p2.y, p2.z);
        glTexCoord2f(th, tw); glVertex3f(p3.x, p3.y, p3.z);
        glTexCoord2f(th, 0.0); glVertex3f(p4.x, p4.y, p4.z);

        glTexCoord2f(0.0, 0.0); glVertex3f(p1.x, p1.y, p1.z);
        glTexCoord2f(th, 0.0); glVertex3f(p4.x, p4.y, p4.z);
        glTexCoord2f(th, tw); glVertex3f(p3.x, p3.y, p3.z);
        glTexCoord2f(0.0, tw); glVertex3f(p2.x, p2.y, p2.z);
    } glEnd();
    glFlush();

    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

void Drawing::TextureQuad(float w, float h)
{
    if(tex_ < 0) {
        return Quad(w, h);
    }

    // in XZ: p1, p1 + (w, 0), p1 + (w, h), p1 + (0, h)
    // I have p1 and N
    Point3D p1 = Point3D(-w/2.f, 0, -h/2.f);
    Point3D p2 = Point3D(w/2.f, 0, -h/2.f);
    Point3D p3 = Point3D(w/2.f, 0, h/2.f);
    Point3D p4 = Point3D(-w/2.f, 0, h/2.f);

    Point3D OP = currentPoint_;
    
    glPushMatrix();
    glLineWidth(2);

    glTranslatef(currentPoint_.x, currentPoint_.y, -currentPoint_.z);
    glRotatef(r_.x, 1, 0, 0);
    glRotatef(r_.y, 0, 1, 0);
    glRotatef(r_.z, 0, 0, 1);
    
    glAlphaFunc(GL_GREATER, 0.5);
    glEnable(GL_ALPHA_TEST);

    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);//GL_DECAL);
    glBindTexture(GL_TEXTURE_2D, textures_[tex_]);    

    glBegin(GL_QUADS); {
        float tw = 1.0, th = 1.0;
        if(texScale_.x > 0.f) {
            tw = w / texScale_.x;
        }
        if(texScale_.y >0.f) {
            th = h / texScale_.y;
        }
        glTexCoord2f(0.0, 0.0); glVertex3f(p1.x, p1.y, p1.z);
        glTexCoord2f(0.0, tw); glVertex3f(p2.x, p2.y, p2.z);
        glTexCoord2f(th, tw); glVertex3f(p3.x, p3.y, p3.z);
        glTexCoord2f(th, 0.0); glVertex3f(p4.x, p4.y, p4.z);

        glTexCoord2f(0.0, 0.0); glVertex3f(p1.x, p1.y, p1.z);
        glTexCoord2f(th, 0.0); glVertex3f(p4.x, p4.y, p4.z);
        glTexCoord2f(th, tw); glVertex3f(p3.x, p3.y, p3.z);
        glTexCoord2f(0.0, tw); glVertex3f(p2.x, p2.y, p2.z);
    } glEnd();
    glFlush();
    
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

void Drawing::SetTextureScale(float w, float h)
{
    texScale_ = Point2D(w, h);
}

void Drawing::Quad(float w, float h)
{
    // in XZ: p1, p1 + (w, 0), p1 + (w, h), p1 + (0, h)
    // I have p1 and N
    Point3D p1 = Point3D(-w/2.f, 0, -h/2.f);
    Point3D p2 = Point3D(w/2.f, 0, -h/2.f);
    Point3D p3 = Point3D(w/2.f, 0, h/2.f);
    Point3D p4 = Point3D(-w/2.f, 0, h/2.f);

    Point3D OP = currentPoint_;
    
    glPushMatrix();
    glLineWidth(2);

    glTranslatef(currentPoint_.x, currentPoint_.y, -currentPoint_.z);
    glRotatef(r_.x, 1, 0, 0);
    glRotatef(r_.y, 0, 1, 0);
    glRotatef(r_.z, 0, 0, 1);

    glBegin(GL_QUADS); {
        glVertex3f(p1.x, p1.y, p1.z);
        glVertex3f(p2.x, p2.y, p2.z);
        glVertex3f(p3.x, p3.y, p3.z);
        glVertex3f(p4.x, p4.y, p4.z);

        glVertex3f(p1.x, p1.y, p1.z);
        glVertex3f(p4.x, p4.y, p4.z);
        glVertex3f(p3.x, p3.y, p3.z);
        glVertex3f(p2.x, p2.y, p2.z);
    } glEnd();

    glPopMatrix();
}

Point3D Drawing::UtilityHelpers::Translate(Point3D P, Point3D dP)
{
    return Point3D(P.x + dP.x, P.y + dP.y, P.z + dP.z);
}

Point3D Drawing::UtilityHelpers::Rotate(Point3D P, Point3D O, float rx, float ry)
{
    Point3D dP(P.x - O.x, P.y - O.y, P.z - O.z);
    return Translate(O, RotateDeltaVector(dP, rx, ry));
}

Point3D Drawing::UtilityHelpers::RotateDeltaVector(Point3D dP, float rx, float ry)
{
    return rotateDirectionVector(dP, rx, ry);
}

void Drawing::MoveCamera(Point3D O, float rx, float ry)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    setPerspective();

    rrx_ = -rx;

    //rotate around center point
    float x, y, z;
    x = +5.f * SIN(-rx) * COS(-ry);
    y = +5.f * SIN(-ry);
    z = +5.f * COS(-rx) * COS(-ry);
    gluLookAt(
            -O.x, O.y, O.z,
            -O.x + x, O.y + y, O.z + z,
            0, 1, 0);

	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity(); 
	glEnable(GL_DEPTH_TEST);
}

void Drawing::SetTexture(int i)
{
    if(i > textures_.size()) {
        tex_ = -1;
    } else {
        tex_ = i;
    }
}

int Drawing::LoadBitmapTexture(std::string const& path, int chromaKey)
{
    CBitmap bmp;
    if(!bmp.Load(path.c_str())) {
        std::cerr << "ERROR: failed to load " << path << std::endl;
    }
    uint8_t* buf(NULL);
    size_t nBuf;
    bmp.GetBits(buf, nBuf, /*R*/0x000000FF, /*G*/0x0000FF00, /*B*/0x00FF0000, /*A*/0xFF000000);
    buf = (uint8_t*)malloc(nBuf * sizeof(uint8_t));
    bmp.GetBits(buf, nBuf, /*R*/0x000000FF, /*G*/0x0000FF00, /*B*/0x00FF0000, /*A*/0xFF000000);

    int r = 0, g = 0, b = 0;
    if(chromaKey >= 0) {
        r = chromaKey % 256;
        g = chromaKey / 256 % 256;
        b = chromaKey / 256 / 256 % 256;
    }

    for(size_t i = 0; i < nBuf / 4; i++) {
        if(buf[4*i + 0] == r && buf[4*i + 1] == g && buf[4*i + 2] == b) {
            buf[4*i + 3] = 0;
        }
    }

    // do stuff
    GLuint tex(0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmp.GetWidth(), bmp.GetHeight(),
        0, GL_RGBA, GL_UNSIGNED_BYTE, buf);

    free(buf);

    textures_.push_back(tex);
    return textures_.size() - 1;
}
