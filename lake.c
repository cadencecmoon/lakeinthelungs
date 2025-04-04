#define A_MOONLIT_WALK_MAIN
#include <amw/a_moonlit_walk.h>

/* variables for a sphere of 100 faces */
#define SLICES 10
#define STACKS 7
#define VERTICES_COUNT (2 + (STACKS-2) * SLICES)
#define INDICES_COUNT (100 * 3)

/* The 3D data is hardcoded for now so i don't have to mess with reading from the filesystem,
 * preparing models with Blender and choosing a (or defining my own) 3D format for the scene, 
 * meshes, materials, etc. It's raw, and it's fine enough */

struct vertex {
    vec3 pos;
    vec3 normal;
    vec2 uv;
};

static void construct_sphere(u32 slices, u32 stacks, struct vertex *vertices, u16 *indices)
{
    assert_debug(aria_floorf(2.7f) == 2.0f);
    assert_debug(aria_floorf(-2.7f) == -3.0f);
    assert_debug(aria_floorf(0.9f) == 0.0f);
    assert_debug(aria_floorf(-0.1f) == -1.0f);
    assert_debug(aria_floorf(1e20f) == 1e20f);

    /* north pole */
    vertices[0] = (struct vertex){
        .pos = {0.0f, 0.0f, 1.0f},
        .normal = {0.0f, 0.0f, 1.0f},
        .uv = {0.5f, 0.0f},
    };

    /* south pole */
    vertices[VERTICES_COUNT - 1] = (struct vertex){
        .pos = {0.0f, 0.0f, -1.0f},
        .normal = {0.0f, 0.0f, -1.0f},
        .uv = {0.5f, 1.0f},
    };

    /* generate vertices */
    for (u32 i = 1; i < stacks-1; i++) {
        const f32 phi = ARIA_PIf * i / (stacks-1);
        const f32 cos_phi = aria_cosf(phi);
        const f32 sin_phi = aria_sinf(phi);

        for (u32 j = 0; j < slices; j++) {
            const f32 theta = ARIA_TAUf * j / slices;
            const f32 cos_theta = aria_cosf(theta);
            const f32 sin_theta = aria_sinf(theta);

            const u32 index = 1 + (i-1) * slices + j;

            vertices[index].pos[0] = sin_phi * cos_theta;
            vertices[index].pos[1] = sin_phi * sin_theta;
            vertices[index].pos[2] = cos_phi;

            vertices[index].normal[0] = vertices[index].pos[0];
            vertices[index].normal[1] = vertices[index].pos[1];
            vertices[index].normal[2] = vertices[index].pos[2];
            
            vertices[index].uv[0] = (f32)j / (f32)slices;
            vertices[index].uv[1] = (f32)i / (f32)(stacks-1);
        }
    }

    /* generate indices */
    u32 index = 0;

    /* top */
    for (u32 i = 0; i < slices; i++) {
        indices[index++] = 0;
        indices[index++] = 1 + i;
        indices[index++] = 1 + (i+1) % slices;
    }

    /* middle quads */
    for (u32 i = 0; i < stacks-3; i++) {
        const u32 current_start = 1 + i * slices;
        const u32 next_start = 1 + (i+1) * slices;

        for (u32 j = 0; j < slices; j++) {
            const u32 j_next = (j+1) % slices;

            indices[index++] = current_start + j;
            indices[index++] = current_start + j_next;
            indices[index++] = next_start + j_next;

            indices[index++] = current_start + j;
            indices[index++] = next_start + j_next;
            indices[index++] = next_start + j;
        }
    }

    /* bottom */
    const u32 south_pole = 1 + (stacks-2) * slices; /* VERTICES_COUNT - 1 */
    const u32 last_ring_start = 1 + (stacks-3) * slices;
    for (u32 i = 0; i < slices; i++) {
        indices[index++] = south_pole;
        indices[index++] = last_ring_start + (i+1) % slices;
        indices[index++] = last_ring_start + i;
    }
}

