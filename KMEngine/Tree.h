#pragma once

#include <vector>
#include <string>

template <typename T>
struct SNode
{
    std::string m_NodeName{};
    T m_TData{};
    std::vector<T> m_TDataVector{};
    std::vector<SNode<T>> m_Children{};
};

template <typename T>
class CTemplatedTree
{
public:
    CTemplatedTree() = default;

    CTemplatedTree(SNode<T> RootNode) : m_RootNode(RootNode) {}

    void SetRootNode(SNode<T> RootNode)
    {
        m_RootNode = RootNode;
    }

    void Insert(SNode<T>& NewNode, SNode<T>& Parent)
    {
        Parent.m_Children.push_back(NewNode);
    }

    void PreOrderTraversal(SNode<T>& StartNode, std::vector<SNode<T>*>& NodeList)
    {
        NodeList.push_back(&StartNode);

        for (SNode<T>& Child : StartNode.m_Children)
        {
            PreOrderTraversal(Child, NodeList);
        }
    }

    SNode<T>* FindNode(SNode<T>& StartNode, T Value)
    {
        if (StartNode.m_TData == Value)
        {
            return &StartNode;
        }

        for (SNode<T>& Child : StartNode.m_Children)
        {
            SNode<T>* Found = FindNode(Child, Value);
            if (Found != nullptr)
            {
                return Found;
            }
        }

        return nullptr;
    }

    SNode<T>* FindNodeByName(SNode<T>& StartNode, std::string NodeName)
    {
        if (StartNode.m_NodeName == NodeName)
        {
            return &StartNode;
        }

        for (SNode<T>& Child : StartNode.m_Children)
        {
            SNode<T>* Found = FindNodeByName(Child, NodeName);
            if (Found != nullptr)
            {
                return Found;
            }
        }

        return nullptr;
    }

    SNode<T>* FindParent(SNode<T>& StartNode, std::string NodeName)
    {
        for (SNode<T>& Child : StartNode.m_Children)
        {
            if (Child.m_NodeName == NodeName)
            {
                return &StartNode;
            }

            SNode<T>* FoundNode = FindParent(Child, NodeName);
            if (FoundNode != nullptr)
            {
                return FoundNode;
            }
        }

        return nullptr;
    }

    SNode<T> m_RootNode{};
};