#include <iostream>

#include "Logger/Logger.hpp"
#include "Window/Window.hpp"
#include "Event/Event.hpp"
#include "Time/Timer.hpp"
#include "Asset/AssetManager.hpp"
#include "Asset/AssetFactory.hpp"

#include "Planet.hpp"
#include "Stellar.hpp"
#include "Tag.hpp"
#include "SystemGenerator.hpp"
#include "Moon.hpp"

int main(int argc, char* argv[])
{
    // Create logger
    AMB::LogState log_state = AMB::LogLevel::Debug | AMB::LogLevel::Error | AMB::LogLevel::Fatal | AMB::LogLevel::Info | AMB::LogLevel::Warning | AMB::LogLevel::DispCMD | AMB::LogLevel::DispTXT;
    AMB::Logger::init(log_state, "sirona.log");
    AMB::Logger& logger = AMB::Logger::instance();

    AMB::Window window(1000, 800, "Stellar Test", SDL_INIT_EVERYTHING, SDL_WINDOW_OPENGL);
    AMB::EventManager event_manager(&window);
    AMB::Timer timer(60);

    std::random_device rd;
    AMB::Lehmer32 lehmer(rd());

    logger.log(AMB::Info, "=== PROGRAM STARTED ===");

    SystemGenerator system_generator("res/data/SystemGeneratorParam.csv");

    AMB::AssetManager asset_manager;
    AMB::FontSystem font_system;
    AMB::AssetFactory asset_factory(asset_manager, font_system);

    // Load shader
    AMB::AssetHandle shader_handle = asset_factory.create_shader("res/shader/stellar.vert", "res/shader/stellar.frag");
    if (!asset_manager.shaders.validity(shader_handle)) {
        logger.log(AMB::Fatal, "Can not load shader");
        exit(EXIT_FAILURE);
    }
    AMB::Shader& stellar_shader = asset_manager.shaders.get(shader_handle);

    AMB::CameraOrthographic camera(mat::Vec2f{0.0f, 0.0f}, mat::Vec2f{float(window.get_width()), float(window.get_height())});

    StellarSystem system = system_generator.generate(lehmer.next_uint32());
    StellarSystemSceneBuilder scene_builder;
    StellarSystemRenderer scene_renderer(window, stellar_shader);
    StellarSystemScene scene = scene_builder.build(system, camera);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    // Enable blending so alpha works
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mat::Mat4f mvp;

    while(!event_manager.is_quitting()) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        event_manager.manage();

        if (event_manager.keyboard().key_down(AMB::KeyCode::KEY_CODE_ESCAPE)) {
            event_manager.quit();
        }

        if (event_manager.keyboard().key_down(AMB::KEY_CODE_SPACE)) {
            system = system_generator.generate(lehmer.next_uint32());
            scene = scene_builder.build(system, camera);
            logger.log(AMB::Info, system.info_str());
            logger.log(AMB::Debug, "Visual radius = " + std::to_string(scene.get_stellar_visual().radius_visual));
        }

        float cam_max_velocity = 5.0f;
        mat::Vec2f cam_movement{0.0f, 0.0f};
        if (event_manager.keyboard().key_press(AMB::KEY_CODE_W)) { cam_movement[1] += cam_max_velocity; }
        if (event_manager.keyboard().key_press(AMB::KEY_CODE_S)) { cam_movement[1] -= cam_max_velocity; }
        if (event_manager.keyboard().key_press(AMB::KEY_CODE_D)) { cam_movement[0] += cam_max_velocity; }
        if (event_manager.keyboard().key_press(AMB::KEY_CODE_A)) { cam_movement[0] -= cam_max_velocity; }
        if (cam_movement.norm2() > 0.01) {
            cam_movement.normalize();
            cam_movement*=cam_max_velocity;
            scene.camera().move(cam_movement);
        }
        if (event_manager.keyboard().key_down(AMB::KEY_CODE_DOWN)) { scene.camera().zoom(0.9); }
        if (event_manager.keyboard().key_down(AMB::KEY_CODE_UP)) { scene.camera().zoom(1.1); }

        mvp = scene.camera().get_vp();

        scene_renderer.update_sytem_scene(scene);
        scene_renderer.render_stellar(mvp, scene.get_stellar_visual());

        window.present();

        timer.wait();
    }

    logger.log(AMB::Info, "=== PROGRAM FINISHED ===");    

    return 0;
}