#pragma once

#include <queue>

template <typename T>
class CSceneGraphNode
{
public:
	std::vector<CSceneGraphNode*> ChildNode;
	T m_TType;
};

template <typename T>
class CSceneGraph
{
public:
	CSceneGraph()
		: m_pRootNode{ nullptr }
	{}

	CSceneGraph(CSceneGraphNode<T>* InRoot)
		: m_pRootNode{ InRoot }
	{}

	CSceneGraph(const CSceneGraph& SceneGraphCopy)
	{
		if (SceneGraphCopy.m_pRootNode)
			m_pRootNode = new CSceneGraphNode<T>(*SceneGraphCopy.m_pRootNode);
		else
			m_pRootNode = nullptr;
	}

	void SetRoot(CSceneGraphNode<T>* InRoot)
	{
		m_pRootNode = InRoot;
	}

	void AddChild(CSceneGraphNode<T>* Node)
	{
		CSceneGraphNode<T>* NewNode = new CSceneGraphNode<T>;
		Node->ChildNode.push_back(NewNode);
	}

	void Loop(CSceneGraphNode<T>* StartNode, std::vector<CSceneGraphNode<T>*> TempNodeVector)
	{
		for (auto NodeIt : StartNode->ChildNode)
		{
			TempNodeVector.push_back(NodeIt);
			Loop(NodeIt, TempNodeVector);
		}
	}

	void Traverse(CSceneGraphNode<T>* StartNode, std::vector<CSceneGraphNode<T>*>& NodeVectorTemp)
	{
		if (StartNode->ChildNode.size())
		{
			for (auto NodeIt : StartNode->ChildNode)
			{
				if (NodeIt)
				{
					NodeVectorTemp.push_back(NodeIt);
				}
				Traverse(NodeIt, NodeVectorTemp);
			}
		}
	}

	void BFS(CSceneGraphNode<T> Root, std::vector<CSceneGraphNode<T>*>& NodeVector)
	{
		if (Root == nullptr)
		{
			return;
		}

		std::queue<CSceneGraphNode<T>*> Queue;
		Queue.push(Root);

		while (!Queue.empty())
		{
			CSceneGraphNode<T>* CurrentNode = Queue.front();
			Queue.pop();

			NodeVector.push_back(CurrentNode);

			for (auto NodeIt : CurrentNode->ChildNode)
			{
				Queue.push(NodeIt);
			}
		}
	}

	CSceneGraphNode<T>* m_pRootNode;
};