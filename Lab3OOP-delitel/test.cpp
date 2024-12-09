#include <gtest/gtest.h>
#include "./Terminal/Terminal.hpp"
#include "./Storage/Storage.hpp"
#include "./Container/Container.hpp"
#include "./Container/FragileContainer.hpp"
#include "./Container/RefragedContainer.hpp"
#include "./Container/Frag_and_Ref.hpp"


TEST(StorageTests, Initialization) {
    Storage storage(1, 100, 100, 100, 20.0);
    EXPECT_EQ(storage.getInfoAboutStorage(), "Length: 100, Width: 100, Height: 100, Temperature: 20.000000");
}

TEST(StorageTest, CopyConstructor){
    Storage storage(1, 100, 100, 100, 20.0);
    storage.addContainer(std::make_shared<Container>("_", "Cargo A", 4, 10, 1, 21.2, 2.1));
    Storage storageCopy = storage;
    EXPECT_EQ(storageCopy.getInfoAboutStorage(), storage.getInfoAboutStorage());
}

TEST(StorageTest, AddContainer){
    Storage storage(1, 100, 100, 100, 20.0);
    std::shared_ptr<IContainer> container =   std::make_shared<RefragedContainer>("_", "Cargo A", 4, 10, 1, 21.2, 2.1, 13.1);
    EXPECT_THROW(storage.addContainer(container, 1, 1, 1), std::invalid_argument);
    container =   std::make_shared<FragileContainer>("_", "Cargo B", 101, 101, 101, 23.5, 2.5, 18.3);
    EXPECT_THROW(storage.addContainer(container, 1, 1, 1), std::invalid_argument);
    EXPECT_THROW(storage.addContainer(container, -1, -1, 0), std::invalid_argument);
    container =   std::make_shared<FragileContainer>("_", "Cargo B", 10, 10, 10, 23.5, 2.5, 1.3);
    EXPECT_THROW(storage.addContainer(container, 8, 9, 3), std::invalid_argument);
    container =   std::make_shared<FragileContainer>("_", "Cargo B", 2, 5, 2, 23.5, 2.5, 11.3);
    storage.addContainer(container, 1, 1, 1);
    container =   std::make_shared<FragileContainer>("_", "Cargo B", 6, 15, 3, 7.6, 1.5, 1.3);
    EXPECT_THROW(storage.addContainer(container, 1, 1, 4), std::invalid_argument);
    container =  std::make_shared<Container>("_", "Cargo A", 1, 4, 1, 21.2, 111.1);
    EXPECT_THROW(storage.addContainer(container, 1, 1, 4), std::invalid_argument);
    container =  std::make_shared<Container>("_", "Cargo A", 1, 4, 1, 21.2, 1.1);
    EXPECT_THROW(storage.addContainer(container, 1, 1, 5), std::invalid_argument);
    container =  std::make_shared<Container>("_", "Cargo A", 1, 4, 1, 21.2, 1.1);
    storage.addContainer(container, 1, 1 , 4);
    container =  std::make_shared<Container>("_", "Cargo A", 14, 4, 12, 21.2, 1.1);
    storage.addContainer(container, 17, 17 , 1);
    container =  std::make_shared<Container>("_", "Cargo A", 7, 4, 3, 21.2, 1.1);
    storage.addContainer(container, 81, 81 , 1);
    container =  std::make_shared<Container>("_", "Cargo A", 1, 1, 1, 21.2, 1.1);
    storage.addContainer(container, 1, 1 , 6);
    std::string result = storage.getInfo();
    EXPECT_EQ(result.find("1_1_1") != std::string::npos, true);
    EXPECT_EQ(result.find("1_1_4") != std::string::npos, true);
    EXPECT_EQ(result.find("17_17_1") != std::string::npos, true);
    EXPECT_EQ(result.find("81_81_1") != std::string::npos, true);
    EXPECT_EQ(result.find("1_1_6") != std::string::npos, true);
}


