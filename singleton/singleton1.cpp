#include<iostream>
using namespace std;

class Singleton
{
    public:
        static Singleton *GetInstance()
        {
            return m_pInstance;
        }

    private:
        static Singleton *m_pInstance;
        Singleton() { cout << "Singleton()" << endl; };
        ~Singleton() { cout << "~Singleton()" << endl; };

        class Garbo     /* Garbo意为垃圾工人，任务是在 Singleton 实例生命周期结束后(即进程即将结束）回收该实例 */
        {
            public:
                ~Garbo()
                {
                    if(Singleton::m_pInstance)
                        delete m_pInstance;
                }
        };
        static Garbo garbo;
};

Singleton::Garbo Singleton::garbo;      /* 一定要初始化，否则程序结束时不会调用~Garbo() */
Singleton *Singleton::m_pInstance = new Singleton;  /* main函数之前就生成一个实例 */

int main(void)
{
    Singleton *p1 = Singleton::GetInstance();
    Singleton *p2 = Singleton::GetInstance();
    if(p1 == p2)
    {
        cout << "p1 == p2" << endl;
    }
    return 0;
}
