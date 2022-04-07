#ifndef ITERATOR_ADAPTER_H
#define ITERATOR_ADAPTER_H

#include <stddef.h>
#include <iterator>
#include <concepts>
#include <iostream>

namespace adapter {


//IteratorBasis is a minimal class out of which we can implement a full iterator interface.

template<typename X>
concept IteratorBasis =
     std::semiregular<typename X::value_type> 
  //&& IteratorCategory<X::iterator_category>
  //&& std::indirectly_writable<typename X::pointer,typename X::value_type>
  && std::indirectly_readable<typename X::pointer>
  && std::signed_integral<typename X::difference_type>
  && requires(X& b, const X& c1, const X& c2, typename X::value_type v,typename X::difference_type n) {
      typename X::iterator_category;
                { c1.ptr() } -> std::same_as<typename X::pointer>;
                {*c1.ptr()} -> std::same_as<typename X::reference>;
                { b.increment() } -> std::same_as<void>; 
                // defined if iterator category is at least bidirecitional:
                { b.decrement() } -> std::same_as<void>;
                // defined if iterator category is random_access:
                { b.increment(n) } -> std::same_as<void>;
                { c1.difference(c1) } -> std::same_as<typename X::difference_type>;
                {c1.state()};
                {b.state()};
             };


/*
template <typename Iterator>
class reverse_iterator_basis
{
  typedef Iterator state_type;
  typedef typename Iterator::iterator_category iterator_category; 
  typedef typename Iterator::value_type value_type;
  typedef typename Iterator::difference_type difference_type;
  typedef typename Iterator::reference reference;
  typedef typename Iterator::pointer pointer;  

  state_type position;

  reference deref() {
    state_type tmp = position;
    return *--tmp;
  }

  void increment() { --position; }
  void decrement() { ++position; }
  void increment(difference_type n) { position -= n; }
  difference_type difference(const reverse_iterator_basis& x) { 
    return position - x.position; 
  }
  const state_type& state() { return position; }

};

typedef iterator<reverse_iterator_basis<int*> > reverse_int_pointer;
*/


template <IteratorBasis B>
class iterator
{
public:
  typedef typename B::iterator_category iterator_category;
  typedef typename B::value_type value_type;
  typedef typename B::difference_type difference_type;
  typedef typename B::reference reference;
  typedef typename B::pointer pointer;


public:

  B basis;

  auto state()const{
      return basis.state();
  }

  auto state(){
      return basis.state();
  }

  iterator() {}

  iterator(const B& basis) : basis(basis) {} 

  reference operator*() const { 
    return *basis.ptr();
  }

  pointer operator->() const { return basis.ptr(); }

  iterator& operator++() { 
    basis.increment();
    return *this;
  }

  iterator operator++(int) {
    iterator tmp = *this;
    ++*this;
    return tmp;
  }

  // requires bidirectional iterator (at least)
  iterator& operator--() { 
    basis.decrement();
    return *this;
  }

  // requires bidirectional iterator (at least)
  iterator operator--(int) {
    iterator tmp = *this;
    --*this;
    return tmp;
  }

  // requires random access iterator
  iterator& operator+=(difference_type n) {
    basis.increment(n);
    return *this;
  }

  // requires random access iterator
  iterator& operator-=(difference_type n) {
    return *this += -n;
  }

  // requires random access iterator
  friend
  iterator operator+(iterator x, difference_type n) {
    return x += n;
  }

  // requires random access iterator
  friend
  iterator operator+(difference_type n, iterator x) {
    return x += n;
  }

  // requires random access iterator
  friend
  iterator operator-(iterator x, difference_type n) {
    return x -= n;
  }

  // requires random access iterator
  friend
  difference_type operator-(const iterator& x, const iterator& y) {
    return x.basis.difference(y.basis);
  }

  // requires random access iterator
  reference operator[](difference_type n) const { return *(*this + n); }

  friend
  bool operator==(const iterator& x, const iterator& y) {
    return x.state() == y.state();
  }

  friend
  bool operator!=(const iterator& x, const iterator& y) {
    return !(x == y);
  }

