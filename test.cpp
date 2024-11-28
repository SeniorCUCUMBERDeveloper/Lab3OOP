#include <gtest/gtest.h>
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
    storage.addContainer(new Container("_", "Cargo A", 4, 10, 1, 21.2, 2.1));
    Storage storageCopy = storage;
    EXPECT_EQ(storageCopy.getInfoAboutStorage(), storage.getInfoAboutStorage());
}

TEST(StorageTest, AddContainer){
    Storage storage(1, 100, 100, 100, 20.0);
    IContainer* container = new RefragedContainer("_", "Cargo A", 4, 10, 1, 21.2, 2.1, 13.1);
    EXPECT_THROW(storage.addContainer(container, 0, 0, 0), std::invalid_argument);
    delete container;
    container = new FragileContainer("_", "Cargo B", 101, 101, 101, 23.5, 2.5, 18.3);
    EXPECT_THROW(storage.addContainer(container, 0, 0, 0), std::invalid_argument);
    EXPECT_THROW(storage.addContainer(container, -1, -1, 0), std::invalid_argument);
    delete container;
    container = new FragileContainer("_", "Cargo B", 10, 10, 10, 23.5, 2.5, 1.3);
    EXPECT_THROW(storage.addContainer(container, 7, 8, 2), std::invalid_argument);
    delete container;
    container = new FragileContainer("_", "Cargo B", 2, 5, 2, 23.5, 2.5, 11.3);
    storage.addContainer(container, 0, 0, 0);
    container = new FragileContainer("_", "Cargo B", 6, 15, 3, 7.6, 1.5, 1.3);
    EXPECT_THROW(storage.addContainer(container, 0, 0, 3), std::invalid_argument);
    delete container;
    container =  new Container("_", "Cargo A", 1, 4, 1, 21.2, 111.1);
    EXPECT_THROW(storage.addContainer(container, 0, 0, 3), std::invalid_argument);
    delete container;
    container =  new Container("_", "Cargo A", 1, 4, 1, 21.2, 1.1);
    EXPECT_THROW(storage.addContainer(container, 0, 0, 4), std::invalid_argument);
    delete container;
    container =  new Container("_", "Cargo A", 1, 4, 1, 21.2, 1.1);
    storage.addContainer(container, 0, 0 , 3);
    container =  new Container("_", "Cargo A", 14, 4, 12, 21.2, 1.1);
    storage.addContainer(container, 16, 16 , 0);
    container =  new Container("_", "Cargo A", 7, 4, 3, 21.2, 1.1);
    storage.addContainer(container, 80, 80 , 0);
    container =  new Container("_", "Cargo A", 1, 1, 1, 21.2, 1.1);
    storage.addContainer(container, 0, 0 , 5);
    std::string result = storage.getInfo();
    EXPECT_EQ(result.find("0_0_0") != std::string::npos, true);
    EXPECT_EQ(result.find("0_0_3") != std::string::npos, true);
    EXPECT_EQ(result.find("16_16_0") != std::string::npos, true);
    EXPECT_EQ(result.find("80_80_0") != std::string::npos, true);
    EXPECT_EQ(result.find("0_0_5") != std::string::npos, true);
}


TEST(StorageTest, setSize){
    Storage storage(1, 100, 100, 100, 20.0);
    IContainer* container = new FragileContainer("_", "Cargo B", 2, 5, 2, 23.5, 2.5, 11.3);
    storage.addContainer(container, 0, 0, 0);
    container =  new Container("_", "Cargo A", 1, 4, 1, 21.2, 1.1);
    storage.addContainer(container, 0, 0 , 3);
    container =  new Container("_", "Cargo A", 14, 4, 12, 21.2, 1.1);
    storage.addContainer(container, 16, 16 , 0);
    container =  new Container("_", "Cargo A", 7, 4, 3, 21.2, 1.1);
    storage.addContainer(container, 80, 80 , 0);
    container =  new Container("_", "Cargo A", 1, 1, 1, 21.2, 1.1);
    storage.addContainer(container, 0, 0 , 5);
    EXPECT_THROW(storage.getSize(100, 100, 10), std::runtime_error);
    storage.getSize(101, 100, 100);
    EXPECT_EQ(storage.getInfoAboutStorage(), "Length: 101, Width: 100, Height: 100, Temperature: 20.000000");
    std::string result = storage.getInfo();
    EXPECT_EQ(result.find("0_0_0") != std::string::npos, true);
    EXPECT_EQ(result.find("0_0_3") != std::string::npos, true);
    EXPECT_EQ(result.find("16_16_0") != std::string::npos, true);
    EXPECT_EQ(result.find("80_80_0") != std::string::npos, true);
    EXPECT_EQ(result.find("0_0_5") != std::string::npos, true);
    Storage st = storage;
    result = st.getInfo();
    EXPECT_EQ(result.find("0_0_0") != std::string::npos, true);
    EXPECT_EQ(result.find("0_0_3") != std::string::npos, true);
    EXPECT_EQ(result.find("16_16_0") != std::string::npos, true);
    EXPECT_EQ(result.find("80_80_0") != std::string::npos, true);
    EXPECT_EQ(result.find("0_0_5") != std::string::npos, true);
}