TEST(StorageTest, setSize){
    Storage storage(1, 100, 100, 100, 20.0);
    std::shared_ptr<IContainer> container =   std::make_shared<FragileContainer>("_", "Cargo B", 2, 5, 2, 23.5, 2.5, 11.3);
    storage.addContainer(container, 1, 1, 1);
    container =  std::make_shared<Container>("_", "Cargo A", 1, 4, 1, 21.2, 1.1);
    storage.addContainer(container, 1, 1 , 4);
    container =  std::make_shared<Container>("_", "Cargo A", 14, 4, 12, 21.2, 1.1);
    storage.addContainer(container, 17, 17 , 1);
    container =  std::make_shared<Container>("_", "Cargo A", 7, 4, 3, 21.2, 1.1);
    storage.addContainer(container, 81, 81 , 1);
    container =  std::make_shared<Container>("_", "Cargo A", 1, 1, 1, 21.2, 1.1);
    storage.addContainer(container, 1, 1 , 6);
    EXPECT_THROW(storage.getSize(100, 100, 10), std::runtime_error);
    storage.getSize(101, 100, 100);
    EXPECT_EQ(storage.getInfoAboutStorage(), "Length: 101, Width: 100, Height: 100, Temperature: 20.000000");
    std::string result = storage.getInfo();
    EXPECT_EQ(result.find("1_1_1") != std::string::npos, true);
    EXPECT_EQ(result.find("1_1_4") != std::string::npos, true);
    EXPECT_EQ(result.find("17_17_1") != std::string::npos, true);
    EXPECT_EQ(result.find("81_81_1") != std::string::npos, true);
    EXPECT_EQ(result.find("1_1_6") != std::string::npos, true);
    Storage st = storage;
    result = st.getInfo();
    EXPECT_EQ(result.find("1_1_1") != std::string::npos, true);
    EXPECT_EQ(result.find("1_1_4") != std::string::npos, true);
    EXPECT_EQ(result.find("17_17_1") != std::string::npos, true);
    EXPECT_EQ(result.find("81_81_1") != std::string::npos, true);
    EXPECT_EQ(result.find("1_1_6") != std::string::npos, true);
}


TEST(StorageTest, Move){
    Storage storage(1, 100, 100, 100, 20.0);
    std::shared_ptr<IContainer> container =   std::make_shared<FragileContainer>("_", "Cargo B", 2, 5, 2, 23.5, 2.5, 11.3);
    storage.addContainer(container, 1, 1, 1);
    container =  std::make_shared<Container>("_", "Cargo A", 1, 4, 1, 21.2, 1.1);
    storage.addContainer(container, 1, 1 , 4);
    container =  std::make_shared<Container>("_", "Cargo A", 14, 4, 12, 21.2, 1.1);
    storage.addContainer(container, 17, 17 , 1);
    container =  std::make_shared<Container>("_", "Cargo A", 7, 4, 3, 21.2, 1.1);
    storage.addContainer(container, 81, 81 , 1);
    container =  std::make_shared<Container>("_", "Cargo A", 1, 1, 1, 21.2, 1.1);
    storage.addContainer(container, 1, 1 , 6);
    EXPECT_THROW(storage.moveContainer("1_1_1", 0, 1, 1), std::invalid_argument);
    EXPECT_THROW(storage.moveContainer("4_1_8", 2, 1, 1), std::invalid_argument);
    EXPECT_THROW(storage.moveContainer("1_1_4", 2, 1, 1), std::invalid_argument);
    EXPECT_THROW(storage.moveContainer("1_1_6", 2, 1, 1), std::invalid_argument);
    storage.moveContainer("1_1_6", 91, 9, 1);
    std::string result = storage.getInfo();
    EXPECT_EQ(result.find("91_9_1") != std::string::npos, true);
    EXPECT_EQ(result.find("1_1_6") != std::string::npos, false);
}


