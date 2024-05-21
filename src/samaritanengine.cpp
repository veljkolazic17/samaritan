#include <engine/Engine.hpp>
#define PROJECT_NAME "samaritanengine"

int main(void) 
{
    samaritan::Engine::Engine& engine = samaritan::Engine::Engine::GetInstance();

    engine.Init();
    engine.Run();
    engine.Shutdown();

    return 0;
}