TEST(StorageTest, Move){
    Storage storage(1, 100, 100, 100, 20.0);
    IContainer* container = new FragileContainer("_", "Cargo B", 2, 5, 2, 23.5, 2.5, 11.3);
    storage.addContainer(container, 0, 0, 0);
    container =  new Container("_", "Cargo A", 1, 4, 1, 21.2, 1.1);
    storage.addContainer(container, 0, 0 , 3);
    container =  new Container("_", "Cargo A", 14, 4, 12, 21.2, 1.1);
    storage.addContainer(container, 16, 16 , 0);
    container =  new Container("_", "Cargo A", 7, 4, 3, 21.2, 1.1);
    storage.addContainer(container, 80, 80 , 0);
    container =  new Container("_", "Cargo A", 1, 1, 1, 21.2, 1.1);
    storage.addContainer(container, 0, 0 , 5);
    EXPECT_THROW(storage.moveContainer("0_0_0", -1, 0, 0), std::invalid_argument);
    EXPECT_THROW(storage.moveContainer("3_0_7", 1, 0, 0), std::invalid_argument);
    EXPECT_THROW(storage.moveContainer("0_0_3", 1, 0, 0), std::invalid_argument);
    EXPECT_THROW(storage.moveContainer("0_0_5", 1, 0, 0), std::invalid_argument);
    storage.moveContainer("0_0_5", 90, 8, 0);
    std::string result = storage.getInfo();
    EXPECT_EQ(result.find("90_8_0") != std::string::npos, true);
    EXPECT_EQ(result.find("0_0_5") != std::string::npos, false);
}


TEST(StorageTest, Rotate){
    Storage storage(1, 100, 100, 100, 20.0);
    IContainer* container = new FragileContainer("_", "Cargo B", 2, 5, 2, 23.5, 2.5, 11.3);
    storage.addContainer(container, 0, 0, 0);
    container =  new Container("_", "Cargo A", 1, 4, 1, 21.2, 1.1);
    storage.addContainer(container, 0, 0 , 3);
    container =  new Container("_", "Cargo A", 14, 4, 12, 21.2, 1.1);
    storage.addContainer(container, 16, 16 , 0);
    container =  new Container("_", "Cargo A", 7, 4, 3, 21.2, 1.1);
    storage.addContainer(container, 80, 80 , 0);
    container =  new Container("_", "Cargo A", 1, 1, 1, 21.2, 1.1);
    storage.addContainer(container, 0, 0 , 5);
    EXPECT_THROW(storage.rotateContainer("0_0_3", 0), std::invalid_argument);
    EXPECT_THROW(storage.rotateContainer("0_0_0", 0), std::invalid_argument);
    storage.rotateContainer("80_80_0", 5);
    auto i = storage.find("80_80_0");
    EXPECT_EQ(i.second->getLength(), 3);
    EXPECT_EQ(i.second->getWidth(), 4);
    EXPECT_EQ(i.second->getHeight(), 7);
    Storage st(1, 100, 100, 2, 20.0);
    container =  new Container("_", "Cargo A", 7, 4, 1, 21.2, 1.1);
    st.addContainer(container, 0, 0, 0);
    EXPECT_THROW(st.rotateContainer("0_0_0", 3), std::invalid_argument);
    i = st.find("0_0_0");
    EXPECT_EQ(i.second->getLength(), 7);
    EXPECT_EQ(i.second->getWidth(), 4);
    EXPECT_EQ(i.second->getHeight(), 1);
    EXPECT_THROW(st.rotateContainer("99_0_0", 3), std::invalid_argument);
}


