#pragma once

#include <vector>

#include "Model.h"

class GraphNode
{
public:
	GraphNode(Model* model = nullptr, glm::mat4 localTransform = glm::mat4(1.0f)) : model(model), localTransform(localTransform)
	{ 
		worldTransform = glm::mat4(1.0f);
		dirty_ = true;
	}

	void AddChild(GraphNode* node) { children.push_back(node); }

	// Transform manipulation functions ---------------------------------------------------------
	void SetLocalPosition(glm::vec3 localPosition)
	{
		localTransform[3][0] = localPosition.x;
		localTransform[3][1] = localPosition.y;
		localTransform[3][2] = localPosition.z;
		dirty_ = true;
	}
	
	void Move(glm::vec3 vector) 
	{ 
		localTransform = glm::translate(localTransform, vector); 
		dirty_ = true; // Very important to always set the dirty flag when changing the transform
	}

	void Rotate(glm::vec3 axis, float degrees) 
	{ 
		localTransform = glm::rotate(localTransform, glm::radians(degrees), axis); 
		dirty_ = true;
	}

	void Scale(glm::vec3 scale)
	{
		localTransform = glm::scale(localTransform, scale);
		dirty_ = true; // Very important to always set the dirty flag when changing the transform
	}

	// Deferred recalculation implementation for GraphNode class with dirty flag --------------------
	void Render(glm::mat4 &parentWorld, bool dirty = false)
	{
		dirty |= dirty_;
		// Check for the status of the dirty flag
		if (dirty)
		{
			// If the flag is set recalc the worldTransform
			worldTransform = parentWorld * localTransform;
			dirty_ = false;
		}
		// Draw the object's model with the calculated (or not) world transform (if the model exists of course)
		if (model != nullptr)
		{
			// Set the appropriate model matrix
			model->GetMaterial()->SetModel(&worldTransform);
			model->Draw();
		}

		// Recursively call render on all the child nodes
		for (GraphNode* childNode : children)
		{
			childNode->Render(worldTransform, dirty);
		}
	}

private:
	Model* model;

	glm::mat4 localTransform;
	glm::mat4 worldTransform;

	bool dirty_ = false;

	std::vector<GraphNode*> children;
};