TEST(StorageTest, Rotate){
    Storage storage(1, 100, 100, 100, 20.0);
    std::shared_ptr<IContainer> container =   std::make_shared<FragileContainer>("_", "Cargo B", 2, 5, 2, 23.5, 2.5, 11.3);
    storage.addContainer(container, 1, 1, 1);
    container =  std::make_shared<Container>("_", "Cargo A", 1, 4, 1, 21.2, 1.1);
    storage.addContainer(container, 1, 1 , 4);
    container =  std::make_shared<Container>("_", "Cargo A", 14, 4, 12, 21.2, 1.1);
    storage.addContainer(container, 17, 17 , 1);
    container =  std::make_shared<Container>("_", "Cargo A", 7, 4, 3, 21.2, 1.1);
    storage.addContainer(container, 81, 81 , 1);
    container =  std::make_shared<Container>("_", "Cargo A", 1, 1, 1, 21.2, 1.1);
    storage.addContainer(container, 1, 1 , 6);
    EXPECT_THROW(storage.rotateContainer("1_1_4", 0), std::invalid_argument);
    EXPECT_THROW(storage.rotateContainer("1_1_1", 0), std::invalid_argument);
    storage.rotateContainer("81_81_1", 5);
    auto i = storage.find("81_81_1");
    EXPECT_EQ(i.second->getLength(), 3);
    EXPECT_EQ(i.second->getWidth(), 4);
    EXPECT_EQ(i.second->getHeight(), 7);
    Storage st(1, 100, 100, 3, 20.0);
    container =  std::make_shared<Container>("_", "Cargo A", 7, 4, 1, 21.2, 1.1);
    st.addContainer(container, 1, 1, 1);
    EXPECT_THROW(st.rotateContainer("1_1_1", 3), std::invalid_argument);
    i = st.find("1_1_1");
    EXPECT_EQ(i.second->getLength(), 7);
    EXPECT_EQ(i.second->getWidth(), 4);
    EXPECT_EQ(i.second->getHeight(), 1);
    EXPECT_THROW(st.rotateContainer("99_1_1", 3), std::invalid_argument);
}


