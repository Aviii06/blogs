#include <iostream>
template <typename T,typename ... Arg>
T Create(Arg&& ... a)
{
    return T(std::forward<Arg>(a) ... );
}


class foo
{
private:
    int m_i;
public:
    foo(const int& i) : m_i(i)
    {
        printf("foo(const int& i)\n");
    }
    foo(int&& i): m_i(i)
    {
        printf("foo(int&& i)\n");
    }
};

int main()
{
    int i = 10;
    std::cout << "i = " << &i << std::endl;
    foo temp1 = Create<foo>(i); // OK
    foo temp2 = Create<foo>(10); // OK and now no contruction!!!
    return 0;
}