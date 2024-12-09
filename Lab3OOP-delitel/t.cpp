#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>
#include <iostream>

void drawCube(float x, float y, float z) {
    glBegin(GL_QUADS);

    // Передняя грань
    glColor3f(1.0f, 0.0f, 0.0f); // Красный
    glVertex3f(x - 0.5f, y - 0.5f, z + 0.5f);
    glVertex3f(x + 0.5f, y - 0.5f, z + 0.5f);
    glVertex3f(x + 0.5f, y + 0.5f, z + 0.5f);
    glVertex3f(x - 0.5f, y + 0.5f, z + 0.5f);

    // Задняя грань
    glColor3f(0.0f, 1.0f, 0.0f); // Зеленый
    glVertex3f(x - 0.5f, y - 0.5f, z - 0.5f);
    glVertex3f(x + 0.5f, y - 0.5f, z - 0.5f);
    glVertex3f(x + 0.5f, y + 0.5f, z - 0.5f);
    glVertex3f(x - 0.5f, y + 0.5f, z - 0.5f);

    // Левые и правые грани
    glColor3f(0.0f, 0.0f, 1.0f); // Синий
    glVertex3f(x - 0.5f, y - 0.5f, z - 0.5f);
    glVertex3f(x - 0.5f, y - 0.5f, z + 0.5f);
    glVertex3f(x - 0.5f, y + 0.5f, z + 0.5f);
    glVertex3f(x - 0.5f, y + 0.5f, z - 0.5f);

    glVertex3f(x + 0.5f, y - 0.5f, z - 0.5f);
    glVertex3f(x + 0.5f, y - 0.5f, z + 0.5f);
    glVertex3f(x + 0.5f, y + 0.5f, z + 0.5f);
    glVertex3f(x + 0.5f, y + 0.5f, z - 0.5f);

    // Верхняя и нижняя грани
    glColor3f(1.0f, 1.0f, 0.0f); // Желтый
    glVertex3f(x - 0.5f, y + 0.5f, z - 0.5f);
    glVertex3f(x + 0.5f, y + 0.5f, z - 0.5f);
    glVertex3f(x + 0.5f, y + 0.5f, z + 0.5f);
    glVertex3f(x - 0.5f, y + 0.5f, z + 0.5f);

    glColor3f(0.5f, 0.5f, 0.5f); // Серый
    glVertex3f(x - 0.5f, y - 0.5f, z - 0.5f);
    glVertex3f(x + 0.5f, y - 0.5f, z - 0.5f);
    glVertex3f(x + 0.5f, y - 0.5f, z + 0.5f);
    glVertex3f(x - 0.5f, y - 0.5f, z + 0.5f);

    glEnd();
}

int main() {
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.majorVersion = 3; // Измените на более современную версию, например, 3
    settings.minorVersion = 3;
    
    // Создание окна с заданными параметрами
    sf::Window window(sf::VideoMode(800, 600), "3D Warehouse with SFML and OpenGL", sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);
    // Проверка успешности создания окна
    if (!window.isOpen()) {
        std::cerr << "Failed to create window!" << std::endl;
        return -1;
    }

    // Инициализация OpenGL
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);

    // Главный цикл приложения
    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        // Установка камеры
        glTranslatef(0.0f, 0.0f, -5.0f);

        // Рисуем контейнеры
        for (int x = -2; x <= 2; ++x) {
            for (int z = -2; z <= 2; ++z) {
                drawCube(x, 0.0f, z);
            }
        }

        window.display();
    }

    return 0;
}