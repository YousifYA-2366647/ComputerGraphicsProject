#include "BezierCurve.h"


BezierCurve::BezierCurve(std::vector<glm::vec3> controlPoints) {
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
	glDrawArrays(GL_LINE_STRIP, 0, vertices.size());
}

void BezierCurve::setupCurve() {
	int steps = 1000;

	for (int i = 0; i < steps; i++) {
		float sample = static_cast<float>(i) / (steps - 1);

		vertices.push_back(calculateBezierCurve(sample));
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	// lower curve
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

	// Assign the position attribute (you can find the values at the top in the variable 'vertices')
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

glm::vec3 BezierCurve::calculateBezierCurve(float sample) {
	float oneMinusSample = 1 - sample;
	float oneMinusSampleSquared = oneMinusSample * oneMinusSample;
	float oneMinusSampleCubed = oneMinusSampleSquared * oneMinusSample;

	float sampleSquared = sample * sample;
	float sampleCubed = sample * sampleSquared;

	return oneMinusSampleCubed * controlPoints[0] +
		3 * oneMinusSampleSquared * sample * controlPoints[1] +
		3 * oneMinusSample * sampleSquared * controlPoints[2] +
		sampleCubed * controlPoints[3];
}