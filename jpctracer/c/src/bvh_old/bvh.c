#include "bvh.h"
#include "../bsdf.h"
#include "../utils.h"
#include "cglm/cglm.h"
#include "cglm/vec3.h"
#include "jpc_api.h"
#include "sorting/sort.h"
#include <assert.h>
#include <log/log.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//#define LOG_BUILDING
//#define LOG_TRAVERSAL

void triangles_create_bounds_old(const triangles_t* triangles,
                                 bounds3d_t*        bounds)
{
    printf("Create tri bounds\n");
    for (int i = 0; i < triangles->faces_count; i++)
    {
        const uint* vert_ids = triangles->verticies_ids[i];
        float*      v1 = triangles->verticies[vert_ids[0]];
        float*      v2 = triangles->verticies[vert_ids[1]];
        float*      v3 = triangles->verticies[vert_ids[2]];

        glm_vec3_maxv(v1, v2, bounds[i].max);
        glm_vec3_maxv(v3, bounds[i].max, bounds[i].max);

        glm_vec3_minv(v1, v2, bounds[i].min);
        glm_vec3_minv(v3, bounds[i].min, bounds[i].min);

#ifdef LOG_BUILDING
        printf("Build vert:");
        printf_arrayf(3, v1);
        printf_arrayf(3, v2);
        printf_arrayf(3, v3);
        printf("\n");

        printf("Bounds: ");
        printf_arrayf(6, (float*)&bounds[i]);
        printf("\n");
#endif
    }
}

void spheres_create_bounds_old(const spheres_t* spheres, bounds3d_t* bounds)
{
    for (int i = 0; i < spheres->count; i++)
    {
        glm_vec3_adds(spheres->positions[i],
                      -spheres->radii[i] - ERROR_THICKNESS,
                      bounds[i].min);
        glm_vec3_adds(spheres->positions[i],
                      spheres->radii[i] + ERROR_THICKNESS,
                      bounds[i].max);
    }
}

void triangles_get_centers_old(const triangles_t* triangles, vec3* centers)
{
    for (int i = 0; i < triangles->faces_count; i++)
    {

        const uint* vert_ids = triangles->verticies_ids[i];
        float*      v1 = triangles->verticies[vert_ids[0]];
        float*      v2 = triangles->verticies[vert_ids[1]];
        float*      v3 = triangles->verticies[vert_ids[2]];

        glm_vec3_add(v1, v2, centers[i]);
        glm_vec3_add(v3, centers[i], centers[i]);
        glm_vec3_scale(centers[i], 1. / 3., centers[i]);
    }
}

void spheres_get_centers_old(const spheres_t* spheres, vec3* centers)
{
    for (int i = 0; i < spheres->count; i++)
        glm_vec3_copy(spheres->positions[i], centers[i]);
}

void bounds3d_t_merge_old(bounds3d_t* bounds, uint n, bounds3d_t* dst)
{
    memcpy(dst, bounds, sizeof(bounds3d_t));
    for (int i = 1; i < n; i++)
    {
        glm_aabb_merge((vec3*)(bounds + i), (vec3*)dst, (vec3*)dst);
    }
}

bvh_tree_t* bvhtree_alloc(uint n)
{
    bvh_tree_t* tree = malloc(sizeof(bvh_tree_t));
    tree->n = n;
    tree->node_bounds = malloc(sizeof(bounds3d_t) * (n - 1));
    tree->nodes = malloc(sizeof(bvh_node_t) * (n - 1));
    tree->shape_bounds = malloc(sizeof(bounds3d_t) * n);
    return tree;
}

void bvhtree_free(bvh_tree_t* tree)
{
    free(tree->node_bounds);
    free(tree->nodes);
    free(tree->shape_bounds);
    free(tree);
}

