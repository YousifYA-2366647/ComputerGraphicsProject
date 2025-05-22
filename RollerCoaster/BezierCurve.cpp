#include "BezierCurve.h"


BezierCurve::BezierCurve(std::vector<Vertex> controlPoints) {
	this->controlPoints = controlPoints;

	setupCurve();
}

BezierCurve::~BezierCurve() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void BezierCurve::Draw(Shader& shader) {
	shader.use();
	glBindVertexArray(VAO);
	glDrawArrays(GL_LINE_STRIP, 0, lookupTable.size());
}

void BezierCurve::setupCurve() {
	int steps = 1000;
	float totalLength = 0.0f;
	glm::vec3 prevPoint = calculateBezierPoint(0.0f);
	glm::vec3 prevColor = calculateBezierColor(0.0f);
	lookupTable.push_back({ 0.0f, 0.0f, {prevPoint, prevColor} });

	for (int i = 0; i < steps; i++) {
		float sample = static_cast<float>(i) / (steps - 1);

		glm::vec3 position = calculateBezierPoint(sample);

		glm::vec3 color = calculateBezierColor(sample);

		float segmentLength = glm::distance(prevPoint, position);
		totalLength += segmentLength;

		lookupTable.push_back({totalLength, sample, {position, color}});

		prevPoint = position;
	}

	std::vector<Vertex> vertexData;
	vertexData.reserve(lookupTable.size());
	for (const auto& entry : lookupTable) {
		vertexData.push_back(entry.arcVertex);
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(Vertex), vertexData.data(), GL_STATIC_DRAW);

	// Assign the position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
	glEnableVertexAttribArray(0);

	// Assign the color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

glm::vec3 BezierCurve::calculateBezierPoint(float sample) {
	int n = controlPoints.size();
	glm::vec3 result = glm::vec3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < n; i++) {
		float coefficient = static_cast<float>(combination(n, i)) * std::pow(1.0f - sample, n - 1 - i) * std::pow(sample, i);
		result += coefficient * controlPoints[i].Position;
	}

	return result;
}

glm::vec3 BezierCurve::calculateBezierColor(float t) {
	int n = controlPoints.size() - 1;
	glm::vec3 result = glm::vec3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i <= n; i++) {
		float coefficient = static_cast<float>(combination(n, i)) * std::pow(1.0f - t, n - i) * std::pow(t, i);
		result += coefficient * controlPoints[i].Color;
	}

	return result;
}

glm::vec3 BezierCurve::calculateBezierDerivative(float t) {
	int n = controlPoints.size() - 1;
	glm::vec3 result(0.0f);

	for (int i = 0; i < n; ++i) {
		glm::vec3 diff = controlPoints[i + 1].Position - controlPoints[i].Position;
		float bernstein = combination(n - 1, i) * pow(t, i) * pow(1 - t, n - 1 - i);
		result += diff * bernstein;
	}

	result *= static_cast<float>(n);
	return result;
}

int BezierCurve::combination(int n, int r) {
	if (r > n) {
		return 0;
	}
	if (r == 0 || r == n) {
		return 1;
	}
	return factorial(n) / (factorial(r) * factorial(n - r));
}

int BezierCurve::factorial(int n) {
	if (n <= 1) {
		return 1;
	}
	int res = 1;
	for (int i = 2; i <= n; i++) {
		res = res * i;
	}
	return res;
}

float BezierCurve::getTotalLength() const {
	return lookupTable.back().arcLength;
}