/* icosahedron vertex format: [ position.x, position.y, position.z, normal.x, normal.y, normal.z, uv.x, uv.y ] */
static const struct vertex icosahedron_vertices[] = {
    /* Positions (vec3)                Normals (vec3)                 UVs (vec2) */
    {{-0.5257f,  0.0000f,  0.8507f}, {-0.5257f,  0.0000f,  0.8507f}, {0.000f,  0.000f}},
    {{ 0.5257f,  0.0000f,  0.8507f}, { 0.5257f,  0.0000f,  0.8507f}, {0.333f,  0.000f}},
    {{-0.5257f,  0.0000f, -0.8507f}, {-0.5257f,  0.0000f, -0.8507f}, {0.666f,  0.000f}},
    {{ 0.5257f,  0.0000f, -0.8507f}, { 0.5257f,  0.0000f, -0.8507f}, {1.000f,  0.000f}},
    {{ 0.0000f,  0.8507f,  0.5257f}, { 0.0000f,  0.8507f,  0.5257f}, {0.166f,  0.333f}},
    {{ 0.0000f,  0.8507f, -0.5257f}, { 0.0000f,  0.8507f, -0.5257f}, {0.500f,  0.333f}},
    {{ 0.0000f, -0.8507f,  0.5257f}, { 0.0000f, -0.8507f,  0.5257f}, {0.833f,  0.333f}},
    {{ 0.0000f, -0.8507f, -0.5257f}, { 0.0000f, -0.8507f, -0.5257f}, {0.166f,  0.666f}},
    {{ 0.8507f,  0.5257f,  0.0000f}, { 0.8507f,  0.5257f,  0.0000f}, {0.333f,  0.666f}},
    {{-0.8507f,  0.5257f,  0.0000f}, {-0.8507f,  0.5257f,  0.0000f}, {0.666f,  0.666f}},
    {{ 0.8507f, -0.5257f,  0.0000f}, { 0.8507f, -0.5257f,  0.0000f}, {0.000f,  1.000f}},
    {{-0.8507f, -0.5257f,  0.0000f}, {-0.8507f, -0.5257f,  0.0000f}, {0.333f,  1.000f}},
};

static const u16 icosahedron_indices[] = {
    0,  4,  1, 0,  9,  4,  9,  5,  4,  4,  5,  8,  4,  8,  1,
    8, 10,  1, 8,  3, 10,  5,  3,  8,  5,  2,  3,  2,  7,  3,
    7, 10,  3, 7,  6, 10,  7, 11,  6, 11,  0,  6,  0,  1,  6,
    6,  1, 10, 9,  0, 11,  9, 11,  2,  9,  2,  5,  7,  2, 11,
};

/* icosahedron vertex format: [ position.x, position.y, position.z, normal.x, normal.y, normal.z, uv.x, uv.y ] */
static const struct vertex plane_vertices[] = {
    /* Positions (vec3)   Normals (vec3)       UVs (vec2) */
    {{-1.0f,  0.0f, -1.0f}, {0.0f,  1.0f,  0.0f}, {0.0f,  0.0f}},
    {{ 1.0f,  0.0f, -1.0f}, {0.0f,  1.0f,  0.0f}, {1.0f,  0.0f}},
    {{-1.0f,  0.0f,  1.0f}, {0.0f,  1.0f,  0.0f}, {0.0f,  1.0f}},
    {{ 1.0f,  0.0f,  1.0f}, {0.0f,  1.0f,  0.0f}, {1.0f,  1.0f}},
};

static const u16 plane_indices[] = {
    0, 1, 2, 1, 3, 2,
};

struct instance_data {
    mat4 transform; 
    quat rotation;
    s32  material_index;
};

static const struct instance_data instances[] = {
    { /* 0: metallic red */
        .transform = {
            { 0.5f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.5f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.5f, 0.0f }, { 2.0f, 0.5f, 1.0f, 1.0f },
        },
        .material_index = 0,
    }, 
    { /* 1: mirror */
        .transform = {
            { 0.3f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.3f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.3f, 0.0f }, { -1.0f, 0.3f, -2.0f, 1.0f },
        },
        .material_index = 1,
    },
    { /* 2: translucent blue */
        .transform = {
            { 0.4f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.4f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.4f, 0.0f }, { 0.0f, 0.4f, 0.0f, 1.0f },
        },
        .material_index = 2,
    },
    { /* 3: emissive */
        .transform = {
            { 0.6f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.6f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.6f, 0.0f }, { 1.5f, 1.0f, -1.0f, 1.0f },
        },
        .material_index = 3,
    },
    { /* 4: purple */
        .transform = {
            { 0.1f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.1f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.1f, 0.0f }, { -2.0f, 0.5f, -1.0f, 1.0f },
        },
        .material_index = 4,
    },
    { /* 5: worn iron */
        .transform = {
            { 0.8f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.8f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.8f, 0.0f }, { 3.0f, 0.5f, 2.0f, 1.0f },
        },
        .material_index = 5,
    },
};