bvh_tree_t* bvhtree_build_triangles(triangles_t tris)
{
    printf("bvhtree build triangles\n");
    vec3*       centers = malloc(sizeof(vec3) * tris.faces_count);
    uint32_t*   permutations = malloc(sizeof(uint32_t) * tris.faces_count);
    bvh_tree_t* tree = bvhtree_alloc(tris.faces_count);

    triangles_create_bounds_old(&tris, tree->shape_bounds);
    triangles_get_centers_old(&tris, centers);

    uint start = 0;
    for (int i = 0; i < tris.mesh_count; i++)
    {
        uint end = tris.mesh_end_idx[i];

        int        n = end - start;
        bvh_tree_t temp_tree = {
            .n = n,
            .shape_bounds = tree->shape_bounds + start,
            .nodes = tree->nodes + start,
            .node_bounds = tree->node_bounds + start,
        };
        uint* temp_perm = permutations + start;

        lbvh_build_old(temp_tree, centers + start, temp_perm);
        temp_perm[0] = 0;

        apply_permutation(
            temp_perm, tris.verticies_ids + start, n, sizeof(uint3));

        apply_permutation(
            temp_perm, tris.normals_ids + start, n, sizeof(uint3));
        apply_permutation(temp_perm, tris.uvs_ids + start, n, sizeof(uint2));
        apply_permutation(
            temp_perm, tris.material_slots + start, n, sizeof(uint));

        uint start = end;
    }

    free(centers);
    free(permutations);

    return tree;
}

bvh_tree_t* bvhtree_build_spheres(spheres_t sphs)
{

    vec3*       centers = malloc(sizeof(vec3) * sphs.count);
    uint*       permutations = malloc(sizeof(uint) * sphs.count);
    bvh_tree_t* tree = bvhtree_alloc(sphs.count);

    spheres_create_bounds_old(&sphs, tree->shape_bounds);
    spheres_get_centers_old(&sphs, centers);

    uint start = 0;
    for (int i = 0; i < sphs.mesh_count; i++)
    {
        uint end = sphs.mesh_end_idx[i];

        int        n = end - start;
        bvh_tree_t temp_tree = {
            .n = n,
            .shape_bounds = tree->shape_bounds + start,
            .nodes = tree->nodes + start,
            .node_bounds = tree->node_bounds + start,
        };
        uint* temp_perm = permutations + start;

        lbvh_build_old(temp_tree, centers + start, temp_perm);

        apply_permutation(temp_perm, sphs.positions + start, n, sizeof(float3));
        apply_permutation(temp_perm, sphs.radii + start, n, sizeof(float));
        apply_permutation(
            temp_perm, sphs.material_slot_id + start, n, sizeof(uint));

        start = end;
    }
    free(centers);
    free(permutations);
    return tree;
}

bvh_tree_t* bvhtree_build_instances(geometries_t* geometries)
{
    printf("Inst Vert ids:");
    if (geometries->triangles.verticies_count > 0)
    {
        printf_arrayui(3, *geometries->triangles.verticies_ids);
    }
    else
    {
        printf("Null");
    }
    printf("\n");
    bvh_tree_t* tree = bvhtree_alloc(geometries->instances_count);
    vec3*       centers = malloc(sizeof(vec3) * geometries->instances_count);
    uint* permutation = malloc(sizeof(uint) * geometries->instances_count);

    for (int i = 0; i < geometries->instances_count; i++)
    {

        uint        mesh_id = geometries->instances[i].mesh_id;
        bvh_tree_t* mesh_tree;
        uint        mesh_start_i;
        uint        mesh_end_i;

        switch (geometries->instances[i].type)
        {
        case JPC_TRIANGLE:
            mesh_start_i = mesh_id > 0
                               ? geometries->triangles.mesh_end_idx[mesh_id - 1]
                               : 0;
            mesh_end_i = geometries->triangles.mesh_end_idx[mesh_id];
            mesh_tree = geometries->bvhtree_triangles;
            break;
        case JPC_SPHERE:
            mesh_start_i = mesh_id > 0
                               ? geometries->spheres.mesh_end_idx[mesh_id - 1]
                               : 0;
            mesh_end_i = geometries->spheres.mesh_end_idx[mesh_id];
            mesh_tree = geometries->bvhtree_spheres;
            break;
        }
        uint mesh_n = mesh_end_i - mesh_start_i;

        bounds3d_t bounds;
        if (mesh_n > 1)
            bounds = mesh_tree->node_bounds[mesh_start_i];
        else if (mesh_n == 1)
            bounds = mesh_tree->shape_bounds[mesh_start_i];

        mat4 trans;

        glm_mat4_ucopy(geometries->instances[i].transformations, trans);
        glm_aabb_transform(
            (vec3*)&bounds, trans, (vec3*)&tree->shape_bounds[i]);

#ifdef LOG_BUILDING
        printf("type: %d\n", geometries->instances[i].type);
        printf("bounds: ");
        printf_arrayf(6, (float*)&bounds);
        printf("\ntrans: ");
        printf_arrayf(16, (float*)&trans);
        printf("\nshape_bounds: ");
        printf_arrayf(6, (float*)&tree->shape_bounds[i]);
        printf("\n");
        printf("mesh_range: %d,%d\n", mesh_start_i, mesh_end_i);
#endif
    }

    for (int i = 0; i < geometries->instances_count; i++)
    {
        glm_aabb_center((vec3*)&tree->shape_bounds[i], centers[i]);
    }

    lbvh_build_old(*tree, centers, permutation);
    apply_permutation(permutation,
                      geometries->instances,
                      geometries->instances_count,
                      sizeof(instance_t));

    free(centers);
    free(permutation);
    return tree;
}

