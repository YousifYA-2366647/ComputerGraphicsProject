#include "UIPanel.h"
#include <glad/glad.h>
#include <glm-1.0.1/glm/gtc/matrix_transform.hpp>
#include <cmath>

// ----------- UIButton -----------
UIButton::UIButton(glm::vec2 pos, glm::vec2 size, std::function<void()> cb) {
    this->pos = pos;
    this->size = size;
    this->callback = cb;
}

void UIButton::draw(Shader& shader, const glm::mat4& panelModel) {
    shader.use();

    float x0 = pos.x, y0 = pos.y;
    float x1 = pos.x + size.x, y1 = pos.y + size.y;
    float cx = (x0 + x1) / 2 - 0.5f;
    float cy = (y0 + y1) / 2 - 0.5f;
    float w = (x1 - x0) * 0.5f;
    float h = (y1 - y0) * 0.2f;

    glm::vec4 color;

    if (size.x > 0.5f) {
        color = glm::vec4(0.1f, 0.7f, 0.2f, 1.0f); // Helder groen
    }
    else if (pos.x < 0.2f) {
        color = glm::vec4(0.8f, 0.2f, 0.2f, 1.0f); // Helder rood
    }
    else {
        color = glm::vec4(0.2f, 0.2f, 0.8f, 1.0f); // Helder blauw
    }

    shader.setVec4("panelColor", color);

    glm::vec3 verts[4] = {
    glm::vec3(x0 - 0.5f, y0 - 0.5f, 0.1f),  
    glm::vec3(x1 - 0.5f, y0 - 0.5f, 0.1f),
    glm::vec3(x1 - 0.5f, y1 - 0.5f, 0.1f),
    glm::vec3(x0 - 0.5f, y1 - 0.5f, 0.1f)
    };
    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };
    unsigned int vertexArray, vertexBuffer, elementBuffer;
    glGenVertexArrays(1, &vertexArray);
    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &elementBuffer);
    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    shader.setMat4("model", panelModel);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    shader.setVec4("panelColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)); // Wit

    
    if (size.x > 0.5f) {
        shader.setVec4("panelColor", glm::vec4(0.1f, 0.7f, 0.2f, 1.0f)); // Groene knop

        shader.setVec4("panelColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)); // Witte tekst

        float zOffset = 0.2f; 

        
    }

    // - knop: horizontaal streepje
    else if (pos.x < 0.2f) {
        glm::vec3 minusVerts[4] = {
            glm::vec3(cx - w * 0.8f, cy - h * 0.6f, 0.12f), 
            glm::vec3(cx + w * 0.8f, cy - h * 0.6f, 0.12f),
            glm::vec3(cx + w * 0.8f, cy + h * 0.6f, 0.12f),
            glm::vec3(cx - w * 0.8f, cy + h * 0.6f, 0.12f)
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(minusVerts), minusVerts, GL_STATIC_DRAW);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    // + knop: horizontaal en verticaal streepje
    else {
        
        glm::vec3 plusH[4] = {
            glm::vec3(cx - w * 0.8f, cy - h * 0.6f, 0.12f),  
            glm::vec3(cx + w * 0.8f, cy - h * 0.6f, 0.12f),
            glm::vec3(cx + w * 0.8f, cy + h * 0.6f, 0.12f),
            glm::vec3(cx - w * 0.8f, cy + h * 0.6f, 0.12f)
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(plusH), plusH, GL_STATIC_DRAW);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        
        glm::vec3 plusV[4] = {
            glm::vec3(cx - h * 0.6f, cy - w * 0.8f, 0.12f), 
            glm::vec3(cx + h * 0.6f, cy - w * 0.8f, 0.12f),
            glm::vec3(cx + h * 0.6f, cy + w * 0.8f, 0.12f),
            glm::vec3(cx - h * 0.6f, cy + w * 0.8f, 0.12f)
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(plusV), plusV, GL_STATIC_DRAW);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    glDeleteVertexArrays(1, &vertexArray);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &elementBuffer);
}



bool UIButton::isInside(const glm::vec2& current) {
    return current.x >= pos.x && current.x <= pos.x + size.x &&
        current.y >= pos.y && current.y <= pos.y + size.y;
}

void UIButton::onClick() {
    if (callback) callback();
}

// ----------- UIPanel -----------
UIPanel::UIPanel(glm::vec3 pos, glm::vec2 size, glm::vec3 normal, glm::vec3 up)
    : pos(pos), size(size), normal(glm::normalize(normal)), up(glm::normalize(up)) {
}

void UIPanel::draw(Shader& shader, const glm::mat4& view, const glm::mat4& proj) {
    if (!visible) return;
    shader.use();
    glm::vec3 right = glm::normalize(glm::cross(normal, up));
    glm::mat4 model(1.0f);
    model[0] = glm::vec4(right * size.x, 0);
    model[1] = glm::vec4(up * size.y, 0);
    model[2] = glm::vec4(normal, 0);
    model[3] = glm::vec4(pos, 1);

    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", proj);

    shader.setVec4("panelColor", glm::vec4(0.2f, 0.2f, 0.2f, 0.85f)); 


    glm::vec3 verts[4] = {
        glm::vec3(-0.5f, -0.5f, 0),
        glm::vec3(0.5f, -0.5f, 0),
        glm::vec3(0.5f, 0.5f, 0),
        glm::vec3(-0.5f, 0.5f, 0)
    };
    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };
    unsigned int vertexArray, vertexBuffer, elementBuffer;
    glGenVertexArrays(1, &vertexArray);
    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &elementBuffer);
    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glDeleteVertexArrays(1, &vertexArray);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &elementBuffer);

   
    for (auto* el : elements) el->draw(shader, model);
}


int UIPanel::pickElement(const glm::vec3& rayStart, const glm::vec3& rayDir) {
    float dotNormalRay = glm::dot(normal, rayDir);
    if (std::abs(dotNormalRay) < 1e-6f) return 6;
    float t = glm::dot(pos - rayStart, normal) / dotNormalRay;
    if (t < 0) return -1;
    glm::vec3 hit = rayStart + t * rayDir;
    glm::vec3 right = glm::normalize(glm::cross(normal, up));
    glm::vec3 current = hit - pos;
    float x = glm::dot(current, right) / size.x + 0.5f;
    float y = glm::dot(current, up) / size.y + 0.5f;
    if (x < 0 || x > 1 || y < 0 || y > 1) return 6;
    glm::vec2 currentPanel(x, y);
    for (size_t i = 0; i < elements.size(); ++i) {

        if (elements[i]->isInside(currentPanel))
        {
			printf("Element %d clicked\n", i);
            return int(i);
        }
    }
    return -1;
}

void UIPanel::processClick(int index) {
    printf("Element %d\n", (int)elements.size());
    if (index >= 0 && index < (int)elements.size()) {
		printf("Processing click on element %d\n", index);
		printf("Element %d\n", (int)elements.size());
        elements[index]->onClick();
    }
	
		
}
	

UIPanel::~UIPanel() {
    for (auto* el : elements) delete el;
}
// Start drag
void UIPanel::startDrag(const glm::vec3& hitPoint) {
    dragging = true;
    dragOffset = pos - hitPoint;
}

// Stop drag
void UIPanel::stopDrag() {
    dragging = false;
}

// Update drag
void UIPanel::updateDrag(const glm::vec3& hitPoint) {
    if (dragging) {
        pos = hitPoint + dragOffset;
    }
}
void UIPanel::dragDepth(float amount) {
    pos += normal * amount;
}
