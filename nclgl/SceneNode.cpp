#include "SceneNode.h"

SceneNode::SceneNode(Mesh* mesh, Vector4 colour)
{
	this->mesh		= mesh;
	this->colour	= colour;
	parent			= NULL;
	modelScale		= Vector3(1, 1, 1);

	boundingRadius		= 10.0f;
	distanceFromCamera	= 0.0f;
}

SceneNode::~SceneNode(void)
{
	for (unsigned int i = 0; i < children.size(); ++i) {
		delete children[i];
	}
}

void SceneNode::AddChild(SceneNode* s) {
	children.push_back(s);
	s->parent = this;
}

void SceneNode::RemoveChild(SceneNode* s) {
	//children.erase(std::remove(children.begin(),
	//	children.end(), s), children.end());
}

/*
 * Advanced rendering can be performed here per SceneNode
 * Such as disabling depth testing or reuploading data
 */
void SceneNode::Draw(/*const OGLRenderer &r*/) {
	if (mesh) {
		mesh->Draw();
	}
}

void SceneNode::Update(float msec) {
	if (parent) {
		worldTransform = parent->worldTransform * transform;
	}
	else {
		worldTransform = transform;
	}
	for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
		(*i)->Update(msec);
	}
}
