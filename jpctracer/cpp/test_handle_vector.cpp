#include "handle_vector.hpp"
#include <iostream>

template <class T> void print_range(T first, T last)
{
    while (first != last)
    {
        std::cout << *first << ",";
        ++first;
    }
    std::cout << "\n";
}

template<class T>
void check_distance(T first, T last){
    std::cout<<"first: "<<*first<<" ";
    std::cout<<"last: "<<*last<<"\n";
    std::cout<<"dist: "<<last-first<<"\n";
}

int main()
{
    jpc::handle_vector<int> vec;
    const int               n = 5;
    using handle_t = typename jpc::handle_vector<int>::handle_t;

    handle_t handles[n];

    for (int i = 0; i < n; i++)
    {
        handles[i] = vec.push_back(i);
    }
    std::cout << "all pushed back\n";
    print_range(vec.begin(), vec.end());

    vec.erase(handles[1]);
    vec.erase(handles[2]);
    vec.erase(handles[3]);
    std::cout << "erased\n";
    print_range(vec.begin(), vec.end());

    
    handle_t h = vec.push_back(n);
    print_range(vec.begin(), vec.end());

    check_distance(vec.begin(),vec.begin()+2);

    
    check_distance(vec.end()-1,vec.end()-3);

}
