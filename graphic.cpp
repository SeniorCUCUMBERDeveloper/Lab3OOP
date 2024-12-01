#ifndef GRAPHIC
#define GRAPHIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "./Storage/Storage.hpp"
#include "./Container/I/IContainer.hpp"
#include "./Octree/ContainerPosition.hpp"

template <typename T>
void drawContainer(const ContainerPosition<T>& container) {
    glBegin(GL_QUADS);

    // Задняя грань
    glColor3f(1.0f, 0.0f, 0.0f); // Красный
    glVertex3f(container.LLDown.x, container.LLDown.y, container.LLDown.z); // V1
    glVertex3f(container.LRDown.x, container.LRDown.y, container.LRDown.z); // V2
    glVertex3f(container.LRUp.x, container.LRUp.y, container.LRUp.z);       // V6
    glVertex3f(container.LLUp.x, container.LLUp.y, container.LLUp.z);       // V5

    // Передняя грань
    glColor3f(0.0f, 1.0f, 0.0f); // Зеленый
    glVertex3f(container.RRDown.x, container.RRDown.y, container.RRDown.z); // V4
    glVertex3f(container.LRDown.x, container.LRDown.y, container.LRDown.z); // V2
    glVertex3f(container.LRUp.x, container.LRUp.y, container.LRUp.z);       // V6
    glVertex3f(container.RRUp.x, container.RRUp.y, container.RRUp.z);       // V7

    // Левая грань
    glColor3f(0.0f, 0.0f, 1.0f); // Синий
    glVertex3f(container.LLDown.x, container.LLDown.y, container.LLDown.z); // V1
    glVertex3f(container.LLUp.x, container.LLUp.y, container.LLUp.z);       // V5
    glVertex3f(container.RLUp.x, container.RLUp.y, container.RLUp.z);       // V8
    glVertex3f(container.RLDown.x, container.RLDown.y, container.RLDown.z); // V3

    // Правая грань
    glColor3f(1.0f, 1.0f, 0.0f); // Желтый
    glVertex3f(container.RRDown.x, container.RRDown.y, container.RRDown.z); // V4
    glVertex3f(container.RRUp.x, container.RRUp.y, container.RRUp.z);       // V7
    glVertex3f(container.LRUp.x, container.LRUp.y, container.LRUp.z);       // V6
    glVertex3f(container.LRDown.x, container.LRDown.y, container.LRDown.z); // V2

    // Верхняя грань
    glColor3f(1.0f, 0.0f, 1.0f); // Пурпурный
    glVertex3f(container.LLUp.x, container.LLUp.y, container.LLUp.z);       // V5
    glVertex3f(container.LRUp.x, container.LRUp.y, container.LRUp.z);       // V6
    glVertex3f(container.RRUp.x, container.RRUp.y, container.RRUp.z);       // V7
    glVertex3f(container.RLUp.x, container.RLUp.y, container.RLUp.z);       // V8

    // Нижняя грань
    glColor3f(0.0f, 1.0f, 1.0f); // Бирюзовый
    glVertex3f(container.LLDown.x, container.LLDown.y, container.LLDown.z); // V1
    glVertex3f(container.RLDown.x, container.RLDown.y, container.RLDown.z); // V3
    glVertex3f(container.RRDown.x, container.RRDown.y, container.RRDown.z); // V4
    glVertex3f(container.LRDown.x, container.LRDown.y, container.LRDown.z); // V2

    glEnd();
}

void draw(Storage& storage) {
    auto containers = storage.getALLcontainers();
    if (containers.empty()) {
        return;
    }

    // Инициализация GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Container Visualization", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error initializing GLEW: " << glewGetErrorString(err) << "\n";
        return;
    }

    glViewport(0, 0, 800, 600);
    glClearColor(0.0f, 0.0f, 0.5f, 1.0f); // Цвет фона
    glEnable(GL_DEPTH_TEST); // Включаем тест глубины

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f); // Углы обзора
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0.0f, 5.0f, 10.0f,  // Позиция камеры
                  0.0f, 0.0f, 0.0f,  // Точка назначения
                  0.0f, 1.0f, 0.0f); // Вектор вверх

        // Отрисовка контейнеров
        for (auto& container : containers) {
            drawContainer(container.first); // Предполагается, что container.first - это ContainerPosition<int>
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}

#endif