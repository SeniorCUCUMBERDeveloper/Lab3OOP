#include "./Octree/Octree.hpp"
#include "./Storage/Storage.hpp"
#include "./Container/Container.hpp"
#include "./Terminal/Terminal.hpp"
#include "./Container/FragileContainer.hpp"
#include "./Container/RefragedContainer.hpp"
#include "./Request/Request.hpp"

int main(){
   Terminal terminal;
   
   // Создаем новый склад
   Storage* st = new Storage(1, 32, 32, 16, 23.4);
     terminal.add(1, st);
  //  Request request(st, 15);
  //  std::thread requestThread(&Request::RequestQ, &request);
  //  requestThread.join();
   // //st->getInfoAboutStorage();
   IContainer* container = new FragileContainer("_", "Cargo A", 2, 5, 2, 21.2, 2.1, 300.0);
   IContainer* container2 = new Container("_", "Cargo A", 2, 5, 1, 21.2, 2.1);
   IContainer* container3 = new Container("_", "Cargo A", 2, 5, 1, 21.2, 11.1);
   IContainer* container4 = new Container("_", "Cargo A", 1, 7, 1, 21.2, 2.1);
   IContainer* container5 = new Container("_", "Cargo A", 2, 1, 1, 21.2, 2.1);
     IContainer* container6 = new FragileContainer("_", "Cargo A", 7, 8, 1, 21.2, 2.1, 2.2);
     IContainer* container7 = new FragileContainer("_", "Cargo A", 7, 8, 3, 21.2, 2.1, 2.2);
   st->addContainer(container, 0, 0, 0);
  st->addContainer(container2, 0, 0, 8);
  // st->addContainer(container3);
  // st->addContainer(container4);
  // st->addContainer(container5);
  // st->addContainer(container6);
  // st->addContainer(container7);
  st->check();
   std::cout << st->getInfo() << std::endl;
    // IContainer* container3 = new Container("_", "Cargo A", 2, 5, 1, 21.2, 11.1);
    //st->addContainer(container3);
  //   std::cout << st->getInfo() << std::endl;
     //st->addContainer(container2);
  //   std::cout << st->getInfo() << std::endl;
//  IContainer* container4 = new Container("_", "Cargo A", 1, 7, 1, 21.2, 2.1);
//    IContainer* container5 = new Container("_", "Cargo A", 2, 1, 1, 21.2, 2.1);
//      IContainer* container6 = new FragileContainer("_", "Cargo A", 7, 8, 1, 21.2, 2.1, 2.2);
//      IContainer* container7 = new FragileContainer("_", "Cargo A", 7, 8, 3, 21.2, 2.1, 2.2);
// //   /* std::string f =*/ st->addContainer(container, 0, 0, 0); // (0,0,3) над первым контейнером
     // st->addContainer(container, 0, 0, 0); // (1,0,3) над вторым контейнером
   //   st->moveContainer("0_0_0", 7, 8, 2);
   //   std::cout << st->getInfo() << std::endl;
//    // st->addContainer(container2, 4, 0, 3); 
    // st->addContainer(container);
    // st->addContainer(container2);
    // std::string f = st->addContainer(container5);
    // st->removeContainer(f);

    // st->addContainer(container3);
    // f = st->addContainer(container4);
    // st->removeContainer(f);
    // f = st->addContainer(container6);
    // st->removeContainer(f);
    // std::cout << st->getInfo() << std::endl;
    //Storage stt(*st);
    //  st->getInfoAboutStorage();
    //  st->getSize(80, 80, 16);
    //  st->getInfoAboutStorage();
   //  st->removeContainer("0_20_6");
   // std::cout << st->getInfo() << std::endl;
   // st->rotateContainer("0_20_4", 4);
   // std::cout << st->getInfo() << std::endl;
   //  std::cout << st->getInfo() << std::endl;
   //  st->rotateContainer("0_0_2", 0);
   //  std::cout << st->getInfo() << std::endl;
   //  st->rotateContainer("0_0_2", 3);
   //  std::cout << st->getInfo() << std::endl;
   //  st->rotateContainer("0_0_2", 5);
   //  std::cout << st->getInfo() << std::endl;
   //st->addContainer(container5, 36, 32, 0);  
   //----
   // std::string f = st->addContainer(container);
   // st->printPosFULL(f);
   // f = st->addContainer(container2);
   // st->printPosFULL(f);
   // f = st->addContainer(container3);
   // st->printPosFULL(f);
   // f = st->addContainer(container4);;
   // st->printPosFULL(f);
   // f = st->addContainer(container5);
   // st->printPosFULL(f);
   // //----
   //  st->printPosFULL("0_0_0");
   //  st->printPosFULL("0_0_3");
   //  st->printPosFULL("0_0_8");
   //  std::cout << st->getInfo() << std::endl;
   //  st->removeContainer("0_0_0");
   //  std::cout << st->getInfo() << std::endl;
    // size_t number = st->howContainer(container7);
    // delete container7;
    // std::cout << "Number of Fragile Containers: " << number << std::endl;
   // st->rotateContainer("0_0_3", 90);
   // st->rotateContainer("4_0_8", 6);
   // st->printPosFULL("4_0_8");
   //st->getInfoAboutStorage();
   //st->check();
   //st->rotateContainer("19_1_10", 4);
   //st->printPosFULL("19_1_10");
   //st->addContainer(container6, 70, 70, 0);  
   //st->printPosFULL("70_70_0");
   //st->check();
   //st->printPosFULL(11);
}