TEST(StorageTest, Remove){
    Storage storage(1, 32, 32, 32, 20.0);
    std::shared_ptr<IContainer> container =   std::make_shared<FragileContainer>("_", "Cargo B", 5, 5, 2, 23.5, 2.5, 11.3);
    storage.addContainer(container, 1, 1, 1);//1
    container =  std::make_shared<Container>("_", "Cargo A", 4, 4, 1, 21.2, 1.1);
    storage.addContainer(container, 1, 1 , 4);//2
    container =  std::make_shared<Container>("_", "Cargo A", 14, 4, 12, 21.2, 1.1);
    storage.addContainer(container, 6, 17 , 1);//3
    container =  std::make_shared<Container>("_", "Cargo A", 7, 4, 3, 21.2, 1.1);
    storage.addContainer(container, 6, 17 , 14);//4
    container =  std::make_shared<Container>("_", "Cargo A", 4, 3, 1, 21.2, 1.1);
    storage.addContainer(container, 1, 1 , 6);//5
    container =  std::make_shared<Container>("_", "Cargo A", 4, 3, 1, 21.2, 1.1);
    storage.addContainer(container, 1, 1 , 8);//6
    container =  std::make_shared<Container>("_", "Cargo A", 3, 2, 3, 21.2, 1.1);
    storage.addContainer(container, 1, 1 , 10);//7
    container =  std::make_shared<Container>("_", "Cargo A", 1, 2, 1, 21.2, 1.1);
    storage.addContainer(container, 1, 1 , 14);//8
    std::cout << storage.getInfo() << std::endl;
    std::cout<< std::endl;
    EXPECT_THROW(storage.removeContainer("90_1056_14"), std::invalid_argument);
    storage.removeContainer("1_1_14");
    std::vector<std::string> result = storage.getListContainers();
    EXPECT_EQ(result.size(), 7);
    std::cout << storage.getInfo() << std::endl;
    std::cout<< std::endl;
    storage.removeContainer("1_1_4");
    result = storage.getListContainers();
    EXPECT_EQ(result.size(), 6);
    std::cout << storage.getInfo() << std::endl;
    std::cout<< std::endl;
    storage.removeContainer("6_17_1");
    result = storage.getListContainers();
    EXPECT_EQ(result.size(), 5);
    std::cout << storage.getInfo() << std::endl;
    std::cout<< std::endl;
    EXPECT_NO_THROW(storage.getSize(100, 100, 32));
    storage.getInfo();
    Storage st(2, 12, 3, 5, 20.0);
    container =  std::make_shared<Container>("_", "Cargo A", 1, 1, 1, 21.2, 1.1);
    st.addContainer(container, 1, 1 , 1);
    container =  std::make_shared<Container>("_", "Cargo A", 8, 1, 1, 21.2, 1.1);
    st.addContainer(container,3, 1, 1);
    container =  std::make_shared<Container>("_", "Cargo A", 10, 1, 1, 21.2, 1.1);
    st.addContainer(container,1, 1, 3);
    std::cout << st.getInfo() << std::endl;
    EXPECT_THROW(st.removeContainer("3_1_1"), std::invalid_argument);
    std::cout << st.getInfo() << std::endl;
    Storage lastT(2, 13, 3, 5, 20.0);
    container =  std::make_shared<Container>("_", "Cargo A", 2, 1, 1, 21.2, 1.1);
    lastT.addContainer(container, 1, 1, 1);
    container =  std::make_shared<Container>("_", "Cargo A", 8, 1, 1, 21.2, 1.1);
    lastT.addContainer(container, 4, 1, 1);
    container =  std::make_shared<Container>("_", "Cargo A", 1, 1, 1, 21.2, 1.1);
    lastT.addContainer(container,1, 1, 3);
    container =  std::make_shared<Container>("_", "Cargo A", 1, 1, 1, 21.2, 1.1);
    lastT.addContainer(container,3, 1, 3);
    container =  std::make_shared<Container>("_", "Cargo A", 7, 1, 1, 21.2, 1.1);
    lastT.addContainer(container,5, 1, 3);
    EXPECT_NO_THROW(lastT.removeContainer("4_1_1"));
}


 TEST(StorageTest, howContainersInStorage){
     Storage storage(1, 10, 5, 3, 20.0);
     std::shared_ptr<IContainer> container = std::make_shared<Container>("_", "Cargo B", 2, 1, 1, 23.5, 2.5);
     storage.addContainer(container, 1, 1, 1);
     container = std::make_shared<Container>("_", "Cargo B", 1, 1, 1, 23.5, 2.5);
    int max = storage.howContainer(container);
     std::cout << max << std::endl;
     EXPECT_EQ(max, 7);
}


void checkCheker(Storage& storage, std::shared_ptr<IContainer> container, ContainerPosition<Point<int>> pos){
    if(((*container).isType() == "Fragile and Refraged Container" ))
    {
        throw std::invalid_argument("Proved");
    }
}

TEST(StorageTest, Checker){
    Storage storage(1, 10, 5, 5, 29.0);
    storage.addExternalCheckFunction(checkCheker);
    std::shared_ptr<IContainer> container =   std::make_shared<FragileRefragedContainer>("_", "Cargo B", 2, 1, 1, 23.5, 2.5, 24.5, 24.5);
    EXPECT_THROW(storage.addContainer(container, 1, 1, 1), std::invalid_argument);
}



