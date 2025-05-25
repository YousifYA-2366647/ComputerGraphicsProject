#include "BezierCurve.h"


BezierCurve::BezierCurve(std::vector<Vertex> controlPoints) {
	this->controlPoints = controlPoints;
	this->curveShader = new Shader("curveVertexShader.vert", "curveFragmentShader.frag");
	trackPiece = new TrackPiece("model/trackModel.glb");
	useCustomModel = true;

	if (!trackPiece->loadedCorrectly) {
		useCustomModel = false;
	}

	setupCurve();
}

BezierCurve::~BezierCurve() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	delete curveShader;
	delete trackPiece;
}

void BezierCurve::Draw() {
	curveShader->use();
	curveShader->setMat4("model", curveModel);
	if (!useCustomModel) {
		glBindVertexArray(VAO);
		glLineWidth(8.0f);
		glDrawArrays(GL_LINE_STRIP, 0, lookupTable.size());
	}
	else {
		DrawTrackPieces(0.15f);
	}
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

	curveModel = glm::rotate(curveModel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	curveModel = glm::scale(curveModel, glm::vec3(15.0f, 10.0f, 15.0f));
	curveModel = glm::translate(curveModel, glm::vec3(-2.0f, 0.0f, -2.0f));
  
	curveShader->use();
	curveShader->setMat4("model", curveModel);
}

glm::vec3 BezierCurve::calculateBezierPoint(float sample) {
	int n = controlPoints.size() - 1;
	glm::vec3 result = glm::vec3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i <= n; i++) {
		float coefficient = static_cast<float>(combination(n, i)) * std::pow(1.0f - sample, n - i) * std::pow(sample, i);
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

glm::mat4 BezierCurve::getModel() {
	return curveModel;
}

void BezierCurve::setModel(glm::mat4& newModel) {
	this->curveShader->use();
	curveShader->setMat4("model", newModel);
	this->curveModel = newModel;
}

void BezierCurve::setView(glm::mat4& newView) {
	this->curveShader->use();
	curveShader->setMat4("view", newView);
	trackPiece->setView(newView);
}

void BezierCurve::setProjection(glm::mat4& newProjection) {
	this->curveShader->use();
	curveShader->setMat4("projection", newProjection);
	trackPiece->setProjection(newProjection);
}

void BezierCurve::DrawTrackPieces(float spacing) {
	float totalLength = getTotalLength();
	float distance = 0.0f;

	while (distance < totalLength) {
		auto it = std::lower_bound(lookupTable.begin(), lookupTable.end(), distance, [](const ArcLengthEntry& a, float d) {return a.arcLength < d;});

		if (it == lookupTable.end()) {
			break;
		}

		glm::vec4 transformed = curveModel * glm::vec4(it->arcVertex.Position, 1.0f);
		glm::vec3 position = glm::vec3(transformed);
		float t = it->t;
		glm::vec3 tangent = glm::normalize(calculateBezierDerivative(t));
		glm::vec4 transformedTangent = curveModel * glm::vec4(tangent, 0.0f);
		tangent = glm::normalize(glm::vec3(transformedTangent));

		glm::vec3 up = glm::vec3(0, 1, 0);
		if (glm::length(glm::cross(tangent, up)) < 0.001f) {
			up = glm::vec3(0, 0, 1);
		}

		glm::vec3 right = glm::normalize(glm::cross(up, tangent));
		glm::vec3 normal = glm::normalize(glm::cross(tangent, right));
		glm::mat4 rotation = glm::mat4(
			glm::vec4(right, 0),
			glm::vec4(normal, 0),
			glm::vec4(tangent, 0),
			glm::vec4(0, 0, 0, 1)
		);

		glm::mat4 model = glm::translate(glm::mat4(1.0f), position) * rotation;
		model = glm::scale(model, glm::vec3(1.0f, 0.2f, 1.0f));
		model = glm::rotate(model, glm::radians(90.0f), up);

		trackPiece->setModel(model);
		trackPiece->Draw();

		distance += spacing;
	}
}

TrackPiece* BezierCurve::getTrack() {
	return trackPiece;
}