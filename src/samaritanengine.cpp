#include <engine/Engine.hpp>
#define PROJECT_NAME "samaritanengine"

int main(void) 
{
    samaritan::Engine::Engine::GetInstance().Init();
    samaritan::Engine::Engine::GetInstance().Run();
    samaritan::Engine::Engine::GetInstance().Shutdown();
    return 0;
}