  // requires random access iterator
  friend
  bool operator<(const iterator& x, const iterator& y) {
    return y - x > 0;
  }

  // requires random access iterator
  friend
  bool operator>(const iterator& x, const iterator& y) {
    return y < x;
  }

  // requires random access iterator
  friend
  bool operator<=(const iterator& x, const iterator& y) {
    return !(y < x);
  }

  // requires random access iterator
  friend
  bool operator>=(const iterator& x, const iterator& y) {
    return !(x < y);
  }
};

// Our implementation of output_iterator template depends on STL guarantee that
// "any [output] iterator value should be assigned through before
// it is incremented" (STL Report, page 8)
// http://www.stepanovpapers.com/STL/DOC.PDF

/*
//OutputIteratorBasis is a minimal class out of which we can implement a full output iterator interface.

concept OutputIteratorBasis<SemiRegular X> =
  && SemiRegular<X::state_type>
  && requires(X b, Typename T) {
                const state_type& { b.state() };
                void { b.store(const Storable<T>& x) };
             }
*/

class cout_basis {
public:
  typedef void state_type;
  void state() {}
  void store(const int& x) { std::cout << x; }
  void store(const double& x) { std::cout << x; }
}; 

template <typename OutputIteratorBasis>
class output_iterator
{
public:
  typedef typename OutputIteratorBasis::state_type state_type;
  typedef std::output_iterator_tag iterator_category; 
  typedef void                     value_type;
  typedef void                     difference_type;
  typedef void                     pointer;
  typedef void                     reference;

private:
  OutputIteratorBasis basis;
public:
  const state_type& state() const { return basis.state(); }

  output_iterator() {}
  
  output_iterator(const OutputIteratorBasis& basis) : basis(basis) {}

  template <typename T>
  void operator=(const T& value) { 
    basis.store(value);
  }

  output_iterator& operator*() { return *this; }
  output_iterator& operator++() { return *this; }
  output_iterator& operator++(int) { return *this; }
};

template <typename OutputIteratorBasis>
inline
output_iterator<OutputIteratorBasis> 
make_output_iterator(const OutputIteratorBasis& basis) {
  return output_iterator<OutputIteratorBasis>(basis);
}



//a pointer to a custom reference object
template<class Ref>
class proxy_pointer{
private:
    Ref ref;
public:


    using value_type = typename Ref::value_type;
    using reference_type = Ref;

    proxy_pointer(Ref ref):ref(ref){};
    const Ref* operator->()const{
        return &ref;
    }
    Ref operator*()const{
        return ref;
    }
};

//pointer to a const proxy object
template<class T>
class proxy_pointer_const{
private:
    T obj;
public:


    using value_type = T;
    using reference_type = T&;

    proxy_pointer_const(const T& o):obj(o){};
    const T* operator->()const{
        return &obj;
    }
    const T& operator*()const{
        return obj;
    }
};


/*
template<class T>
concept ReferencBasis = 
*/


template<class T>
concept ReferenceBasis = std::copy_constructible<T>
&& std::move_constructible<T>
&& std::is_object_v<T>
&& requires(const T& c, T& m,const typename T::value_type& v)
{
    typename T::value_type;
    {c.get()}->std::convertible_to<typename T::value_type>;
    {m.set(v)}->std::same_as<void>;
};

template<ReferenceBasis Ref>
struct proxy_ref: public Ref{

    using value_type = typename Ref::value_type;

    constexpr operator value_type() const noexcept{
        return this->get();
    }

    constexpr proxy_ref& operator=(const proxy_ref& d)noexcept{
        operator=((value_type)d);
        return *this;
    };
    constexpr proxy_ref operator=(const proxy_ref&d) const noexcept{
        proxy_ref ref(*this);
        ref=d;
        return d;
    }

    constexpr proxy_ref operator=(const value_type&d) const noexcept{
        proxy_ref dst(*this);
        dst.set(d);
        return dst;

    }
    constexpr
    friend
    void swap(proxy_ref a, proxy_ref b) noexcept
    {
        value_type tmp = a;
        a = b;
        b=tmp;
    }

};


} // end namespace adapter

// Local Variables:
// mode: c++
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
#endif