TEST(StorageTest, Remove){
    Storage storage(1, 32, 32, 16, 20.0);
    IContainer* container = new FragileContainer("_", "Cargo B", 5, 5, 2, 23.5, 2.5, 11.3);
    storage.addContainer(container, 0, 0, 0);//1
    container =  new Container("_", "Cargo A", 4, 4, 1, 21.2, 1.1);
    storage.addContainer(container, 0, 0 , 3);//2
    container =  new Container("_", "Cargo A", 14, 4, 12, 21.2, 1.1);
    storage.addContainer(container, 5, 16 , 0);//3
    container =  new Container("_", "Cargo A", 7, 4, 3, 21.2, 1.1);
    storage.addContainer(container, 5, 16 , 13);//4
    container =  new Container("_", "Cargo A", 4, 3, 1, 21.2, 1.1);
    storage.addContainer(container, 0, 0 , 5);//5
    container =  new Container("_", "Cargo A", 4, 3, 1, 21.2, 1.1);
    storage.addContainer(container, 0, 0 , 7);//6
    container =  new Container("_", "Cargo A", 3, 2, 3, 21.2, 1.1);
    storage.addContainer(container, 0, 0 , 9);//7
    container =  new Container("_", "Cargo A", 1, 2, 1, 21.2, 1.1);
    storage.addContainer(container, 0, 0 , 13);//8
    std::cout << storage.getInfo() << std::endl;
    std::cout<< std::endl;
    EXPECT_THROW(storage.removeContainer("90_1056_14"), std::invalid_argument);
    storage.removeContainer("0_0_13");
    std::vector<std::string> result = storage.getListContainers();
    EXPECT_EQ(result.size(), 7);
    std::cout << storage.getInfo() << std::endl;
    std::cout<< std::endl;
    storage.removeContainer("0_0_3");
    result = storage.getListContainers();
    EXPECT_EQ(result.size(), 6);
    std::cout << storage.getInfo() << std::endl;
    std::cout<< std::endl;
    storage.removeContainer("5_16_0");
    result = storage.getListContainers();
    EXPECT_EQ(result.size(), 5);
    std::cout << storage.getInfo() << std::endl;
    std::cout<< std::endl;
    EXPECT_NO_THROW(storage.getSize(100, 100, 32));
    storage.getInfo();
    Storage st(2, 10, 1, 3, 20.0);
    container =  new Container("_", "Cargo A", 1, 1, 1, 21.2, 1.1);
    st.addContainer(container, 0, 0 , 0);
    container =  new Container("_", "Cargo A", 8, 1, 1, 21.2, 1.1);
    st.addContainer(container,2, 0, 0);
    container =  new Container("_", "Cargo A", 10, 1, 1, 21.2, 1.1);
    st.addContainer(container,0, 0, 2);
    EXPECT_THROW(st.removeContainer("2_0_0"), std::invalid_argument);
    Storage lastT(2, 11, 1, 3, 20.0);
    container =  new Container("_", "Cargo A", 2, 1, 1, 21.2, 1.1);
    lastT.addContainer(container, 0, 0, 0);
    container =  new Container("_", "Cargo A", 8, 1, 1, 21.2, 1.1);
    lastT.addContainer(container, 3, 0, 0);
    container =  new Container("_", "Cargo A", 1, 1, 1, 21.2, 1.1);
    lastT.addContainer(container,0, 0, 2);
    container =  new Container("_", "Cargo A", 1, 1, 1, 21.2, 1.1);
    lastT.addContainer(container,2, 0, 2);
    container =  new Container("_", "Cargo A", 7, 1, 1, 21.2, 1.1);
    lastT.addContainer(container,4, 0, 2);
    EXPECT_NO_THROW(lastT.removeContainer("3_0_0"));
}


 TEST(StorageTest, howContainersInStorage){
     Storage storage(1, 10, 5, 1, 20.0);
     IContainer* container = new Container("_", "Cargo B", 2, 1, 1, 23.5, 2.5);
     storage.addContainer(container, 0, 0, 0);
     container = new Container("_", "Cargo B", 1, 1, 1, 23.5, 2.5);
    int max = storage.howContainer(container);
     std::cout << max << std::endl;
     EXPECT_EQ(max, 14);
        delete container;
}