bool bounds3d_intersect(bounds3d_t* bound,
                        vec3        origin,
                        vec3        inverse_direction,
                        float       max_distance)
{
#ifdef LOG_TRAVERSAL
    printf("bounds3d: ");
    printf_arrayf(6, (float*)bound);
    printf("\n");
    printf("ray orig: ");
    printf_arrayf(3, origin);
    printf(" inv dir: ");
    printf_arrayf(3, inverse_direction);
    printf("\n");
    printf("Distance %f\n", max_distance);
#endif

    float x_min = (bound->min[0] - origin[0]);
    float x_max = (bound->max[0] - origin[0]);

    float y_min = (bound->min[1] - origin[1]);
    float y_max = (bound->max[1] - origin[1]);

    float z_min = (bound->min[2] - origin[2]);
    float z_max = (bound->max[2] - origin[2]);

    float temp;
    if (inverse_direction[0] < 0)
    {
        temp = x_min;
        x_min = x_max;
        x_max = temp;
    }
    if (inverse_direction[1] < 0)
    {
        temp = y_min;
        y_min = y_max;
        y_max = temp;
    }
    if (inverse_direction[2] < 0)
    {
        temp = z_min;
        z_min = z_max;
        z_max = temp;
    }

    x_min *= inverse_direction[0];
    x_max *= inverse_direction[0];
    y_min *= inverse_direction[1];
    y_max *= inverse_direction[1];
    z_min *= inverse_direction[2];
    z_max *= inverse_direction[2];

    float t_min = MAX(x_min, MAX(y_min, z_min));

    float t_max = MIN(x_max, MIN(y_max, z_max));
#ifdef LOG_TRAVERSAL
    printf("t_min: %f\n", t_min);
    printf("distance: %f\n", distance);
#endif

    if (t_min > t_max || t_max <= 0 || t_min > max_distance)
    {
#ifdef LOG_TRAVERSAL
        printf("Inter false\n");
#endif
        return false;
    }
#ifdef LOG_TRAVERSAL
    printf("Inter true\n");
#endif
    return true;
}
// updates the distance of the ray
bool traverse_bvh(const bvh_tree_t* tree,
                  ray_t*            ray,
                  intersect_f       intersect,
                  void*             param,
                  bool              shadow_test)
{
    if (tree->n == 0)
    {
        log_warn("useless call to traverse bvh");
        return false;
    }
    vec3 inv_dir = {
        1. / ray->direction[0],
        1. / ray->direction[1],
        1. / ray->direction[2],
    };

    int    nodes_to_visit[64];
    int    to_visit = 0;
    int    current_idx = 0;
    float* origin = ray->origin;
    bool   is_inter = false;

    if (tree->n == 1)
    {
        if (bounds3d_intersect(
                &tree->shape_bounds[0], origin, inv_dir, ray->clip_end))
        {
            if (intersect(0, ray, param))
            {
                return true;
            }
            return false;
        }
        return false;
    }
    do
    {
        if (current_idx == 10)
        {
            int test = 0;
        }
        if (bounds3d_intersect(&tree->node_bounds[current_idx],
                               origin,
                               inv_dir,
                               ray->clip_end))
        {
#ifdef LOG_TRAVERSAL
            printf("Node intersect %d\n", current_idx);
#endif
            /*
                        for (int i = 0; i < to_visit; i++)
                        {
                            if (nodes_to_visit[i] == current_idx ||
               nodes_to_visit[i] == 7)
                            {
                                log_warn("Dupplicate");
                            }
                        }
                        if (to_visit > 64)
                        {
                            log_warn("to large");
                        }*/
            bvh_node_t node = tree->nodes[current_idx];
            if (node.first_idx == node.split_idx)
            {
#ifdef LOG_TRAVERSAL
                printf("Left Child %d\n", node.first_idx);
#endif

                if (bounds3d_intersect(&tree->shape_bounds[node.first_idx],
                                       origin,
                                       inv_dir,
                                       ray->clip_end))
                {
#ifdef LOG_TRAVERSAL
                    printf("Intersect Child\n");
#endif
                    bool temp_inter = intersect(node.first_idx, ray, param);
                    is_inter = is_inter || temp_inter;
                    if (is_inter && shadow_test)
                        return true;
                }
            }
            else
            {
                nodes_to_visit[to_visit++] = node.split_idx;
            }

            if (node.last_idx == node.split_idx + 1)
            {
#ifdef LOG_TRAVERSAL
                printf("Right Child %d\n", node.last_idx);
#endif
                if (bounds3d_intersect(&tree->shape_bounds[node.last_idx],
                                       origin,
                                       inv_dir,
                                       ray->clip_end))
                {
#ifdef LOG_TRAVERSAL
                    printf("Intersect Child\n");
#endif
                    bool temp_inter = intersect(node.last_idx, ray, param);
                    is_inter = is_inter || temp_inter;

                    if (is_inter && shadow_test)
                        return true;
                }
            }
            else
            {
                nodes_to_visit[to_visit++] = node.split_idx + 1;
            }
        }

        current_idx = nodes_to_visit[--to_visit];
    } while (to_visit >= 0);
    // printf("traverse_bvh end\n");
    return is_inter;
}

