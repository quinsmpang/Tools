#include "MGraph.h"

#include <numeric>
#include <stdexcept>
#include <list>

std::list<Vertex*> MGraph::buildPath(Vertex *endVert)
{
	std::list<Vertex*> path;

	Vertex *vert = endVert;
	while (vert != nullptr) 
	{
		path.push_front(vert);
		vert = vert->m_nearestVert;
	}

	return path;
}

void MGraph::initializeNodes(unsigned int startId, unsigned int endId)
{
	m_startVert = m_endVert = nullptr;

	for (auto pVert : m_verts)
	{
		pVert->m_distance = std::numeric_limits<unsigned int>::max();
		pVert->m_nearestVert = nullptr;
		pVert->m_state = State::Unknown;

		if (pVert->m_id == startId) 
		{
			m_startVert = pVert;
		}

		if (pVert->m_id == endId) 
		{
			m_endVert = pVert;
		}
	}

	if (m_startVert == nullptr || m_endVert == nullptr) 
	{
		throw std::runtime_error("Failed to find the start/end node(s)!");
	}

	m_startVert->m_distance = 0;
}

std::list<Vertex*> MGraph::getShortestPath(unsigned int startId, unsigned int endId)
{
	initializeNodes(startId, endId);

	int v0;
	int v, w, k, min;
	int nVer = m_vertsCount;
	Vertex* pVert = nullptr;

	// 初始化数据
	for (v = 0; v < nVer; ++v)
	{
		m_startVert->m_state = State::Unknown;    // 全部顶点初始化为未知对短路径状态
		pVert = m_verts[v];
		pVert->m_distance = adjacentCost(startId, v); //将与V0点有连线的顶点加上权值
		pVert->m_nearestVert = nullptr;    // 初始化路径数组p为0
	}

	m_startVert->m_distance = 0;    // V0至V0路径为0
	m_startVert->m_state = State::Closed;    // final[W]=1表示V0至V0不需要求路径

	// 开始主循环，每次求得V0到某个V顶点的最短路径
	for (v = 1; v < nVer; ++v)
	{
		min = INFINITY;    // 当前所知离V0顶点最近距离
		for (w = 0; w < nVer; ++w) // 寻找离V0最近的顶点
		{
			pVert = m_verts[w];
			if (pVert->m_state != State::Closed && pVert->m_distance < min)
			{
				min = pVert->m_distance; // w顶点离V0顶点更近
				k = w;
			}
		}

		pVert = m_verts[k];
		pVert->m_state = State::Closed; // 将目前找到的最近的顶点置为1

		for (w = 0; w < nVer; ++w) // 修正当前最短路径距离
		{
			pVert = m_verts[w];
			// 如果经过V顶点的路径比现在这条路径的长度短的话
			if (pVert->m_state != State::Closed && (min + adjacentCost(k, w) < pVert->m_distance))
			{
				// 说明找到了最短的路径，修改D[w] 和 p[w]
				pVert->m_distance = min + adjacentCost(k, w); // 修改当前路径长度
				pVert->m_nearestVert = m_verts[k];
			}
		}

		if (k == endId)			// 如果查找到
		{
			break;
		}
	}

	return buildPath(m_endVert);
}