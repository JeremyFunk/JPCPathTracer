#pragma once

#include <vcruntime.h>
#include "memory"
namespace core
{
    struct MemorySavePoint
    {

    };
    class MemoryArea
    {
    public:
        MemoryArea();
        
        template<class T, class... Args>
        T* Create(Args&&... args);
        MemorySavePoint GetSavePoint();
        void SetSavePoint(MemorySavePoint savepoint);
        ~MemoryArea();
    private:

        
    };

    template<class T, class... Args>
    T* MemoryArea::Create(Args&&... args) 
    {
        return new T(std::forward<Args>(args)...);

    }

}