bool triangle_intersect_old(ray_t* ray, vec3 v1, vec3 v2, vec3 v3, float* uv)
{
#ifdef LOG_TRAVERSAL
    printf("triangle intersect\n");
    printf("ray orig: ");
    printf_arrayf(3, ray->origin);
    printf(" dir: ");
    printf_arrayf(3, ray->direction);
    printf(" clip_end: %f\n", ray->clip_end);

    printf("Verts:");
    printf_arrayf(3, v1);
    printf_arrayf(3, v2);
    printf_arrayf(3, v3);
    printf("\n");
#endif

    float epsilion = 1e-6;
    vec3  support_vec_1, support_vec_2;
    vec3  point_dir, diff_origin_position, cross_op_s1;

    glm_vec3_sub(v2, v1, support_vec_1);
    glm_vec3_sub(v3, v1, support_vec_2);

    glm_vec3_cross(ray->direction, support_vec_2, point_dir);
    float determinante = glm_vec3_dot(support_vec_1, point_dir);
    if (determinante < epsilion)
        return false;

    float inv_det = 1. / determinante;

    glm_vec3_sub(ray->origin, v1, diff_origin_position);

    float u = glm_vec3_dot(diff_origin_position, point_dir) * inv_det;
    glm_vec3_cross(diff_origin_position, support_vec_1, cross_op_s1);
    float v = glm_vec3_dot(ray->direction, cross_op_s1) * inv_det;

    float intersection_point_distance
        = glm_vec3_dot(support_vec_2, cross_op_s1) * inv_det;

    if (u < -ERROR_THICKNESS || u > 1 + ERROR_THICKNESS || v < -ERROR_THICKNESS
        || u + v > 1 + ERROR_THICKNESS || intersection_point_distance < 0
        || intersection_point_distance > ray->clip_end)
        return false;

    ray->clip_end = intersection_point_distance;
    uv[0] = u;
    uv[1] = v;

    return true;
}