void checkCheker(Storage& storage, IContainer* container, ContainerPosition<int> pos){
    if(((*container).isType() == "Fragile and Refraged Container" ))
    {
        throw std::invalid_argument("Proved");
    }
}

TEST(StorageTest, Checker){
    Storage storage(1, 10, 5, 1, 29.0);
    storage.addExternalCheckFunction(checkCheker);
    IContainer* container = new FragileRefragedContainer("_", "Cargo B", 2, 1, 1, 23.5, 2.5, 24.5, 24.5);
    EXPECT_THROW(storage.addContainer(container, 0, 0, 0), std::invalid_argument);
    delete container;
}



TEST(StorageTest, RemoveHard){
    Storage st =  Storage(1, 100, 100, 32, 23.4);
    IContainer* container1 = new Container("_", "Cargo B", 6, 6, 1, 23.5, 2.5);
    IContainer* container2 = new Container("_", "Cargo B", 8, 10, 1, 23.5, 2.5);
    IContainer* container3 = new Container("_", "Cargo B", 10, 8, 1, 23.5, 2.5);
    IContainer* container4 = new Container("_", "Cargo B", 9, 9, 1, 23.5, 2.5);
    IContainer* container5 = new Container("_", "Cargo B", 8, 4, 1, 23.5, 2.5);
    IContainer* container6 = new Container("_", "Cargo B", 5, 2, 1, 23.5, 2.5);
    IContainer* container7 = new Container("_", "Cargo B", 1, 6, 1, 23.5, 2.5);
    IContainer* container8 = new Container("_", "Cargo B", 3, 2, 1, 23.5, 2.5);
    IContainer* container9 = new Container("_", "Cargo B", 2, 4, 1, 23.5, 2.5);
    IContainer* container10 = new Container("_", "Cargo B", 4, 10, 1, 23.5, 2.5);
    IContainer* container11 = new Container("_", "Cargo B", 7, 1, 1, 23.5, 2.5);
    IContainer* container12 = new Container("_", "Cargo B", 4, 3, 1, 23.5, 2.5);
    IContainer* container13 = new Container("_", "Cargo B", 8, 1, 1, 23.5, 2.5);
    IContainer* container14 = new Container("_", "Cargo B", 2, 3, 1, 23.5, 2.5);
    IContainer* container15 = new Container("_", "Cargo B", 7, 8, 1, 23.5, 2.5);
    IContainer* container16 = new Container("_", "Cargo B", 2, 7, 1, 23.5, 2.5);
    IContainer* container17 = new Container("_", "Cargo B", 7, 5, 1, 23.5, 2.5);
    IContainer* container18 = new Container("_", "Cargo B", 4, 1, 1, 23.5, 2.5);
    IContainer* container19 = new Container("_", "Cargo B", 8, 3, 1, 23.5, 2.5);
    st.addContainer(container1, 12, 0, 0);
    st.addContainer(container4, 2, 0, 0);
    st.addContainer(container10, 5, 20, 0);
    st.addContainer(container12, 0, 20, 0);
    st.addContainer(container2, 10, 20, 0);
    st.addContainer(container3, 8, 20, 2);
    st.addContainer(container5, 6, 0, 2);
    st.addContainer(container6, 0, 0, 2);
    st.addContainer(container11, 0, 20, 2);
    st.addContainer(container8, 0, 0, 4);
    st.addContainer(container9, 4, 0, 4);
    st.addContainer(container13, 0, 20, 4);
    st.addContainer(container7, 4, 0, 6);
    st.addContainer(container14, 0, 20, 6);
    st.addContainer(container15, 1, 0, 8);
    st.addContainer(container16, 0, 20, 8);
    st.addContainer(container18, 0, 20, 10);
    st.addContainer(container19, 0, 20, 12);
    st.addContainer(container17, 0, 20, 14);
    EXPECT_NO_THROW(st.removeContainer("10_20_0"));
    std::cout << st.getInfo() << std::endl;
    std::vector<std::string> result = st.getListContainers();
    EXPECT_EQ(result.size(), 18);
    Storage st1 =  Storage(1, 100, 100, 32, 23.4);
    container1 = new Container("_", "Cargo B", 70, 80, 27, 23.5, 2.5);
    container2 = new Container("_", "Cargo B", 1, 1, 1, 23.5, 2.5);
    container3 = new Container("_", "Cargo B", 1, 1, 1, 23.5, 2.5);
    st1.addContainer(container1, 0, 0, 0);
    st1.addContainer(container2, 0, 0, 28);
    st1.addContainer(container3, 98, 98, 0);
    st1.removeContainer("0_0_0");
    std::cout << st1.getInfo() << std::endl;
    std::vector<std::string> result1 = st1.getListContainers();
    EXPECT_EQ(result1.size(), 2);
}


