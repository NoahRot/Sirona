#include <iostream>

#include "Logger/Logger.hpp"

#include "Planet.hpp"
#include "Stellar.hpp"
#include "Tag.hpp"
#include "SystemGenerator.hpp"

int main(int argc, char* argv[])
{
    // Create logger
    AMB::LogState log_state = AMB::LogLevel::Debug | AMB::LogLevel::Error | AMB::LogLevel::Fatal | AMB::LogLevel::Info | AMB::LogLevel::Warning | AMB::LogLevel::DispCMD | AMB::LogLevel::DispTXT;
    AMB::Logger::init(log_state, "sirona.log");
    AMB::Logger& logger = AMB::Logger::instance();

    std::random_device rd;
    AMB::Lehmer32 lehmer(rd());

    logger.log(AMB::Info, "=== PROGRAM STARTED ===");

    generate_system(lehmer.next_uint32(), {});

    logger.log(AMB::Info, "=== PROGRAM FINISHED ===");    

    return 0;
}