struct mesh_inter_params
{
    union {
        const triangles_t* tris;
        const spheres_t*   spheres;
    };
    int    id;
    int    start_offset;
    float2 uv;
};

bool triangle_intersect_program(int tri_id, ray_t* ray, void* _p)
{
    struct mesh_inter_params* p = _p;
    tri_id = tri_id + p->start_offset;

    uint*   v_ids = p->tris->verticies_ids[tri_id];
    float3* verts = p->tris->verticies;
    if (triangle_intersect_old(
            ray, verts[v_ids[0]], verts[v_ids[1]], verts[v_ids[2]], p->uv))
    {
        p->id = tri_id;
        return true;
    }
    return false;
}

void interpolate3d(vec3 v1, vec3 v2, vec3 v3, float u, float v, vec3 dst)
{

    // u * v2 + v * v3 + (1 - u - v) * v1;
    vec3 temp;
    glm_vec3_scale(v2, u, dst);
    glm_vec3_scale(v3, v, temp);
    glm_vec3_add(temp, dst, dst);
    glm_vec3_scale(v1, (1 - u - v), temp);
    glm_vec3_add(temp, dst, dst);
}

void interpolate2d(vec2 v1, vec2 v2, vec2 v3, float u, float v, vec2 dst)
{

    // u * v2 + v * v2 + (1 - u - v) * v1;
    vec2 temp;
    glm_vec2_scale(v2, u, dst);
    glm_vec2_scale(v3, v, temp);
    glm_vec2_add(temp, dst, dst);
    glm_vec2_scale(v1, (1 - u - v), temp);
    glm_vec2_add(temp, dst, dst);
}

void triange_fill_hitpoint(hit_point_t*              hit,
                           ray_t                     ray,
                           uint*                     mat_slots_bindings,
                           struct mesh_inter_params* p)
{
    uint* uv_ids = p->tris->uvs_ids[p->id];

    float* uv1 = p->tris->uvs[uv_ids[0]];
    float* uv2 = p->tris->uvs[uv_ids[1]];
    float* uv3 = p->tris->uvs[uv_ids[2]];

    uint*  norm_ids = p->tris->normals_ids[p->id];
    float* norm1 = p->tris->normals[norm_ids[0]];
    float* norm2 = p->tris->normals[norm_ids[1]];
    float* norm3 = p->tris->normals[norm_ids[2]];

    glm_vec3_scale(ray.direction, ray.clip_end, hit->location);
    glm_vec3_add(hit->location, ray.origin, hit->location);

    interpolate2d(uv1, uv2, uv3, p->uv[0], p->uv[1], hit->uv);
    interpolate3d(norm1, norm2, norm3, p->uv[0], p->uv[1], hit->normal);

    uint mat_slot = p->tris->material_slots[p->id];
    hit->material_id = mat_slots_bindings[mat_slot];
}

bool solveQuadratic(const float a,
                    const float b,
                    const float c,
                    float*      x0,
                    float*      x1)
{
    float discr = b * b - 4 * a * c;
    if (discr < 0)
        return false;
    else if (discr == 0)
        *x0 = *x1 = -0.5 * b / a;
    else
    {
        float q = (b > 0) ? -0.5 * (b + sqrt(discr)) : -0.5 * (b - sqrt(discr));
        *x0 = q / a;
        *x1 = c / q;
    }
    if (*x0 > *x1)
    {
        float temp = *x0;
        *x0 = *x1;
        *x1 = temp;
    }

    return true;
}

