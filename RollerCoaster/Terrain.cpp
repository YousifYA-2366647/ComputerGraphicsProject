#include "Terrain.h"

Terrain::Terrain() {
	vertices = {
		// Position								// Color								// Texture
		{glm::vec3(0.5f, -0.2f, 0.5f),			glm::vec3(0.0f, 1.0f, 0.0f),			glm::vec2(1.0f, 1.0f)},		// bottom right (green)
		{glm::vec3(0.5f, -0.2f, -0.5f),			glm::vec3(1.0f, 0.0f, 0.0f),			glm::vec2(1.0f, 0.0f)},		// top right (red)
		{glm::vec3(-0.5f, -0.2f, -0.5f),		glm::vec3(0.0f, 0.0f, 1.0f),			glm::vec2(0.0f, 0.0f)},		// top left (blue)
		{glm::vec3(-0.5f, -0.2f, 0.5f),			glm::vec3(1.0f, 1.0f, 1.0f),			glm::vec2(0.0f, 1.0f)},		// bottom left (white)
	};

	indices = {
		0, 1, 3,
		2, 1, 3
	};

	setupTerrain();
}

Terrain::~Terrain() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteTextures(1, &textureId);
	delete terrainShader;
}

void Terrain::Draw() {
	terrainShader->use();
	terrainShader->setInt("textureSampler", 0);
	terrainShader->setMat4("model", model);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	for (Building* building : buildings) {
		building->Draw();
	}
}

void Terrain::setupTerrain() {
	terrainShader = new Shader("terrainVertexShader.vert", "terrainFragmentShader.frag");

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// Assign the position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
	glEnableVertexAttribArray(0);

	// Assign the color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
	glEnableVertexAttribArray(1);

	// Assign the texture attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Texture));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	std::string path = "model/Yokohama3/negy.jpg";

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	stbi_set_flip_vertically_on_load(false);
	float size = 150.0f;

	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(size));
	terrainShader->use();
	terrainShader->setMat4("model", model);

	std::vector<glm::vec3> positions = {
		glm::vec3(-50.0f, 0.0f, -50.0f),
		glm::vec3(-30.0f, 0.0f, -10.0f),
		glm::vec3(0.0f, 0.0f,  25.0f),
		glm::vec3(40.0f, 0.0f, -35.0f),
		glm::vec3(60.0f, 0.0f,  60.5f),
		glm::vec3(-60.0f, 0.0f,  55.0f),
		glm::vec3(25.0f, 0.0f, -60.0f),
		glm::vec3(40.0f, 0.0f,  10.0f),
		glm::vec3(0.0f, 0.0f, -55.0f),
		glm::vec3(-10.0f, 0.0f, -22.0f)
	};

	for (size_t i = 0; i < positions.size(); ++i) {
		float factor = static_cast<float>((i % 5) + 1); // size variation
		glm::vec3 scale = glm::vec3(factor * 2.0f, factor * 10.0f, factor * 2.0f);

		positions[i].y = -30.0f + scale.y / 1.01f;

		Building* newBuilding = new Building("model/buildingModel.glb");
		newBuilding->translateModel(positions[i]);
		newBuilding->scaleModel(scale);

		buildings.push_back(newBuilding);
	}
}

void Terrain::setModel(glm::mat4& newModel) {
	this->terrainShader->use();
	terrainShader->setMat4("model", newModel);
	this->model = newModel;
}

void Terrain::setView(glm::mat4& newView) {
	this->terrainShader->use();
	terrainShader->setMat4("view", newView);

	for (Building* building : buildings) {
		building->setView(newView);
	}
}

void Terrain::setProjection(glm::mat4& newProjection) {
	this->terrainShader->use();
	terrainShader->setMat4("projection", newProjection);

	for (Building* building : buildings) {
		building->setProjection(newProjection);
	}
}

std::vector<Shader*> Terrain::getBuildingShaders() {
	std::vector<Shader*> shaders;
	for (Building* building : buildings) {
		shaders.push_back(building->buildingShader);
	}

	return shaders;
}