TEST(StorageTest, RemoveHard){
    Storage st =  Storage(1, 100, 100, 32, 23.4);
    std::shared_ptr<IContainer> container1 = std::make_shared<Container>("_", "Cargo B", 6, 6, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container2 = std::make_shared<Container>("_", "Cargo B", 8, 10, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container3 = std::make_shared<Container>("_", "Cargo B", 10, 8, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container4 = std::make_shared<Container>("_", "Cargo B", 9, 9, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container5 = std::make_shared<Container>("_", "Cargo B", 8, 4, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container6 = std::make_shared<Container>("_", "Cargo B", 5, 2, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container7 = std::make_shared<Container>("_", "Cargo B", 1, 6, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container8 = std::make_shared<Container>("_", "Cargo B", 3, 2, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container9 = std::make_shared<Container>("_", "Cargo B", 2, 4, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container10 = std::make_shared<Container>("_", "Cargo B", 4, 10, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container11 = std::make_shared<Container>("_", "Cargo B", 7, 1, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container12 = std::make_shared<Container>("_", "Cargo B", 4, 3, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container13 = std::make_shared<Container>("_", "Cargo B", 8, 1, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container14 = std::make_shared<Container>("_", "Cargo B", 2, 3, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container15 = std::make_shared<Container>("_", "Cargo B", 7, 8, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container16 = std::make_shared<Container>("_", "Cargo B", 2, 7, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container17 = std::make_shared<Container>("_", "Cargo B", 7, 5, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container18 = std::make_shared<Container>("_", "Cargo B", 4, 1, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container19 = std::make_shared<Container>("_", "Cargo B", 8, 3, 1, 23.5, 2.5);
    st.addContainer(container1, 13, 1, 1);
    st.addContainer(container4, 3, 1, 1);
    st.addContainer(container10, 6, 21, 1);
    st.addContainer(container12, 1, 21, 1);
    st.addContainer(container2, 11, 21, 1);
    st.addContainer(container3, 9, 21, 3);
    st.addContainer(container5, 7, 1, 3);
    st.addContainer(container6, 1, 1, 3);
    st.addContainer(container11, 1, 21, 3);
    st.addContainer(container8, 1, 1, 5);
    st.addContainer(container9, 5, 1, 5);
    st.addContainer(container13, 1, 21, 5);
    st.addContainer(container7, 5, 1, 7);
    st.addContainer(container14, 1, 21, 7);
    st.addContainer(container15, 2, 1, 9);
    st.addContainer(container16, 1, 21, 9);
    st.addContainer(container18, 1, 21, 11);
    st.addContainer(container19, 1, 21, 13);
    st.addContainer(container17, 1, 21, 15);
    EXPECT_NO_THROW(st.removeContainer("3_1_1"));
    std::cout << st.getInfo() << std::endl;
    std::vector<std::string> result = st.getListContainers();
    EXPECT_EQ(result.size(), 18);
    for(auto i = result.begin(); i != result.end(); ++i){
        std::cout << *i <<std::endl;
    }
    Storage st1 =  Storage(1, 100, 100, 32, 23.4);
    container1 = std::make_shared<Container>("_", "Cargo B", 70, 80, 27, 23.5, 2.5);
    container2 = std::make_shared<Container>("_", "Cargo B", 1, 1, 1, 23.5, 2.5);
    container3 = std::make_shared<Container>("_", "Cargo B", 1, 1, 1, 23.5, 2.5);
    container4 = std::make_shared<Container>("_", "Cargo B", 1, 1, 1, 23.5, 2.5);
    container5 = std::make_shared<Container>("_", "Cargo B", 1, 1, 1, 23.5, 2.5);
    st1.addContainer(container1, 1, 1, 1);
    st1.addContainer(container2, 72, 1, 1);
    st1.addContainer(container3, 98, 98, 1);
    st1.addContainer(container4, 77, 1, 1);
    st1.addContainer(container5, 81, 1, 1);
    std::cout << "--------------------------------------------------------------\n";
    EXPECT_NO_THROW(st1.removeContainer("1_1_1"));
    std::cout << "--------------------------------------------------------------\n";
    std::cout << st1.getInfo() << std::endl;
    std::vector<std::string> result1 = st1.getListContainers();
    EXPECT_EQ(result1.size(), 4);
    Storage st2 =  Storage(1, 100, 100, 32, 23.4);
    container1 = std::make_shared<Container>("_", "Cargo B", 61, 80, 27, 23.5, 2.5);
    container2 = std::make_shared<Container>("_", "Cargo B", 1, 1, 1, 23.5, 2.5);
    container3 = std::make_shared<Container>("_", "Cargo B", 1, 1, 1, 23.5, 2.5);
    container4 = std::make_shared<Container>("_", "Cargo B", 1, 1, 1, 23.5, 2.5);
    container5 = std::make_shared<Container>("_", "Cargo B", 1, 1, 1, 23.5, 2.5);
    container6 = std::make_shared<Container>("_", "Cargo B", 26, 10, 1, 23.5, 2.5);
    container7 = std::make_shared<Container>("_", "Cargo B", 1, 1, 1, 23.5, 2.5);
    st2.addContainer(container1, 1, 1, 1);
    st2.addContainer(container2, 63, 1, 1);
    st2.addContainer(container3, 65, 1, 1);
    st2.addContainer(container4, 67, 1, 1);
    st2.addContainer(container5, 69, 1, 1);
    st2.addContainer(container7, 71, 1, 1);
    st2.addContainer(container6, 73, 6, 1);
    std::cout << "--------------------------------------------------------------\n";
    EXPECT_NO_THROW(st2.removeContainer("73_6_1"));
    std::cout << "--------------------------------------------------------------\n";
    std::cout << st2.getInfo() << std::endl;
    std::vector<std::string> result2 = st2.getListContainers();
    EXPECT_EQ(result2.size(), 6);
    for(auto i = result2.begin(); i != result2.end(); ++i){
        std::cout << *i <<std::endl;
    }
}


TEST(OctreeTest, TestIterator){
    Storage st =  Storage(1, 100, 100, 32, 23.4);
    std::shared_ptr<IContainer> container1 = std::make_shared<Container>("_", "Cargo B", 6, 6, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container2 = std::make_shared<Container>("_", "Cargo B", 8, 10, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container3 = std::make_shared<Container>("_", "Cargo B", 10, 8, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container4 = std::make_shared<Container>("_", "Cargo B", 9, 9, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container5 = std::make_shared<Container>("_", "Cargo B", 8, 4, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container6 = std::make_shared<Container>("_", "Cargo B", 5, 2, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container7 = std::make_shared<Container>("_", "Cargo B", 1, 6, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container8 = std::make_shared<Container>("_", "Cargo B", 3, 2, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container9 = std::make_shared<Container>("_", "Cargo B", 2, 4, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container10 = std::make_shared<Container>("_", "Cargo B", 4, 10, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container11 = std::make_shared<Container>("_", "Cargo B", 7, 1, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container12 = std::make_shared<Container>("_", "Cargo B", 4, 3, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container13 = std::make_shared<Container>("_", "Cargo B", 8, 1, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container14 = std::make_shared<Container>("_", "Cargo B", 2, 3, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container15 = std::make_shared<Container>("_", "Cargo B", 7, 8, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container16 = std::make_shared<Container>("_", "Cargo B", 2, 7, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container17 = std::make_shared<Container>("_", "Cargo B", 7, 5, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container18 = std::make_shared<Container>("_", "Cargo B", 4, 1, 1, 23.5, 2.5);
    std::shared_ptr<IContainer> container19 = std::make_shared<Container>("_", "Cargo B", 8, 3, 1, 23.5, 2.5);
    st.addContainer(container1, 13, 1, 1);
    st.addContainer(container4, 3, 1, 1);
    st.addContainer(container10, 6, 21, 1);
    st.addContainer(container12, 1, 21, 1);
    st.addContainer(container2, 11, 21, 1);
    st.addContainer(container3, 9, 21, 3);
    st.addContainer(container5, 7, 1, 3);
    st.addContainer(container6, 1, 1, 3);
    st.addContainer(container11, 1, 21, 3);
    st.addContainer(container8, 1, 1, 5);
    st.addContainer(container9, 5, 1, 5);
    st.addContainer(container13, 1, 21, 5);
    st.addContainer(container7, 5, 1, 7);
    st.addContainer(container14, 1, 21, 7);
    st.addContainer(container15, 2, 1, 9);
    st.addContainer(container16, 1, 21, 9);
    st.addContainer(container18, 1, 21, 11);
    st.addContainer(container19, 1, 21, 13);
    st.addContainer(container17, 1, 21, 15);
    EXPECT_NO_THROW(st.removeContainer("1_21_3"));
    auto vec = st.getListContainers();
    for(auto i = vec.begin(); i != vec.end(); ++i){
        std::cout << *i <<std::endl;
    }
    EXPECT_EQ(vec.size(), 18);
    EXPECT_THROW(st.find("0_120_2"), std::invalid_argument);
    EXPECT_NO_THROW(st.find("3_1_1"));
    auto it = st.find("13_1_1");
    EXPECT_EQ(it.second->getId(), "13_1_1");
}


TEST(AddTerminalTest, Success) {
    Terminal terminal;
    Storage* storage =  new Storage(1, 2, 2, 3, 20.0);
    EXPECT_NO_THROW(terminal.add(1, storage));
}


TEST(AddTerminalTest, AlreadyExists) {
    Terminal terminal;
    Storage* storage =  new  Storage(1, 2, 2, 3, 20.0);
    terminal.add(1, storage);
    EXPECT_THROW(terminal.add(1, storage), std::invalid_argument);
}

TEST(RemoveTerminalTest, Success) {
    Terminal terminal;
    Storage* storage =  new Storage(1, 2, 2, 3, 20.0);
    terminal.add(1, storage);
    EXPECT_NO_THROW(terminal.remove(1));
}

TEST(RemoveTerminalTest, NotExists) {
    Terminal terminal;
    EXPECT_THROW(terminal.remove(1), std::invalid_argument);
}

TEST(GetSizeStorageTest, TerminalExists) {
    Terminal terminal;
    Storage* storage =  new Storage(1, 2, 2, 3, 20.0);
    terminal.add(1, storage);
    int length = 4;
    int width = 4;
    int height = 4;
    EXPECT_NO_THROW(terminal.setsizeStorage(1, length, width, height));
    EXPECT_EQ(length, 4);
    EXPECT_EQ(width, 4);
    EXPECT_EQ(height, 4);
}

TEST(GetSizeStorageTest, TerminalNotExists) {
    Terminal terminal;
    int length, width, height;
    EXPECT_THROW(terminal.setsizeStorage(1, length, width, height), std::invalid_argument);
}

TEST(GetAllInfoTest, OutputsCorrectInfo) {
    Terminal terminal;
    Storage* storage =  new Storage(1, 2, 2, 3, 20.0);
    terminal.add(1, storage);
    testing::internal::CaptureStdout();
    terminal.getallInfo(std::cout);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Storage ID: 1"), std::string::npos);
}


TEST(FindTerminalTest, Success) {
    Terminal terminal;
    Storage* storage =   new Storage(1, 2, 2, 3, 20.0);
    terminal.add(1, storage);
    Storage* foundStorage = terminal.find(1);
    ASSERT_NE(foundStorage, nullptr);
    EXPECT_EQ(foundStorage, storage);
}

TEST(FindTerminalTest, NotFound){
    Terminal terminal;
    Storage* storage =  new Storage(1, 2, 2, 3, 20.0);
    terminal.add(1, storage);
    EXPECT_THROW(terminal.find(2), std::invalid_argument);
}


// TEST(OCTREE, ReverseforwardTest){
//     Octree<int, std::shared_ptr<IContainer>, ContainerPosition<int>> tree;
//     tree.insert()
// }

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}