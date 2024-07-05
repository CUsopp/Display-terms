// DesignPattern.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// main.cpp  
#include <iostream>  
#include "Factory_ShapeFactory.h"  
#include "Singleton.h"
#include "Proxy_Proxy.h"
#include "Decorator_CoffeeSimple.h"
#include "Decorator_Coffee_MilkCoffee.h"
#include "Decorator_Coffee_SugarCoffee.h"
#include "Observer_Concreate.h"
#include "Observer_Subject_Concreate.h"
#include "ChainOfRsp_Request_Specific.h"
#include "ChainOfRsp_Handler_Concreate_A.h"
#include "ChainOfRsp_Handler_Concreate_B.h"

int main() {
    //������
    //Factory ����ģʽ
    /*/
    {
        Shape* circle = ShapeFactory::createShape("Circle");
        if (circle != nullptr) {
            circle->draw(); // �����Drawing Circle.  
            delete circle; // �ͷ��ڴ�  
        }

        Shape* rectangle = ShapeFactory::createShape("Rectangle");
        if (rectangle != nullptr) {
            rectangle->draw(); // �����Drawing Rectangle.  
            delete rectangle; // �ͷ��ڴ�  
        }
    }//*/

    //Singleton ����ģʽ
    /*/
    {
        Singleton* s1 = Singleton::getInstance(); // ��ȡΨһʵ��  
        Singleton* s2 = Singleton::getInstance(); // �ٴλ�ȡ����Ȼ��ͬһ��ʵ��  

        s1->doSomething(); // ����ʵ���ķ���  
        s2->doSomething(); // ͨ����һ��ָ�������ͬ�ķ���  

        // ע�⣺��ʵ��Ӧ���У�ͨ������ֱ��ɾ������ʵ������Ϊ��������������ͨ��������Ӧ�ó�����ͬ��  
        // ���ȷʵ��Ҫɾ��ʵ�������Կ����ڳ������ʱ��ĳ��ȫ�ֵ��������������  
        // delete s1; // ��������������������ȷ����Ҫ�ͷ���Դ  
    }//*/

    //�ṹ��
    //Proxy ����ģʽ
    /*/
    {
        Proxy proxy;
        proxy.request(); // �����������ӵĶ����߼��Լ���ʵ���ʵ��  
    }//*/

    //Decorator װ����ģʽ
    /*/
    {
        // ����һ���򵥿���  
        Coffee* simpleCoffee = new SimpleCoffee();
        std::cout << "Ordered: " << simpleCoffee->getDescription()
            << ", Cost: $" << simpleCoffee->getCost() << std::endl;

        // װ�μ򵥿��ȣ���ţ��  
        Coffee* milkCoffee = new MilkCoffee(simpleCoffee);
        std::cout << "Ordered: " << milkCoffee->getDescription()
            << ", Cost: $" << milkCoffee->getCost() << std::endl;

        // �ٴ�װ�Σ�����  
        Coffee* milkSugarCoffee = new SugarCoffee(milkCoffee);
        std::cout << "Ordered: " << milkSugarCoffee->getDescription()
            << ", Cost: $" << milkSugarCoffee->getCost() << std::endl;

        delete milkSugarCoffee; // ɾ��������װ���ߣ���������ɾ���ڲ���װ���ߺ����  
    }//*/

    //��Ϊ��
    //Observer �۲���ģʽ
    /*/
    {
		ConcreteSubject subject;
		ConcreteObserver observer1("Observer 1");
		ConcreteObserver observer2("Observer 2");

		subject.registerObserver(&observer1);
		subject.registerObserver(&observer2);

		subject.notifyObservers("Hello, Observers!");

		subject.removeObserver(&observer1);

		subject.notifyObservers("Hello again, Observers!");
    }//*/

    //Chain Of Responsibility ְ����ģʽ
    //*/
    {
        std::shared_ptr<Handler> handlerA = std::make_shared<ConcreteHandlerA>();
        std::shared_ptr<Handler> handlerB = std::make_shared<ConcreteHandlerB>();
        handlerA->setNext(handlerB);
        handlerB->setNext(NULL);

        std::shared_ptr<Request> request = std::make_shared<SpecificRequest>();
        handlerA->handleRequest(request); // �����handlerA��ʼ����  
        handlerB->handleRequest(request);
    }//*/

    system("pause");
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
