#include <initializer_list>
#include <memory>
#include <iterator>
#include <stdexcept>

template<typename T, typename Allocator = std::allocator<T>>
class MyList
{
private:
    struct mynode {
        T data;
        mynode* next;
        mynode* prev;
        mynode(const T& d, mynode* p, mynode* n) : data(d), prev(p), next(n) {}
        mynode(T&& d, mynode* p, mynode* n) : data(std::move(d)), prev(p), next(n) {}
    };

    mynode* dummy;
    size_t _size;

    using AllocType = typename std::allocator_traits<Allocator>::template rebind_alloc<mynode>;

    AllocType Alc;
public:
    using allocator_type = Allocator;
    using pointer = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;

    class const_iterator;

    class iterator {
        friend class MyList;
        friend class const_iterator;
        mynode* p;
        
        iterator(mynode* pp) : p(pp) {}
    public:
        using value_type = T;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using reference = value_type&;

        
        T& operator*() { return p->data; }
        iterator& operator++() {
            p = p->next;
            return *this;
        }
        iterator operator++(int) {
            iterator rez(*this);
            p = p->next;
            return rez;
        }
        iterator& operator--() {
            p = p->prev;
            return *this;
        }
        iterator operator--(int) {
            iterator rez(*this);
            p = p->prev;
            return rez;
        }
        friend bool operator==(const iterator &v1, const iterator &v2) {
            return v1.p == v2.p;
        }
        friend bool operator!=(const iterator &v1, const iterator &v2) {
            return !(v1 == v2);
        }
    };

    class const_iterator {
        
        friend class MyList;

        mynode* p;
        const_iterator(mynode* pp) : p(pp) {}
    public:
        using value_type = T;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using pointer = const T*;
        using reference = const value_type&;
        
        const T& operator*() const { return p->data; }
        const_iterator(iterator& it) : p(it.p) {}
        const_iterator& operator++() {
            p = p->next;
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator rez(*this);
            p = p->next;
            return rez;
        }        
        const_iterator& operator--() {
            p = p->prev;
            return *this;
        }
        const_iterator operator--(int) {
            const_iterator rez(*this);
            p = p->prev;
            return rez;
        }
    private:
        iterator get_iterator() {
            return iterator(p);
        }
    public:


        friend bool operator==(const const_iterator &v1, const const_iterator &v2) {
            return v1.p == v2.p;
        }
        friend bool operator!=(const const_iterator &v1, const const_iterator &v2) {
            return !(v1 == v2);
        }
    };

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
private:



    inline mynode* construct_dummy() {
        //  Выделение памяти без инициализации
        mynode* nodeptr{ Alc.allocate(1) };

        //  Инициализация полей кроме поля data - зацикливаем на себя
        std::allocator_traits<AllocType>::construct(Alc, &(nodeptr->prev), nodeptr);
        std::allocator_traits<AllocType>::construct(Alc, &(nodeptr->next), nodeptr);
        return nodeptr;
    }

    inline mynode* construct_node(value_type&& elem, mynode* prev, mynode* next) {
        //  Выделение памяти без инициализации
        mynode* nodeptr{ Alc.allocate(1) };
        //  Инициализация полей включая поле data
        //  Инициализация полей кроме поля data - зацикливаем на себя
        std::allocator_traits<AllocType>::construct(Alc, &(nodeptr->prev), prev);
        std::allocator_traits<AllocType>::construct(Alc, &(nodeptr->next), next);
        
        std::allocator_traits<AllocType>::construct(Alc, &(nodeptr->data), std::move(elem));
        return nodeptr;
    }

    inline mynode* construct_node(const value_type & elem, mynode *prev, mynode *next) {
        return construct_node(std::move(value_type(elem)), prev, next);
    }

    inline void delete_dummy(mynode * node) {
        std::allocator_traits<AllocType>::destroy(Alc, &(node->prev));
        std::allocator_traits<AllocType>::destroy(Alc, &(node->next));
        std::allocator_traits<AllocType>::deallocate(Alc, node, 1);
    }

    inline void delete_node(mynode* node) {
        std::allocator_traits<AllocType>::destroy(Alc, &(node->data));
        delete_dummy(node);
    }

public:
    MyList() : dummy(construct_dummy()), _size(0) {};
    
    MyList(const MyList& other) : MyList(other.cbegin(), other.cend()) {};
    
    MyList(MyList&& other) : dummy(other.dummy), _size(other._size) {
        other.dummy = nullptr;
        std::swap(Alc, other.Alc);
        other._size = 0;
    };

    MyList(std::initializer_list<T> init) : MyList() {
        for (auto x : init)
            push_back(x);
    }

    template<typename iter>
    MyList(iter start, iter fin) : MyList() {
        while (start != fin)
            push_back(*start++);
    }
    
    allocator_type get_allocator() const noexcept {
        return Alc;
    }
    
    reference front() { 
        if(empty()) throw new std::out_of_range("Trying to pop on empty list!");
        return dummy->next->data; 
    }
    