TEST(OctreeTest, TestIterator){
    Storage st =  Storage(1, 100, 100, 32, 23.4);
    IContainer* container1 = new Container("_", "Cargo B", 6, 6, 1, 23.5, 2.5);
    IContainer* container2 = new Container("_", "Cargo B", 8, 10, 1, 23.5, 2.5);
    IContainer* container3 = new Container("_", "Cargo B", 10, 8, 1, 23.5, 2.5);
    IContainer* container4 = new Container("_", "Cargo B", 9, 9, 1, 23.5, 2.5);
    IContainer* container5 = new Container("_", "Cargo B", 8, 4, 1, 23.5, 2.5);
    IContainer* container6 = new Container("_", "Cargo B", 5, 2, 1, 23.5, 2.5);
    IContainer* container7 = new Container("_", "Cargo B", 1, 6, 1, 23.5, 2.5);
    IContainer* container8 = new Container("_", "Cargo B", 3, 2, 1, 23.5, 2.5);
    IContainer* container9 = new Container("_", "Cargo B", 2, 4, 1, 23.5, 2.5);
    IContainer* container10 = new Container("_", "Cargo B", 4, 10, 1, 23.5, 2.5);
    IContainer* container11 = new Container("_", "Cargo B", 7, 1, 1, 23.5, 2.5);
    IContainer* container12 = new Container("_", "Cargo B", 4, 3, 1, 23.5, 2.5);
    IContainer* container13 = new Container("_", "Cargo B", 8, 1, 1, 23.5, 2.5);
    IContainer* container14 = new Container("_", "Cargo B", 2, 3, 1, 23.5, 2.5);
    IContainer* container15 = new Container("_", "Cargo B", 7, 8, 1, 23.5, 2.5);
    IContainer* container16 = new Container("_", "Cargo B", 2, 7, 1, 23.5, 2.5);
    IContainer* container17 = new Container("_", "Cargo B", 7, 5, 1, 23.5, 2.5);
    IContainer* container18 = new Container("_", "Cargo B", 4, 1, 1, 23.5, 2.5);
    IContainer* container19 = new Container("_", "Cargo B", 8, 3, 1, 23.5, 2.5);
    st.addContainer(container1, 12, 0, 0);
    st.addContainer(container4, 2, 0, 0);
    st.addContainer(container10, 5, 20, 0);
    st.addContainer(container12, 0, 20, 0);
    st.addContainer(container2, 10, 20, 0);
    st.addContainer(container3, 8, 20, 2);
    st.addContainer(container5, 6, 0, 2);
    st.addContainer(container6, 0, 0, 2);
    st.addContainer(container11, 0, 20, 2);
    st.addContainer(container8, 0, 0, 4);
    st.addContainer(container9, 4, 0, 4);
    st.addContainer(container13, 0, 20, 4);
    st.addContainer(container7, 4, 0, 6);
    st.addContainer(container14, 0, 20, 6);
    st.addContainer(container15, 1, 0, 8);
    st.addContainer(container16, 0, 20, 8);
    st.addContainer(container18, 0, 20, 10);
    st.addContainer(container19, 0, 20, 12);
    st.addContainer(container17, 0, 20, 14);
    EXPECT_NO_THROW(st.removeContainer("0_20_2"));
    EXPECT_THROW(st.find("0_20_2"), std::runtime_error);
    EXPECT_NO_THROW(st.find("2_0_0"));
    auto it = st.find("12_0_0");
    EXPECT_EQ(it.second->getId(), "12_0_0");
}




int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}