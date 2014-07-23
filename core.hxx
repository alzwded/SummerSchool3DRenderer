#ifndef CORE_HXX
#define CORE_HXX

#include <stddef.h>
#include <vector>
#include <set>
#include <deque>
#include <string>

struct Point3D {
    float x, y, z;

    Point3D(float X = 0.f, float Y = 0.f, float Z = 0.f)
        : x(X), y(Y), z(Z)
    {}
};

struct Point2D {
    float x, y;

    Point2D(float X = 0.f, float Y = 0.f)
        : x(X), y(Y)
    {}
};

struct Beam {
    Point3D firstPoint, secondPoint;

    Beam(Point3D p1, Point3D p2)
        : firstPoint(p1), secondPoint(p2)
    {}

    typedef std::deque<Beam> vector;
};

struct Sensor {
    Point3D location;
    bool selected;
    enum SensorType {
        CENTRAL,
        ROUTER,
        SENSOR
    } type;
    float range;

    Sensor(Point3D p, SensorType t, float r)
        : location(p), type(t), range(r), selected(false)
    {}

    typedef std::deque<Sensor> vector;
};

struct Edge {
    Edge(Sensor const& n1, Sensor const& n2)
        : firstNode_(&n1), secondNode_(&n2)
    {}
    Sensor const& FirstNode() const { return *firstNode_; }
    Sensor const& SecondNode() const { return *secondNode_; }
private:
    Sensor const* firstNode_;
    Sensor const* secondNode_;
};

class AnimationData {
public:
    typedef std::vector<Beam> Beams_t;
    typedef std::set<Sensor const*> Sensors_t;
private:
    Beams_t connections_;
    Sensors_t animatedSensors_;
    Sensors_t emittingSensors_;
public:
    AnimationData& operator<<(Edge const& edge) {
        animatedSensors_.insert(&edge.FirstNode());
        animatedSensors_.insert(&edge.SecondNode());
        connections_.push_back(Beam(edge.FirstNode().location, edge.SecondNode().location));
        emittingSensors_.insert(&edge.FirstNode());
        return *this;
    }

    Beams_t const& Beams() const {
        return connections_;
    }

    Sensors_t const& Sensors() const {
        return animatedSensors_;
    }

    Sensors_t const& EmittingSensors() const {
        return emittingSensors_;
    }

    void Clear() {
        connections_.clear();
        animatedSensors_.clear();
        emittingSensors_.clear();
    }
};

struct Button {
    Point2D location;
    Point2D extent;
    std::string text;
    bool highlighted;
    void (*clicked)();

    Button(Point2D _location, Point2D _extent,
            std::string _text, void (*_clicked)())
        : location(_location), extent(_extent)
        , text(_text), clicked(_clicked)
        , highlighted(false)
    {}

    typedef std::deque<Button> vector;
};

#define Generic2D_Button(G) (((G).type == Generic2D::BUTTON) ? (G).payload.button : NULL )
#define Generic2D_Sensor(G) (((G).type == Generic2D::SENSOR) ? (G).payload.sensor : NULL )
class Generic2D {
    Point2D location;
    enum { 
        BUTTON,
        SENSOR
    } type;
    union {
        Button* button;
        Sensor* sensor;
    } payload;

    Generic2D(Point2D p, Button& b)
        : location(p), type(BUTTON)
    {
        payload.button = &b;
    }

    Generic2D(Point2D p, Sensor& s)
        : location(p), type(SENSOR)
    {
        payload.sensor = &s;
    }

    Button& AsButton() {
        if(type == BUTTON) {
            return *payload.button;
        } else {
            return *((Button*)NULL);
        }
    }

    Sensor& AsSensor() {
        if(type == SENSOR) {
            return *payload.sensor;
        } else {
            return *((Sensor*)NULL);
        }
    }
private:
    Generic2D();
};

#endif