// ray.direction should be normalized
bool sphere_intersect_program(int sphs_id, ray_t* ray, void* _p)
{

    struct mesh_inter_params* p = _p;
    sphs_id += p->start_offset;
    if (sphs_id == 12168)
    {
        int test = 0;
    }
    float* center = p->spheres->positions[sphs_id];

    float radius = p->spheres->radii[sphs_id];

#ifdef LOG_TRAVERSAL
    printf("sphere intersect\n");
    printf("center: ");
    printf_arrayf(3, center);
    printf(" radius: %f\n", radius);
#endif

    float radius2 = radius * radius;

    float t, t0, t1; // soluations for t if the ray intersects
    // geometric solution

    vec3 L;
    glm_vec3_sub(ray->origin, center, L);

    float a = glm_vec3_norm2(ray->direction);
    float b = 2 * glm_vec3_dot(ray->direction, L);
    float c = glm_vec3_norm2(L) - radius2;
    if (!solveQuadratic(a, b, c, &t0, &t1))
    {
#ifdef LOG_TRAVERSAL
        printf("inter false\n");
#endif
        return false;
    }

    if (t0 > t1)
    {
        float temp = t0;
        t0 = t1;
        t1 = temp;
    }

    if (t0 < 0)
    {
        t0 = t1; // if t0 is negative, let's use t1 instead
        if (t0 < 0)
        {

#ifdef LOG_TRAVERSAL
            printf("inter false\n");
#endif
            return false;
        }
    }

    t = t0 - ERROR_THICKNESS;

#ifdef LOG_TRAVERSAL
    printf("t: %f, max_distance: %f norm: %f\n",
           t,
           ray->clip_end,
           glm_vec3_norm(ray->direction));
#endif
    if (t > ray->clip_end)
    {
#ifdef LOG_TRAVERSAL
        printf("inter false\n");
#endif
        return false;
    }

    ray->clip_end = t;
    p->id = sphs_id;
#ifdef LOG_TRAVERSAL
    printf("inter true\n");
#endif
    return true;
}

void sphere_fill_hitpoint(hit_point_t*              hit,
                          ray_t                     ray,
                          uint*                     mat_slots_bindings,
                          struct mesh_inter_params* p)
{
    glm_vec3_normalize_to(ray.direction, hit->location);
    glm_vec3_scale(hit->location, ray.clip_end, hit->location);
    glm_vec3_add(hit->location, ray.origin, hit->location);

    float* center = p->spheres->positions[p->id];
    glm_vec3_sub(hit->location, center, hit->normal);
    glm_vec3_normalize(hit->normal);

    hit->uv[0] = 0; // hit->normal[0];
    hit->uv[1] = 0; // hit->normal[1];

    uint slot_id = p->spheres->material_slot_id[p->id];
    hit->material_id = mat_slots_bindings[slot_id];
}

struct inst_inter_param
{
    const geometries_t* geom;
    hit_point_t*        hit;
    bool                shadow_test;
};

