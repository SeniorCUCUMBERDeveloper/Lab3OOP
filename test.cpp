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
    Storage storageCopy(storage);
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
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}