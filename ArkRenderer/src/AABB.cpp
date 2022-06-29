// https://github.com/iauns/cpm-glm-aabb

#include "AABB.h"


#include <glm/geometric.hpp>
#include <glm/gtx/component_wise.hpp>

AABB::AABB() { SetNull(); }

AABB::AABB(const glm::vec3& center, const float radius) {
	SetNull();
	Extend(center, radius);
}

AABB::AABB(const glm::vec3& p1, const glm::vec3& p2) {
	SetNull();
	Extend(p1);
	Extend(p2);
}

AABB::AABB(const AABB& aabb) {
	SetNull();
	Extend(aabb);
}

void AABB::Extend(const float val) {
	if (!IsNull()) {
		m_min -= glm::vec3(val);
		m_max += glm::vec3(val);
	}
}

void AABB::Extend(const glm::vec3& p) {
	if (!IsNull()) {
		m_min = glm::min(p, m_min);
		m_max = glm::max(p, m_max);
	}
	else {
		m_min = p;
		m_max = p;
	}
}

void AABB::Extend(const glm::vec3& p, float radius) {
	glm::vec3 r(radius);
	if (!IsNull()) {
		m_min = glm::min(p - r, m_min);
		m_max = glm::max(p + r, m_max);
	}
	else {
		m_min = p - r;
		m_max = p + r;
	}
}

void AABB::Extend(const AABB& aabb) {
	if (!aabb.IsNull()) {
		Extend(aabb.m_min);
		Extend(aabb.m_max);
	}
}

void AABB::ExtendDisk(const glm::vec3& c, const glm::vec3& n, float r) {
	if (glm::length(n) < 1.e-12) {
		Extend(c);
		return;
	}
	glm::vec3 norm = glm::normalize(n);
	float x = glm::sqrt(1 - norm.x) * r;
	float y = glm::sqrt(1 - norm.y) * r;
	float z = glm::sqrt(1 - norm.z) * r;
	Extend(c + glm::vec3(x, y, z));
	Extend(c - glm::vec3(x, y, z));
}

glm::vec3 AABB::GetDiagonal() const {
	if (!IsNull()) return m_max - m_min;
	return glm::vec3(0.0);
}

float AABB::GetLongestEdge() const { return glm::compMax(GetDiagonal()); }

float AABB::GetShortestEdge() const { return glm::compMin(GetDiagonal()); }

glm::vec3 AABB::GetCenter() const {
	if (!IsNull()) {
		glm::vec3 d = GetDiagonal();
		return m_min + (d * float(0.5));
	}
	return glm::vec3(0.0);
}

void AABB::Translate(const glm::vec3& v) {
	if (!IsNull()) {
		m_min += v;
		m_max += v;
	}
}

void AABB::Scale(const glm::vec3& s, const glm::vec3& o) {
	if (!IsNull()) {
		m_min -= o;
		m_max -= o;

		m_min *= s;
		m_max *= s;

		m_min += o;
		m_max += o;
	}
}

bool AABB::Overlaps(const AABB& bb) const {
	if (IsNull() || bb.IsNull()) return false;

	if (bb.m_min.x > m_max.x || bb.m_max.x < m_min.x) return false;
	if (bb.m_min.y > m_max.y || bb.m_max.y < m_min.y) return false;
	if (bb.m_min.z > m_max.z || bb.m_max.z < m_min.z) return false;

	return true;
}

AABB::INTERSECTION_TYPE AABB::Intersect(const AABB& b) const {
	if (IsNull() || b.IsNull()) return OUTSIDE;

	if ((m_max.x < b.m_min.x) || (m_min.x > b.m_max.x) ||
		(m_max.y < b.m_min.y) || (m_min.y > b.m_max.y) ||
		(m_max.z < b.m_min.z) || (m_min.z > b.m_max.z)) {
		return OUTSIDE;
	}

	if ((m_min.x <= b.m_min.x) && (m_max.x >= b.m_max.x) &&
		(m_min.y <= b.m_min.y) && (m_max.y >= b.m_max.y) &&
		(m_min.z <= b.m_min.z) && (m_max.z >= b.m_max.z)) {
		return INSIDE;
	}

	return INTERSECT;
}


bool AABB::IsSimilarTo(const AABB& b, float diff) const {
	if (IsNull() || b.IsNull()) return false;

	glm::vec3 acceptable_diff = ((GetDiagonal() + b.GetDiagonal()) / float(2.0)) * diff;
	glm::vec3 min_diff(m_min - b.m_min);
	min_diff = glm::vec3(fabs(min_diff.x), fabs(min_diff.y), fabs(min_diff.z));
	if (min_diff.x > acceptable_diff.x) return false;
	if (min_diff.y > acceptable_diff.y) return false;
	if (min_diff.z > acceptable_diff.z) return false;
	glm::vec3 max_diff(m_max - b.m_max);
	max_diff = glm::vec3(fabs(max_diff.x), fabs(max_diff.y), fabs(max_diff.z));
	if (max_diff.x > acceptable_diff.x) return false;
	if (max_diff.y > acceptable_diff.y) return false;
	if (max_diff.z > acceptable_diff.z) return false;
	return true;
}
