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
    //创建型
    //Factory 工厂模式
    /*/
    {
        Shape* circle = ShapeFactory::createShape("Circle");
        if (circle != nullptr) {
            circle->draw(); // 输出：Drawing Circle.  
            delete circle; // 释放内存  
        }

        Shape* rectangle = ShapeFactory::createShape("Rectangle");
        if (rectangle != nullptr) {
            rectangle->draw(); // 输出：Drawing Rectangle.  
            delete rectangle; // 释放内存  
        }
    }//*/

    //Singleton 单例模式
    /*/
    {
        Singleton* s1 = Singleton::getInstance(); // 获取唯一实例  
        Singleton* s2 = Singleton::getInstance(); // 再次获取，仍然是同一个实例  

        s1->doSomething(); // 调用实例的方法  
        s2->doSomething(); // 通过另一个指针调用相同的方法  

        // 注意：在实际应用中，通常不会直接删除单例实例，因为单例的生命周期通常与整个应用程序相同。  
        // 如果确实需要删除实例，可以考虑在程序结束时由某个全局的清理机制来处理。  
        // delete s1; // 不建议这样做，除非你确定需要释放资源  
    }//*/

    //结构型
    //Proxy 代理模式
    /*/
    {
        Proxy proxy;
        proxy.request(); // 输出代理类添加的额外逻辑以及真实类的实现  
    }//*/

    //Decorator 装饰者模式
    /*/
    {
        // 创建一个简单咖啡  
        Coffee* simpleCoffee = new SimpleCoffee();
        std::cout << "Ordered: " << simpleCoffee->getDescription()
            << ", Cost: $" << simpleCoffee->getCost() << std::endl;

        // 装饰简单咖啡，加牛奶  
        Coffee* milkCoffee = new MilkCoffee(simpleCoffee);
        std::cout << "Ordered: " << milkCoffee->getDescription()
            << ", Cost: $" << milkCoffee->getCost() << std::endl;

        // 再次装饰，加糖  
        Coffee* milkSugarCoffee = new SugarCoffee(milkCoffee);
        std::cout << "Ordered: " << milkSugarCoffee->getDescription()
            << ", Cost: $" << milkSugarCoffee->getCost() << std::endl;

        delete milkSugarCoffee; // 删除最外层的装饰者，它会依次删除内部的装饰者和组件  
    }//*/

    //行为型
    //Observer 观察者模式
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

    //Chain Of Responsibility 职责链模式
    //*/
    {
        std::shared_ptr<Handler> handlerA = std::make_shared<ConcreteHandlerA>();
        std::shared_ptr<Handler> handlerB = std::make_shared<ConcreteHandlerB>();
        handlerA->setNext(handlerB);
        handlerB->setNext(NULL);

        std::shared_ptr<Request> request = std::make_shared<SpecificRequest>();
        handlerA->handleRequest(request); // 请求从handlerA开始传递  
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
