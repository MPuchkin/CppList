#include <initializer_list>
#include <memory>
#include <iterator>

template<typename T>
class MyList
{
private:
    struct mynode {
        T data;
        mynode* next;
        mynode* prev;
        mynode(T d, mynode* p, mynode* n) : data(d), prev(p), next(n) {}
    };

    mynode* head, * tail;
    size_t _size;
public:

    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;

    class iterator {
        mynode* p;
    public:
        using value_type = T;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using reference = value_type&;

        iterator(mynode* pp) : p(pp) {}
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
        friend bool operator==(iterator v1, iterator v2) {
            return v1.p == v2.p;
        }
        friend bool operator!=(iterator v1, iterator v2) {
            return !(v1 == v2);
        }
    };

    class const_iterator {
        mynode* p;
    public:
        using value_type = T;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using pointer = const T*;
        using reference = const value_type&;

        const_iterator(mynode* pp) : p(pp) {}
        const T& operator*() const { return p->data; }
        const_iterator& operator++() {
            p = p->next;
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator rez(*this);
            p = p->next;
            return rez;
        }
        friend bool operator==(const_iterator v1, const_iterator v2) {
            return v1.p == v2.p;
        }
        friend bool operator!=(const_iterator v1, const_iterator v2) {
            return !(v1 == v2);
        }
    };

    MyList() : head(nullptr), tail(nullptr), _size(0) {};
    
    MyList(const MyList& other) : MyList(other.cbegin(), other.cend()) {};
    
    MyList(MyList&& other) : head(other.head), tail(other.tail), _size(other._size) {
        other.head = nullptr;
        other.tail = nullptr;
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


    void push_back(const T& value) {
        mynode *newNode = new mynode(value, tail, nullptr);
        if (tail != nullptr) {
            tail->next = newNode;
            tail = newNode;
        }
        else {
            head = tail = newNode;
        }
        _size++;
    }
    
    void pop_back() {
        if (tail == nullptr) return;
        mynode* tmp = tail;
        tail = tail->prev;
        if (tail != nullptr)
            tail->next = nullptr;
        else
            head = nullptr;
        delete tmp;
        _size--;
    }

    bool empty() const noexcept { return _size == 0; }

    size_t size() const noexcept {
        return _size;
    }

    void clear() noexcept {
        while (!empty())
            pop_back();
    };

    ~MyList() { clear(); }

    iterator begin() {
        return iterator(head);
    }
    
    const_iterator begin() const {
        return const_iterator(head);
    }

    const_iterator cbegin() {
        return const_iterator(head);
    }

    iterator end() {
        return iterator(nullptr);
    }

    const_iterator cend() {
        return const_iterator(nullptr);
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