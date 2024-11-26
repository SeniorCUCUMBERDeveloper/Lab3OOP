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
    std::string result = storage.getInfo();
    EXPECT_EQ(result.find("0_0_0") != std::string::npos, true);
    EXPECT_EQ(result.find("0_0_3") != std::string::npos, true);
    EXPECT_EQ(result.find("16_16_0") != std::string::npos, true);
    EXPECT_EQ(result.find("80_80_0") != std::string::npos, true);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}