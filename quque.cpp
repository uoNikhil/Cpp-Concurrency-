#include <atomic>

template <typename T>
struct Node
{
    T val;
    Node* next;

    Node(T value): val(value), next(nullptr){}
};

template <typename T>
class Queue
{
public:
    Queue():m_head.store(nullptr), m_tail.store(nullptr){}

    ~Queue(){} // later

    void enqueue(T value)
    {
        Node<T>* temp = new Node(value);
        do{
                Node<T>* temp 
        }
        while (!m_tail.compare_and_exchage_weak())
        {
            /* code */
        }
        

        m_tail->next = temp;

    }
private:
    std::atomic<Node<T>*> m_head;
    std::atomic<Node<T>*> m_tail;
};