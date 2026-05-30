#pragma once

#include "Planet.hpp"
#include "Moon.hpp"
#include "Stellar.hpp"

#include "Graphic/VertexArray.hpp"
#include "Graphic/IndexBuffer.hpp"
#include "Graphic/VertexBuffer.hpp"
#include "Graphic/Layout.hpp"
#include "Camera/Camera2D.hpp"
#include "mat/Math.hpp"

struct SystemDisk {
    float richness;
    float instability;
};

enum class SystemOrbitalZone {
    Inner,
    Habitable,
    Outer,
    Frozen
};

enum class SystemSlotContent {
    Empty,
    Planet,
    AsteroidBelt
};

struct StellarSystem {
    uint32_t seed;

    StellarObject stellar_object;
    std::vector<PlanetObject> planets;
    SystemDisk disk;
    std::vector<float> slot_orbital_distance;
    std::vector<SystemSlotContent> slot_content;

    std::string info_str() {
        std::stringstream oss;
        oss << "===== Stellar System =====\n";
        oss << stellar_object.info_str();
        oss << "Disk: \n > Richness: " << disk.richness << "\n > Instability: "  << disk.instability << "\n";
        oss << "Slots: \n > slot count: " << slot_orbital_distance.size() << "\n";

        oss << "Stellar system layout:\n";
        uint32_t planet_count(0); 
        for (uint32_t i(0) ; i < slot_orbital_distance.size() ; ++i) {
            std::string occ_debug;
            uint32_t nbr_moon(0);
            std::string data_str = "";
            switch(slot_content[i]) {
                case SystemSlotContent::Empty:
                    occ_debug = "Empty";
                    break;
                case SystemSlotContent::Planet:
                    occ_debug = "Planet";
                    nbr_moon = planets[planet_count].moons.size();
                    data_str += planets[planet_count].info_str(3);
                    for (uint32_t j(0) ; j < planets[planet_count].moons.size() ; ++j) {
                        data_str += planets[planet_count].moons[j].info_str(6);
                    }
                    planet_count++;
                    break;
                case SystemSlotContent::AsteroidBelt:
                    occ_debug = "Asteroid Belt";
                    break;
            }

            std::string debug = " > Object " + std::to_string(i+1) + "/" + std::to_string(slot_orbital_distance.size()) + 
            ", Orbital: " + std::to_string(slot_orbital_distance[i]) + 
            ", Content: " + occ_debug + 
            ", Nbr moons: " + std::to_string(nbr_moon) + 
            "\n" + data_str + " \n";

            oss << debug;
        }
        
        return oss.str();
    }
};

struct SphericalVisual {
    SphericalVisual(mat::Vec2f position_, float rotation_, float radius_visual_, mat::Vec3f color_)
    : position(position_), rotation(rotation_), radius_visual(radius_visual_), color(color_)
    {}

    mat::Vec2f position;
    float rotation;
    float radius_visual;
    mat::Vec3f color;

    virtual void update(float dt) = 0;

    mat::Mat3f model() {
        mat::Mat3f tran = mat::graph::translate2<float>(position);
        mat::Mat3f rota = mat::graph::rotate2<float>(rotation);

        return mat::dot(tran, rota);
    }
};

struct StellarVisual : SphericalVisual {
    const StellarObject* object;
    float angular_vel;

    StellarVisual(const StellarObject* object_, mat::Vec2f position_, float rotation_, float radius_visual_, mat::Vec3f color_, float angular_vel_)
    : SphericalVisual(position_, rotation_, radius_visual_, color_), object(object_), angular_vel(angular_vel_)
    {}

    virtual void update(float dt) override {
        rotation += dt*angular_vel;
    }
};

struct StellarVertex {
    mat::Vec2f position;
    mat::Vec2f uv;
};

class StellarSystemScene {
public:
    StellarSystemScene(const AMB::CameraOrthographic& camera, const StellarVisual& stellar_visual)
    : m_camera(camera), m_stellar_visual(stellar_visual)
    {}

    AMB::CameraOrthographic& camera() {
        return m_camera;
    }

    const StellarVisual& get_stellar_visual() const {
        return m_stellar_visual;
    }

private:
    AMB::CameraOrthographic m_camera;
    StellarVisual m_stellar_visual;
};

