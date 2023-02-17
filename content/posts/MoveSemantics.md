---
title: Move Semantics
date: 2023-02-17
---

Move semantics are an essential cpp concept. But to understand and appreciate them, we need to understand certain basics first.


# lvalue and rvalue
```cpp
int i = 0;
```
Here:
* `i` is an lvalue whereas `0` is an rvalue.
* lvalue is an object that occupies some identifiable location in memory.
* rvalue is an object that does not occupy some identifiable location in memory.

# Basics of lvalue and rvalue
### 1) We can assign an lvalue to an rvalue. But not vice-versa

For example:
```c++
int i = 10;
10 = i; // non-sense
```

### 2) We can't have an lvalue reference of an rvalue, But having an const lvalue reference of an rvalue is fine
```c++
int& i = 10; // Error
const int& i = 10; // OK
```

### 3) We can't have an rvalue reference of an lvalue
```c++
int i = 10;
int&& j = i; // Error
```


# Functions and lvalue, rvalue
The return value of a function is an l-value if and only if it is a reference
```c++
int Get()
{
    return 5;
}
```

```cpp
i = Get();
Get() = i; // Error
```

just to make it extra clear, it doesn't really matter if you change to:
```c++
int Get()
{
    int i = 5;
    return i;
}
```
Again, only way to return an lvalue is by returning an lvalue reference.
```c++
int& Get()
{
    int a = 1;
    return a;
}
```

Now you can do this:
```c++
i = Get();
Get() = i;
```

Bonus:
```c++
int& Get()
{
    static int a = 1;
    return a;
}
```

Can only have an lvalue refrence of an lvalue
```c++
void Set(int& i)
{
    i = 5;
}
```

```c++
int i = 10;
Set(i); // OK
Set(10); // Error
Set(i+ 10); // Error
```

No location to store this value, so we can't have an lvalue reference of an rvalue

const lvalue reference are fine however, because they can bind to rvalues
```c++
void Set(const int& i)
{
    printf("const lvalue reference\n");
}
```
# Move Semantics
## What is *MOVING*?
Moving in cpp means to transfer the ownership of an object from one variable to another. 
It is a very fast operation, because it doesn't involve any copying of data. It is just a pointer copy. Moving isn't cpoying and deleting the old object.
It is just transferring the ownership of the object from one variable to another.

Copying
```
X -->  0x16af33548
X -->  0x16af33548  0x16d473548 <--Y
```

Moving
```
X -->  0x16af33548
X --> NULL 0x16af33548 <--Y
```
No malloc, no construction

## Move Constructor
```c++
class vector
{
private:
    int* m_data;
    int m_size;
public:
    vector() = default;
    vector(int* data, const int& size) : m_size(size)
    {
        m_data = new int[m_size];
        memcpy(m_data, data, m_size * sizeof(int));
        printf("Vector contructed\n");
    }

    vector(const vector& other) : m_size(other.m_size)
    {
        m_data = new int[m_size];
        memcpy(m_data, other.m_data, m_size * sizeof(int));
        printf("Vector copied\n");
    }

    vector(vector&& other) : m_size(other.m_size)
    {
        m_data = other.m_data;
        m_size = other.m_size;

        // Make other point to null
        other.m_data = nullptr;
        other.m_size = 0;

        printf("Vector moved\n");
    }

    int size() const { return m_size; }

    int& operator[](const int& index) { return m_data[index]; }
};
```

```c++
int main()
{
    int data[] = { 1, 2, 3, 4, 5 };
    vector v1(data, 5);
    vector v2 = v1; // Copy constructor
    vector v3 = std::move(v1); // Move constructor

    std::cout<< v1.size(); // 0
    return 0;
}
```

**Important Note**: now `v1` points to `nullptr` and `v3` points to `v1`'s data. So if we try to access `v1`'s data, we will get a segmentation fault. 

Performance: Ofcoure it would be fast, memory alloc is often the bottleneck.

```cpp
int main()
{
    int size = 1000;
    int* data = new int[size];

    for (int i = 0 ; i < size ; ++i)
    {
        data[i] = i;
    }

    vector v = vector(data, size);

    auto time1 = high_resolution_clock::now();
    for (int i = 0 ; i < 10000000 ; i++)
    {

        foo temp2 = foo(v);
    }
    auto time2 = high_resolution_clock::now();
    for (int i = 0 ; i < 10000000 ; i++)
    {

        foo temp2 = foo(std::move(v));
    }
    auto time3 = high_resolution_clock::now();

    printf("No move, no inline construction: %d\n", duration_cast<microseconds>(time2 - time1).count());
    printf("Move, no inline construction   : %d\n", duration_cast<microseconds>(time3 - time2).count());

    return 0;
}
```

Output:
```
No move, no inline construction: 10116165
Move, no inline construction   : 83041
```

# Perfect Forwarding
Goal: To make a function take values and forward them to another with retaining them as lvalues or rvalues

```c++
template<typename T, typename Args>
T Create(Args& a)
{
    return T(a);
}
```

```c++
int i = 10;
int temp1 = Create(i); // OK
int temp2 = Create(10); // Error
int temp3 = Create(i+ 10); // Error
```

Maybe, overlord it with `const Args&`?
```c++
template<typename T, typename Args>
T Create(const Args& a)
{
    return T(a);
}
```

Now this works
```c++
int i = 10;
int temp1 = Create(i); // OK
int temp2 = Create(10); // OK
int temp3 = Create(i+ 10); // OK
```

But it's not forwarding, it's just taking a copy of the value

```c++
class foo
{
public:
    int m_i;
public:
    foo(int i) : m_i(i) 
    {
        printf("foo(int i)\n");
    }
};

int main()
{
    int i = 10;
    foo temp1 = Create<foo>(i); // OK
    foo temp2 = Create<foo>(10); // OK, but it constructs an object for an rvalue, now it is no longer an rvalue
    return 0;
}
```

For example if we have a constructor that takes an rvalue reference
```c++
class foo
{
public:
    int m_i;
public:
    foo(int&& i) : m_i(i) 
    {
        printf("foo(int&& i)\n");
    }
};
```
Now we get a compiler error. Because we are passing an lvalue to a function that takes an rvalue reference

Solution: Forwarding
```c++
template<typename T, typename ... Args>
T Create(Args&& ... a)
{
    return T(std::forward<Args>(a) ... );
}
```

```c++
int main()
{
    int i = 10;
    std::cout << "i = " << &i << std::endl;
    foo temp1 = Create<foo>(i); // OK
    foo temp2 = Create<foo>(10); // OK and now no construction!!! 
    return 0;
}
```
