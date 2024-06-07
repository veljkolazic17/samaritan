#include <defines.hpp>
#include <engine/memory/memory.hpp>
#include <engine/memory/allocators/allocator.hpp>

BEGIN_NAMESPACE

namespace Containers
{
    template <typename T>
    class Container
    {
    public:
        virtual muint64 GetSize() = 0;
        virtual void Clear() = 0;
        virtual void Print() = 0;
    };
}

END_NAMESPACE