class StellarSystemSceneBuilder {
public:
    StellarSystemScene build(const StellarSystem& system, const AMB::CameraOrthographic& camera) {
        StellarVisual stellar_visual = build_stellar_visual(system.stellar_object);

        StellarSystemScene scene(camera, stellar_visual);

        return scene;
    }

private:
    StellarVisual build_stellar_visual(const StellarObject& stellar) {
        float radius_visual = compute_visual_stellar_radius(stellar.radius);
        mat::Vec3f color{1.0f, 1.0f, 1.0f};
        return StellarVisual(&stellar, mat::Vec2f{0.0f, 0.0f}, 0.0f, radius_visual, {1.0f, 1.0f, 0.0f}, 0.0f);
    }

    float compute_visual_stellar_radius(float stellar_radius) {
        float visual_radius = pow(stellar_radius, 0.4f);
        float clamped = std::clamp(visual_radius, m_min_radius, m_max_radius);
        float delta = abs(visual_radius - clamped);
        
        // If inside bounds, return original value
        if (delta < 0.0001)
            return visual_radius;
        
        // Smooth falloff when outside bounds
        return clamped + (visual_radius - clamped) * exp(-delta / m_radius_transition);
        return visual_radius;
    }

    float m_min_radius = 30.0f;
    float m_max_radius = 400.0f;
    float m_radius_transition = 0.15;
};

class StellarSystemRenderer {
public:
    StellarSystemRenderer(const AMB::Window& window, AMB::Shader& stellar_shader) 
    : m_resolution{float(window.get_width()), float(window.get_height())}, m_stellar_shader(stellar_shader)
    {
        // --- Build stellar rendering objects ---
        m_stellar_layout.add_float(2); // Position
        m_stellar_layout.add_float(2); // UV
        m_stellar_vao = AMB::create_vertex_array();
        m_stellar_vbo = AMB::create_vertex_buffer(0, false);
        m_stellar_ibo = AMB::create_index_buffer(0, false);
        m_stellar_vao->add_vertex_buffer(m_stellar_vbo, m_stellar_layout);
        m_stellar_vao->set_index_buffer(m_stellar_ibo);
    }

    void update_sytem_scene(const StellarSystemScene& scene) {
        update_stellar(scene.get_stellar_visual());
    }

    void update_stellar(const StellarVisual& stellar_visual) {
        float cote = 0.5f*stellar_visual.radius_visual;

        m_stellar_vertices = {
            StellarVertex{{-cote, -cote}, {-1.0f, -1.0f}},
            StellarVertex{{ cote, -cote}, { 1.0f, -1.0f}},
            StellarVertex{{ cote,  cote}, { 1.0f,  1.0f}},
            StellarVertex{{-cote,  cote}, {-1.0f,  1.0f}}
        };

        m_stellar_indices = {
            0,1,2,
            2,3,0
        };

        m_stellar_vbo->update(m_stellar_vertices.data(), m_stellar_vertices.size()*sizeof(StellarVertex));
        m_stellar_ibo->update(m_stellar_indices.data(), m_stellar_indices.size());
    }

    void render_stellar(const mat::Mat4f& mvp, const StellarVisual& stellar_visual) {
        m_stellar_vao->bind();
        m_stellar_shader.use_shader();
        m_stellar_shader.set_mat4f("u_mvp", mvp);
        m_stellar_shader.set_3f("u_star_color", stellar_visual.color);
        m_stellar_shader.set_2f("u_resolution", m_resolution);
        std::shared_ptr<AMB::IndexBuffer> ibo_ptr = m_stellar_vao->get_index_buffer();
        ibo_ptr->bind();
        glDrawElements(GL_TRIANGLES, ibo_ptr->count(), GL_UNSIGNED_INT, nullptr);
    }

private:
    mat::Vec2f m_resolution;

    // --- Stellar rendering objects ---
    AMB::Shader& m_stellar_shader;
    AMB::VertexAttribLayout m_stellar_layout;
    std::shared_ptr<AMB::VertexArray> m_stellar_vao;
    std::shared_ptr<AMB::VertexBuffer> m_stellar_vbo;
    std::shared_ptr<AMB::IndexBuffer> m_stellar_ibo;
    std::vector<StellarVertex> m_stellar_vertices;
    std::vector<uint32_t> m_stellar_indices;
};