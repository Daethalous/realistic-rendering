#ifndef PATH_TRACER_H
#define PATH_TRACER_H

#include <cmath>
#include <cstring>
#include <iostream>
#include <string>
#include <stdlib.h>

#include "camera.hpp"
#include "group.hpp"
#include "hit.hpp"
#include "image.hpp"
#include "light.hpp"
#include "ray.hpp"
#include "scene_parser.hpp"
#include "utils.hpp"
using namespace std;

static Vector3f ptColor(Ray ray, const Scene &scene, int depth)
{
    Group *group = scene.getGroup();
    depth--;
    Vector3f color(0, 0, 0);

    // 判断camRay是否和场景有交点,返回最近交点的数据,存储在hit中.
    Hit hit;
    if (!group->intersect(ray, hit))
    {
        color += scene.getBackgroundColor();
        return color;
    }

    // Path Tracing
    ray.origin += ray.direction * hit.t;
    Material *material = hit.material;
    Vector3f refColor(hit.color), N(hit.normal);

    float prob = 0.6;
    float num = RND2; // 随机获取一个0-1的数值
    if (num > prob && TRACE_DEPTH - depth > 5)
    {
        // 1-prob 的概率不发射光线
        return refColor * hit.material->emission;
    }

    // cf = cf * refColor;
    float type = RND2;
    if (material->type.x() > type)
    { // diffuse
        // 使用BRDF函数采样漫反射方向和颜色
        Vector3f incomingDir = sampleDiffuseDirection(N); // 使用BRDF函数采样漫反射方向
        Ray newRay(ray.origin, incomingDir);

        // 计算漫反射颜色
        color = material->emission * refColor + refColor * ptColor(newRay, scene, depth); // BRDF函数的值在漫反射情况下是常数，所以除以PI可以省略

        return color;
    }
    else if (material->type.x() + material->type.y() > type)
    { // specular
        float cost = Vector3f::dot(ray.direction, N);
        ray.direction = (ray.direction - N * (cost * 2)).normalized();
        color = material->emission * refColor + refColor * ptColor(ray, scene, depth);
        return color;
    }
    else
    { // refraction
        float n = material->refr;
        float R0 = ((1.0 - n) * (1.0 - n)) / ((1.0 + n) * (1.0 + n));
        if (Vector3f::dot(N, ray.direction) > 0)
        { // inside the medium
            N.negate();
            n = 1 / n;
        }
        n = 1 / n;
        float cost1 = -Vector3f::dot(N, ray.direction); // cosine theta_1
        float cost2 =
            1.0 - n * n * (1.0 - cost1 * cost1); // cosine theta_2
        float Rprob = R0 + (1.0 - R0) * pow(1.0 - cost1,
                                            5.0); // Schlick-approximation
        if (cost2 > 0 && RND2 > Rprob)
        { // refraction direction
            ray.direction =
                ((ray.direction * n) + (N * (n * cost1 - sqrt(cost2))))
                    .normalized();
        }
        else
        { // reflection direction
            ray.direction = (ray.direction + N * (cost1 * 2));
        }
        color = material->emission * refColor + refColor * ptColor(ray, scene, depth);
        return color;
    }
    return color;
}

class PathTracer
{
public:
    const Scene &scene;
    int samps = 10;
    const char *fout;
    PathTracer(const Scene &scene,
               const char *fout)
        : scene(scene), fout(fout)
    {
    }

    void render()
    {
        Camera *camera = scene.getCamera();
        int w = camera->getWidth(), h = camera->getHeight();
        cout << "Width: " << w << " Height: " << h << endl;
        Image outImg(w, h);
        time_t start = time(NULL);
#pragma omp parallel for schedule(dynamic, 1) // OpenMP
        for (int y = 0; y < h; ++y)
        {
            float elapsed = (time(NULL) - start), progress = (1. + y) / h;
            fprintf(stderr, "\rRendering (%d spp) %5.2f%% Time: %.2f/%.2f sec",
                    samps, progress * 100., elapsed, elapsed / progress);
            for (int x = 0; x < w; ++x)
            {
                Vector3f color = Vector3f::ZERO;
                for (int s = 0; s < samps; ++s)
                {
                    Ray camRay = camera->generateRay(Vector2f(x + RND2, y + RND2));
                    color += ptColor(camRay, scene, TRACE_DEPTH);
                }
                outImg.SetPixel(x, y, color / samps);
            }
        }
        outImg.SaveBMP(fout);
    }
};

#endif // !PATH_TRACER_H