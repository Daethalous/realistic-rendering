#ifndef SPHERE_H
#define SPHERE_H

#include <vecmath.h>

#include <cmath>
#include "utils.hpp"
#include "object3d.hpp"

class Sphere : public Object3D {
   public:
    Sphere(const Vector3f &center, float radius, Material *material)
        : Object3D(material), cen(center), radius(radius) {
        //
    }
    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h) override {
        Vector3f o(r.getOrigin()), dir(r.getDirection());
        Vector3f OC(cen - o);
        float b = -Vector3f::dot(OC, dir);
        float c = OC.squaredLength() - radius * radius;
        float delta = b * b - c;
        if (delta <= 0) return false;
        float sqrt_delta = sqrtf(delta);
        float t1 = (-b - sqrt_delta), t2 = (-b + sqrt_delta);
        float t;
        if (t1 <= h.getT() && t1 >= 0)
            t = t1;
        else if (t2 <= h.getT() && t2 >= 0)
            t = t2;
        else
            return false;
        Vector3f OP(o + dir * t - cen);
        Vector3f normal = OP.normalized();
        float u = 0.5 + atan2(normal.x(), normal.z()) / (2 * M_PI),
              v = 0.5 - asin(normal.y()) / M_PI;
        h.set(t, material, getNormal(normal, OP, u, v),
              material->getColor(u, v), o + dir * t);
        return true;
    }

    Vector3f getNormal(const Vector3f &n, const Vector3f &p, float u, float v) {
        return n;
    }

    Ray randomRay(int axis=-1, long long int seed=0) const override {
        float u = 2*random(axis, seed) - 1, v = 2*random(axis, seed) - 1;
        float r2 =u * u + v * v;
        while(r2>=1) {
            ++seed;
            u = 2*random(axis, seed) - 1;
            v = 2*random(axis, seed) - 1;
            r2 = u * u + v * v;
        }
        Vector3f dir(2*u*sqrtf(1-r2), 2*v*sqrt(1-r2),1-2*r2);
        dir.normalize();
        return Ray(cen+radius*dir, dir);
    }

    Vector3f min() const override { return cen - Vector3f(radius, radius, radius); }
    Vector3f max() const override { return cen + Vector3f(radius, radius, radius); }
    Vector3f center() const override { return cen; }
    vector<Object3D *> getFaces() override { return {(Object3D *)this}; }

   protected:
    Vector3f cen;
    float radius;
};

#endif