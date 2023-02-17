#include <iostream>
#include <chrono>
using namespace std::chrono;

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
//        printf("Vector contructed\n");
    }

    vector(const vector& other) : m_size(other.m_size)
    {
        m_data = new int[m_size];
        memcpy(m_data, other.m_data, m_size * sizeof(int));
//        printf("Vector copied\n");
    }

    vector(vector&& other) : m_size(other.m_size)
    {
        m_data = other.m_data;
        m_size = other.m_size;

        // Make other point to null
        other.m_data = nullptr;
        other.m_size = 0;

//        printf("Vector moved\n");
    }

    int size() const { return m_size; }

    int& operator[](const int& index) { return m_data[index]; }
};

class foo
{
private:
    vector m_v;
public:
    foo() = default;
    foo(const vector& v) : m_v(v)
    {
//        printf("foo constructed\n");
    }
    foo(vector&& v) : m_v(std::move(v))
    {
//        printf("foo constructed\n");
    }

    void print()
    {
        for (int i = 0 ; i < m_v.size() ; ++i)
        {
            printf("%d ", m_v[i]);
        }
        printf("\n");
    }
};

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

//int main()
//{
//    int data[] = { 1, 2, 3, 4, 5 };
//    vector v1(data, 5);
//    vector v2 = v1; // Copy constructor
//    vector v3 = std::move(v1); // Move constructor
//
//    std::cout<< v1.size(); // 0
//    return 0;
//}