bool instances_intersect_program(int inst_id, ray_t* ray, void* param)
{
#ifdef LOG_TRAVERSAL
    printf("intersect instance:\n");
#endif
    if (inst_id == 72)
    {
        int tes = 0;
    }
    struct inst_inter_param* p = param;

    instance_t inst = p->geom->instances[inst_id];
    vec4       temp_orig, origin, temp_dir, direction;
    mat4       trans;
    mat4       inv_trans;

    glm_vec3_copy(ray->direction, temp_dir);
    glm_vec3_copy(ray->origin, temp_orig);

    temp_dir[3] = 0; // no translataion
    temp_orig[3] = 1;

    glm_mat4_ucopy(inst.transformations, trans);
    glm_mat4_inv(trans, inv_trans);

    glm_mat4_mulv(inv_trans, temp_orig, origin);
    glm_mat4_mulv(inv_trans, temp_dir, direction);

    ray_t ray_local;
    glm_vec3_copy(direction, ray_local.direction);
    glm_vec3_copy(origin, ray_local.origin);

    ray_local.clip_end = glm_vec3_norm(direction);
    glm_vec3_scale(direction, 1. / ray_local.clip_end, ray_local.direction);
#ifdef LOG_TRAVERSAL
    printf("dist local: %f dist max: %f\n", ray_local.clip_end, ray->clip_end);
#endif
    ray_local.clip_end *= ray->clip_end;

    uint*                    mesh_ends;
    bvh_tree_t*              bvh_of_mesh_type;
    struct mesh_inter_params inter_params;

    void (*fill_hit_point)(hit_point_t * hit,
                           ray_t ray,
                           uint * mat_slots_bindings,
                           struct mesh_inter_params * p);
    intersect_f inter_prog;

    switch (inst.type)
    {
    case JPC_TRIANGLE:
        mesh_ends = p->geom->triangles.mesh_end_idx;
        bvh_of_mesh_type = p->geom->bvhtree_triangles;
        inter_params.tris = &p->geom->triangles;
        inter_prog = &triangle_intersect_program;
        fill_hit_point = &triange_fill_hitpoint;
        break;

    case JPC_SPHERE:

        mesh_ends = p->geom->spheres.mesh_end_idx;
        bvh_of_mesh_type = p->geom->bvhtree_spheres;
        inter_params.spheres = &p->geom->spheres;
        inter_prog = &sphere_intersect_program;
        fill_hit_point = &sphere_fill_hitpoint;

        break;

    default:
        assert(false);
    }

    uint mesh_start, mesh_end;
    if (inst.mesh_id > 0)
    {
        mesh_start = mesh_ends[inst.mesh_id - 1];
    }
    else
    {
        mesh_start = 0;
    }
    mesh_end = mesh_ends[inst.mesh_id];

    bvh_tree_t local_bvh = {
        .n = mesh_end - mesh_start,
        .node_bounds = bvh_of_mesh_type->node_bounds + mesh_start,
        .nodes = bvh_of_mesh_type->nodes + mesh_start,
        .shape_bounds = bvh_of_mesh_type->shape_bounds + mesh_start,
    };

    uint* mat_slot_bindings
        = &p->geom->material_slots[inst.material_slot_start];

    inter_params.start_offset = mesh_start;

    // printf("Inst traverse_bvh\n");
    // printf("local bvh: n=%d\n", local_bvh.n);
    if (traverse_bvh(
            &local_bvh, &ray_local, inter_prog, &inter_params, p->shadow_test))
    {
        if (p->shadow_test)
            return true;

        fill_hit_point(p->hit, ray_local, mat_slot_bindings, &inter_params);

        glm_vec3_scale(direction, ray_local.clip_end, direction);

        glm_mat4_mulv(trans, direction, direction);

        ray->clip_end = glm_vec3_norm(direction);
        p->hit->location[3] = 1;
        vec4 tmp;
        glm_mat4_mulv(trans, p->hit->location, tmp);
        glm_vec4_copy(tmp, p->hit->location);
        vec3 tmp2;
        glm_mat4_mulv3(trans, p->hit->normal, 0, tmp2);
        glm_vec3_normalize(tmp2);
        glm_vec3_copy(tmp2, p->hit->normal);

        return true;
    }
    return false;
}

bool ray_intersect(const geometries_t* geometries,
                   ray_t*              ray,
                   hit_point_t*        out_hitpoint)
{
#ifdef LOG_TRAVERSAL
    printf("ray intersect\n");
#endif

    glm_vec3_copy(ray->origin, out_hitpoint->location);
    struct inst_inter_param param = {
        .geom = geometries,
        .hit = out_hitpoint,
        .shadow_test = false,
    };

    // printf("Ray traverse_bvh\n");
    bool is_hit = traverse_bvh(geometries->bvhtree_instances,
                               ray,
                               instances_intersect_program,
                               &param,
                               false);

    if (is_hit)
    {
        glm_vec3_normalize(out_hitpoint->normal);
        // offset location by normal
        vec3 temp;
        glm_vec3_scale(out_hitpoint->normal, ERROR_THICKNESS * 2, temp);
        glm_vec3_add(out_hitpoint->location, temp, out_hitpoint->location);
    }
    return is_hit;
}

uint64_t rays_shadow_test(const geometries_t* geometries,
                          vec3*               dirs,
                          float*              distances,
                          vec3                origin,
                          uint                n)
{

    struct inst_inter_param param = {
        .geom = geometries,
        .hit = NULL,
        .shadow_test = true,
    };

    ray_t ray;
    glm_vec3_copy(origin, ray.origin);

    assert(n < 64);

    uint64_t result = 0;
    for (int i = 0; i < n; i++)
    {
        glm_vec3_copy(dirs[i], ray.direction);
        ray.clip_end = distances[i];

        // printf("Shadow traverse_bvh\n");
        if (traverse_bvh(geometries->bvhtree_instances,
                         &ray,
                         instances_intersect_program,
                         &param,
                         true))
        {
            result |= 1 << i;
            distances[i] = ray.clip_end;
        }
    }
    return result;
}