    const_reference front() const {
        if (empty()) throw new std::out_of_range("Trying to pop on empty list!");
        return dummy->next->data;
    };

    reference back() {
        if (empty()) throw new std::out_of_range("Trying to pop on empty list!");
        return dummy->prev->data;
    };

    const_reference back() const {
        if (empty()) throw new std::out_of_range("Trying to pop on empty list!");
        return dummy->prev->data;
    };

    iterator erase(const_iterator pos) {
        ++pos;
        remove_node(pos.p->prev);
        return pos.get_iterator();
    }

    iterator erase(const_iterator first, const_iterator last) {
        mynode* left = first.p->prev;
        mynode* tmp = first.p;
        while (tmp != last.p) {
            tmp = tmp->next;
            remove_node(tmp->prev);
        }
        left->next = last.p;
        last.p->prev = left;

        return last.get_iterator();
    }

    void push_back(T&& value) {
        mynode* newNode = new mynode(std::move(value), dummy->prev, dummy);
        dummy->prev->next = newNode;
        dummy->prev = newNode;
        _size++;
    }

    void push_back(const T& value) {
        push_back(std::move(T(value)));
    }

private:
    void remove_node(mynode* node) {
        if (node == dummy)
            throw new std::out_of_range("Trying to pop on empty list!");
        node->prev->next = node->next;
        node->next->prev = node->prev;
        delete_node(node);
        _size--;
    }

public:
    void pop_back() {
        remove_node(dummy->prev);
    }

    void push_front(T&& value) {
        mynode* newNode = new mynode(std::move(value), dummy, dummy->next);
        dummy->next->prev = newNode;
        dummy->next = newNode;
        _size++;
    }

    void push_front(const T& value) {
        push_front(std::move(T(value)));
    }

    void pop_front() {
        remove_node(dummy->next);
    }

    bool empty() const noexcept { return _size == 0; }

    size_t size() const noexcept {
        return _size;
    }

    void clear() noexcept {
        while (!empty())
            pop_back();
    };

    ~MyList() {
        if (dummy != nullptr) {
            clear();
            delete_dummy(dummy);
        }
    }

    iterator begin() {
        return iterator(dummy->next);
    }
    
    const iterator begin() const {
        return const_iterator(dummy->next);
    }

    const_iterator cbegin() {
        return const_iterator(dummy->next);
    }

    iterator end() {
        return iterator(dummy);
    }

    const_iterator end() const {
        return iterator(dummy);
    }

    const_iterator cend() {
        return const_iterator(dummy);
    }
    
    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }
    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(cend());
    }
    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }
    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }
    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(cbegin());
    }
    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(cbegin());
    }

};






/*
#include <initializer_list>
#include <memory>
#include <iterator>


template<typename T>
class MyList
{
public:
    using value_type      = T;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using reference       = value_type&;
    using const_reference = const value_type&;

    template<typename Pointer, typename Reference>
    class BaseIterator : public std::iterator<std::bidirectional_iterator_tag, value_type, difference_type, Pointer, Reference>
    {
    public:
        bool operator==(const BaseIterator& other);
        bool operator!=(const BaseIterator& other);
        pointer operator->();
        BaseIterator& operator--();
        BaseIterator& operator--(int);
        BaseIterator& operator++();
        BaseIterator& operator++(int);
        Reference operator*();
    };

    using iterator               = BaseIterator<pointer, reference>;
    using const_iterator         = BaseIterator<const_pointer, const_reference>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    MyList();
    MyList(const MyList& other);
    MyList(MyList&& other);
    MyList(std::initializer_list<T> init);

    MyList& operator=(const MyList& other);
    MyList& operator=(MyList&& other) noexcept;
    MyList& operator=(std::initializer_list<T> ilist);

    reference front();
    const_reference front() const;
    reference back();
    const_reference back() const;

    iterator begin() noexcept;
    const_iterator begin() const noexcept;
    const_iterator cbegin() const noexcept;
    iterator end() noexcept;
    const_iterator end() const noexcept;
    const_iterator cend() const noexcept;

    reverse_iterator rbegin() noexcept;
    const_reverse_iterator rbegin() const noexcept;
    const_reverse_iterator crbegin() const noexcept;
    reverse_iterator rend() noexcept;
    const_reverse_iterator rend() const noexcept;
    const_reverse_iterator crend() const noexcept;

    bool empty() const noexcept;
    size_type size() const noexcept;

    void clear() noexcept;

    iterator insert(const_iterator pos, const T& value);
    iterator insert(const_iterator pos, T&& value);

    iterator erase(iterator pos);
    iterator erase(const_iterator pos);
    iterator erase(iterator first, iterator last);
    iterator erase(const_iterator first, const_iterator last);

    void push_back(const T& value);
    void push_back(T&& value);
    void pop_back();
    void push_front(const T& value);
    void push_front(T&& value);
    void pop_front();

    void swap(MyList& other) noexcept;

    void reverse() noexcept;

    ~MyList();
};


*/