struct pbr_material {
    vec4 albedo;
    f32  metallic;
    f32  roughness;
    vec3 emission;
    f32  emission_strength;
    f32  alpha;
};

static const struct pbr_material materials[] = {
    { /* 0: metallic red */
        .albedo = { 0.7f, 0.1f, 0.1f, 1.0f },
        .metallic = 1.0f,
        .roughness = 0.2f,
        .emission = { 0.0f, 0.0f, 0.0f, },
        .emission_strength = 0.0f,
        .alpha = 1.0f,
    }, 
    { /* 1: mirror */
        .albedo = { 1.0f, 1.0f, 1.0f, 1.0f },
        .metallic = 1.0f,
        .roughness = 0.01f,
        .emission = { 0.0f, 0.0f, 0.0f, },
        .emission_strength = 0.0f,
        .alpha = 1.0f,
    },
    { /* 2: translucent blue */
        .albedo = { 0.2f, 0.5f, 1.0f, 0.7f },
        .metallic = 0.0f,
        .roughness = 0.01f,
        .emission = { 0.0f, 0.0f, 0.0f, },
        .emission_strength = 0.0f,
        .alpha = 0.7f,
    },
    { /* 3: emissive */
        .albedo = { 0.1f, 0.1f, 0.1f, 1.0f },
        .metallic = 0.0f,
        .roughness = 1.0f,
        .emission = { 1.0f, 1.0f, 1.0f, },
        .emission_strength = 5.0f,
        .alpha = 1.0f,
    }, 
    { /* 4: purple */
        .albedo = { 0.5f, 0.0f, 1.0f, 1.0f },
        .metallic = 0.3f,
        .roughness = 0.4f,
        .emission = { 0.2f, 0.0f, 0.3f, },
        .emission_strength = 0.5f,
        .alpha = 1.0f,
    },
    { /* 5: worn iron */
        .albedo = { 0.4f, 0.4f, 0.4f, 1.0f },
        .metallic = 0.8f,
        .roughness = 0.7f,
        .emission = { 0.0f, 0.0f, 0.0f, },
        .emission_strength = 0.0f,
        .alpha = 1.0f,
    },
};

struct moonlit {
    struct moonlit_interface        interface;

    struct a_moonlit_walk_engine   *amw;
    struct vertex                   sphere_vertices[VERTICES_COUNT];
    u16                             sphere_indices[INDICES_COUNT];
};

#include "src/vulkan/vk_reznor.h"

void lake_fini(struct moonlit *lake)
{
    (void)lake;
}

RIVEN_ENCORE(moonlit, lake)
{
    assert_debug(encore->header.interface && *encore->header.interface == NULL);

    struct moonlit *lake = (struct moonlit *) 
        riven_alloc(encore->header.riven, encore->header.tag, sizeof(struct moonlit), _Alignof(struct moonlit));
    zerop(lake);

    lake->amw = encore->amw;
    construct_sphere(SLICES, STACKS, lake->sphere_vertices, lake->sphere_indices);

    /* write the interface */
    lake->interface.header.name = encore->header.metadata->game_name;
    lake->interface.header.riven = encore->header.riven;
    lake->interface.header.tag = encore->header.tag;
    lake->interface.header.fini = (PFN_riven_work)lake_fini;

    *encore->header.interface = (void *)lake;
    log_verbose("'%s' moonlit interface write.", lake->interface.header.name.ptr);
}

PFN_riven_work amw_main(struct amw_config *config, s32 argc, char **argv)
{
    config->metadata.game_build_version = AMW_VERSION;
    config->metadata.game_name = str_init("Lake in the Lungs");

    /* limit to 1 for now */
    config->riven_thread_count = 1;
    config->reznor_target_device_count = 1;

    log_set_verbose(argc-1);

    /* TODO parse the command line */
    (void)argv;

    return (PFN_riven_work)moonlit_encore_lake;
}
