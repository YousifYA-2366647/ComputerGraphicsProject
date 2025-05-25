#pragma once
#include <vector>
#include <functional>
#include <glm-1.0.1/glm/glm.hpp>
#include "Shader.h"
#include "Camera.h"

class UIElement {
public:
    
    glm::vec2 pos, size;
    virtual void draw(Shader& shader, const glm::mat4& panelModel) = 0;
    virtual bool isInside(const glm::vec2& local) = 0;
    virtual void onClick() = 0;
    virtual ~UIElement() = default;
};

class UIButton : public UIElement {
public:
    std::function<void()> callback;
    UIButton(glm::vec2 pos, glm::vec2 size, std::function<void()> cb);
    void draw(Shader& shader, const glm::mat4& panelModel) override;
    bool isInside(const glm::vec2& current) override;
    void onClick() override;
    

    
};

class UIPanel {
public:
    glm::vec3 pos;
    glm::vec2 size;     
    glm::vec3 normal;   
    glm::vec3 up;       
    std::vector<UIElement*> elements;
    bool dragging = false;
    glm::vec3 dragOffset;
    bool visible = false;
    UIPanel(glm::vec3 pos, glm::vec2 size, glm::vec3 normal, glm::vec3 up);
    void draw(Shader& shader, const glm::mat4& view, const glm::mat4& proj);
    int pickElement(const glm::vec3& rayStart, const glm::vec3& rayDir);
    void processClick(int index); 
	void startDrag(const glm::vec3& hitPoint);
	void updateDrag(const glm::vec3& hitPoint);
	void stopDrag();
    void dragDepth(float amount);

    ~UIPanel();
};
