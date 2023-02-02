#include "rayRC.h"

bool engine::RC::hit(const ray& r, float& near_t, const glm::vec3& V1, const glm::vec3& V2, const glm::vec3& V3)
{

    glm::vec3 v0v1{ V2 - V1 };
    glm::vec3 v0v2{ V3 - V1 };
    glm::vec3 pvec{ glm::cross(r.direction(), v0v2) };
    float det{ glm::dot(v0v1, pvec) };

    //if (det < FLT_EPSILON) return false; // face culling

    if (std::fabs(det) < FLT_EPSILON) //ray parralel to triangle
        return false;

    float invDet{ 1 / det };

    glm::vec3 tvec{ r.origin() - V1 };
    float u{ glm::dot(tvec,pvec) * invDet };
    if (u < 0 || u > 1)
        return false;

    glm::vec3 qvec{ glm::cross(tvec, v0v1) };
    float v{ invDet * glm::dot(r.direction(), qvec) };
    if (v < 0 || u + v > 1)
        return false;

    float t{ invDet * glm::dot(v0v2, qvec) };
    if (t < near_t && t > 0)
    {
        near_t = t;
        return true;
    }

    return false;

}
