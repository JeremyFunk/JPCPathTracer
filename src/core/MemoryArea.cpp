#include "MemoryArea.h"
#include <cmath>
#include <corecrt_malloc.h>
#include <unordered_map>



namespace core
{
    MemoryArea::MemoryArea() 
    {
        //_size = 20000*sizeof(int);
        //_start = (char*) malloc(_size);
        //_current = _start;

    }
    
    MemorySavePoint MemoryArea::GetSavePoint() 
    {
        return MemorySavePoint();
    }

    
    void MemoryArea::SetSavePoint(MemorySavePoint savepoint) 
    {
        
    }
    
    MemoryArea::~MemoryArea() 
    {
    }

}