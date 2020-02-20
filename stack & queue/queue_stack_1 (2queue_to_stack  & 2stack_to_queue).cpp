#include <stack>
#include <queue>
#include <iostream>
#include <algorithm>

template<typename T>
class mqueue
{
public:
    void push(T xx)
    {
        st_in.push(xx);

        if (st_out.empty()) {
            while (st_in.size()) {
                st_out.push(st_in.top());
                st_in.pop();
            }
        }
    }

    T front()
    {
        T ret(0);
        if (st_out.empty()) {
            if (st_in.empty())
                throw exception("no element");

            //move st_in elem to st_out
            while (st_in.size()) {
                st_out.push(st_in.top());
                st_in.pop();
            }
        }

        ret = st_out.top();
        st_out.pop();
        return ret;
    }

    size_t size()
    {
        return st_in.size() + st_out.size();
    }

    bool empty()
    {
        return size() == 0;
    }

private:
    std::stack<T> st_in;
    std::stack<T> st_out;
};


template<typename T>
class mstack
{
public:
    void push(T val)
    {
        if (!q_1.empty() && !q_2.empty())
            throw exception("error happen before");

        std::queue<T> &q_elem = (q_1.empty()) ? q_2 : q_1;

        q_elem.push(val);
    }

    T top()
    {
        if ((q_1.empty() && q_2.empty()) || (q_1.size() && q_2.size()))
            throw exception("error happen");

        std::queue<T> &q_elem = (q_1.size()) ? q_1 : q_2;
        std::queue<T> &q_empty = (q_1.size()) ? q_2 : q_1;

        while (q_elem.size() > 1) {
            q_empty.push(q_elem.front());
            q_elem.pop();
        }

        T ret = q_elem.front();
        q_elem.pop();

        q_empty.push(ret);

        return ret;
    }

    void pop()
    {
        if ((q_1.empty() && q_2.empty()) || (q_1.size() && q_2.size()))
            throw exception("error happen");

        std::queue<T> &q_elem = (q_1.size()) ? q_1 : q_2;
        std::queue<T> &q_empty = (q_1.size()) ? q_2 : q_1;

        while (q_elem.size() > 1) {
            q_empty.push(q_elem.front());
            q_elem.pop();
        }

        q_elem.pop();
    }

    size_t size()
    {
        return q_1.size() + q_2.size();
    }

    bool empty()
    {
        return size() == 0;
    }

private:
    std::queue<T> q_1;
    std::queue<T> q_2;
};

int main(void)
{
    mqueue<int> xx;
    xx.push(5);
    xx.push(4);
    std::cout << xx.front() << std::endl;  //5
    xx.push(3);
    xx.push(2);
    xx.push(1);
    std::cout << xx.front() << std::endl;  //4
    std::cout << xx.front() << std::endl;  //3
    std::cout << xx.front() << std::endl;  //2
    std::cout << xx.front() << std::endl;  //1
    //std::cout << xx.front() << std::endl;  //exception

    std::cout << std::endl;

    mstack<int> yy;
    yy.push(5);
    yy.push(4);
    yy.push(3);
    yy.push(2);
    yy.push(1);
    while (yy.size()) {
        std::cout << yy.top() << std::endl;
        yy.pop();
    }

    getchar();
    return 0;
}