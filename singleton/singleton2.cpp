#include<iostream>
#include<pthread.h>
using namespace std;

pthread_mutex_t mutex;

class Singleton
{
    public:
        static Singleton *GetInstance()
        {
            if(NULL == m_pInstance)
            {
                pthread_mutex_lock(&mutex);
                if(m_pInstance == NULL)
                    m_pInstance = new Singleton;
                pthread_mutex_unlock(&mutex);
            }
            return m_pInstance;
        }

    private:
        static Singleton *m_pInstance;
        Singleton() { cout << "Singleton()" << endl; }
        ~Singleton() { cout << "~Singleton()" << endl; }

        class Garbo
        {
            public:
                ~Garbo()
                {
                    if(m_pInstance)
                        delete m_pInstance;
                }
        };
        static Garbo garbo;
};
Singleton::Garbo Singleton::garbo;          /* 静态类成员初始化 */
Singleton *Singleton::m_pInstance = NULL;   /* 静态类成员初始化 */
int main(void)
{
    pthread_mutex_init(&mutex, NULL);

    Singleton *p1 = Singleton::GetInstance();
    Singleton *p2 = Singleton::GetInstance();
    if(p1 == p2)
        cout << "p1 == p2" << endl;

    pthread_mutex_destroy(&mutex);
    return 0;
}
