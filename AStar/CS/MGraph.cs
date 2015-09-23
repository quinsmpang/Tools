// 阻挡点
public class StopPoint
{
	public StopPoint()
    {

    }

    public void dispose()
    {

    }
}

// 当前顶点的状态
public enum State
{
    Closed,			// 如果已经确认了顶点，就是这个状态
	Open,			// 如果已经遍历，但是还没有确认就是这个状态
	Unknown			// 最初的状态就是这个状态
};

// 顶点数据
public class Vertex
{
    public int m_id;
    public State m_state;
    public Vertex m_nearestVert;
    public float m_distance;
    public bool m_bNeighborValid;      // 邻居数据是否有效，因为可能动态修改阻挡点
    public List<int> m_vertsIdVec;          // 保存邻居顶点 Id，这个数值只有在使用的时候才会动态生成，初始化的时候并不生成
    public StopPoint m_pStopPoint;            // 阻挡点信息

    public Vertex()
    {
       reset();
    }

    public void dispose()
    {
        if (m_pStopPoint)
        {
            delete m_pStopPoint;
            m_pStopPoint = nullptr;
        }
    }

    public void reset()
    {
        //m_id = 0;
        m_state = State::Unknown;
        m_nearestVert = null;
        m_distance = float.MaxValue;
        m_bNeighborValid = false;
        m_pStopPoint = null;
        m_vertsIdVec.clear();
    }

    public void setNeighborVertsId(int[] neighborVertIdArr, int len)
    {
        m_vertsIdVec.clear();
        m_bNeighborValid = true;
        for (int idx = 0; idx < len; ++idx)
        {
            if (neighborVertIdArr[idx] != -1)
            {
                m_vertsIdVec.push_back(neighborVertIdArr[idx]);
            }
        }
    }
}

public class MGraph
{
	protected List<Vertex> m_vertsVec;      // 所有的顶点，启动的时候，所有的顶点全部创建，不是需要的时候再创建，如果需要的时候再创建，就需要各种判断
    protected int m_vertsCount;           // 定点总共数量
    protected int m_xCount;               // X 顶点数量
    protected int m_yCount;               // Y 顶点数量
    protected float m_gridWidth;          // 格子宽度
    protected float m_gridHeight;         // 格子高度

    // Dijkstra 算法需要的数据
    protected Vertex m_startVert;
    protected Vertex m_endVert;

    // 最终路径列表
    protected List<Vertex> m_pathList;  // 使用 List ，主要是使用 push_front 这个接口
                                        // 计算中需要用的 8 个邻居顶点索引
    protected int[] m_neighborVertIdArr;
    protected List<int> m_closedVec;   // 已经确认的队列列表

    // 路径缓存列表
    PathCache m_pathCache;


    public MGraph()
    {
        m_neighborVertIdArr = new int[8];
    }

    public void dispose()
    {

    }


    public Vertex getVertexById(int vertId)
    {
	    if (vertId < m_vertsVec.size())
	    {
		    return m_vertsVec[vertId];
	    }

	    return nullptr;
    }

    public List<Vertex> getVertsVec()
    {
	    return m_vertsVec;
    }

    public int getVertsCount()
    {
	    return m_vertsVec.size();
    }

    public void init(int xCount, int yCount, float gridWidth, float gridHeight)
    {
	    m_xCount = xCount;
	    m_yCount = yCount;
	    m_gridWidth = gridWidth;
	    m_gridHeight = gridHeight;

	    m_vertsCount = m_xCount * m_yCount;

	    int idx = 0;
	    Vertex* pVertex = nullptr;

	    for (idx = 0; idx < m_vertsCount; ++idx)
	    {
		    pVertex = new Vertex();
		    m_vertsVec.push_back(pVertex);
		    pVertex->reset();
		    pVertex->m_id = idx;
	    }
    }

    // 转换顶点的 Id 到顶点索引
    public void convVertIdToXY(int vertId, ref int x, ref int y)
    {
	    y = vertId / m_xCount;
	    x = vertId - y * m_xCount;
    }

    public int convXYToVertId(int x, int y)
    {
	    return (y * m_xCount + x);
    }

    // 是否在阻挡点内
    public bool isInStopPt(int nx, int ny)
    {
	    if (nx >= 0 && nx < m_xCount
		    && ny >= 0 && ny < m_yCount)
	    {
		    int index = ny * m_xCount + nx;
		    if (m_vertsVec[index]->m_pStopPoint)			// 如果有阻挡点
		    {
			    return true;
		    }
	    }

	    return false;
    }

    /**
    * @brief 邻居格子成本
    * @param vertId 起始顶点 Id
    * @Param neighborVertId 邻居顶点 Id
    *	0	1	2
    *	3		4
    *	5	6	7
    */
    public float adjacentCost(int vertId, int neighborVertId)
    {
	    int x, y;
	    int xNeighbor, yNeighbor;
	    float neighborCost = float.MaxValue;            // 默认是最大值
        const int[] dx = new int[8] { -1, 0, 1, -1, 1, -1, 0, 1 };
        const int[] dy = new int[8] { -1, -1, 1, 0, 0, 1, 1, 1 };
        const float[] cost = new float[8]{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };

	    if (vertId == neighborVertId)		// 如果是自己，就返回 0
	    {
		    return 0;
	    }

	    convVertIdToXY(neighborVertId, xNeighbor, yNeighbor);
	    if (isInStopPt(xNeighbor, yNeighbor))		// 如果邻居在阻挡点中
	    {
		    return neighborCost;
	    }

	    convVertIdToXY(vertId, x, y);
	    if (std::abs((long)(xNeighbor - x)) > 1 || std::abs((long)(yNeighbor - y)) > 1)	// 如果相差不是 1 ，就说明中间有间隔，不能直接到达
	    {
		    return neighborCost;
	    }

	    int nx = 0;
	    int ny = 0;

	    for (int i = 0; i<8; ++i)
	    {
		    nx = x + dx[i];
		    ny = y + dy[i];

		    if (convXYToVertId(nx, ny) == neighborVertId)		// 如果正好是邻居
		    {
			    // 肯定不在阻挡点中，因为如果在阻挡点中，上面已经判断了
			    if (isHorizontalOrVerticalNeighbor(vertId, neighborVertId))		// 如果是水平或者垂直，是斜线
			    {
				    neighborCost = cost[i];
			    }
			    else
			    {
				    // 需要判断斜线上的另一个斜线的两个格子是否是阻挡点
				    if (!isInStopPt(x, yNeighbor) && !isInStopPt(xNeighbor, y))		// 如果对角线上的两个格子都不是阻挡点
				    {
					    neighborCost = cost[i];
				    }
			    }

			    break;
		    }
	    }

	    return neighborCost;
    }

    public void addStopPoint(int nx, int ny, StopPoint pStopPoint)
    {
	    int vertId = convXYToVertId(nx, ny);
	    m_vertsVec[vertId].m_pStopPoint = pStopPoint;

	    setNeighborInvalidByVertId(vertId);
    }

    public bool isHorizontalOrVerticalNeighbor(int vertId, int neighborVertId)
    {
	    int x, y;
	    int xNeighbor, yNeighbor;
	    convVertIdToXY(vertId, x, y);
	    convVertIdToXY(neighborVertId, xNeighbor, yNeighbor);

	    if ((std::abs((long)(xNeighbor - x)) == 1 && std::abs((long)(yNeighbor - y)) == 0) ||
		    (std::abs((long)(xNeighbor - x)) == 0 && std::abs((long)(yNeighbor - y)) == 1))
	    {
		    return true;
	    }

	    return false;
    }

    public bool isHorizontalNeighbor(int vertId, int neighborVertId)
    {
	    int x, y;
	    int xNeighbor, yNeighbor;
	    convVertIdToXY(vertId, x, y);
	    convVertIdToXY(neighborVertId, xNeighbor, yNeighbor);

	    if ((std::abs((long)(xNeighbor - x)) == 1 && std::abs((long)(yNeighbor - y)) == 0))
	    {
		    return true;
	    }

	    return false;
    }

    public bool isVerticalNeighbor(int vertId, int neighborVertId)
    {
	    int x, y;
	    int xNeighbor, yNeighbor;
	    convVertIdToXY(vertId, x, y);
	    convVertIdToXY(neighborVertId, xNeighbor, yNeighbor);

	    if ((std::abs((long)(xNeighbor - x)) == 0 && std::abs((long)(yNeighbor - y)) == 1))
	    {
		    return true;
	    }

	    return false;
    }

    public bool isSlashNeighbor(int vertId, int neighborVertId)
    {
	    int x, y;
	    int xNeighbor, yNeighbor;
	    convVertIdToXY(vertId, x, y);
	    convVertIdToXY(neighborVertId, xNeighbor, yNeighbor);

	    if ((std::abs((long)(xNeighbor - x)) == 1 && std::abs((long)(yNeighbor - y)) == 1))			// 斜线
	    {
		    return true;
	    }

	    return false;
    }

    public bool isNeighbor(int vertId, int neighborVertId)
    {
	    int x, y;
	    int xNeighbor, yNeighbor;
	    convVertIdToXY(vertId, x, y);
	    convVertIdToXY(neighborVertId, xNeighbor, yNeighbor);

	    if ((std::abs((long)(xNeighbor - x)) == 1 && std::abs((long)(yNeighbor - y)) == 0) ||		// 水平
		    (std::abs((long)(xNeighbor - x)) == 0 && std::abs((long)(yNeighbor - y)) == 1) ||		// 垂直
		    (std::abs((long)(xNeighbor - x)) == 1 && std::abs((long)(yNeighbor - y)) == 1))			// 斜线
	    {
		    return true;
	    }

	    return false;
    }

    public bool isBackSlashStopPoint(int vertId, int neighborVertId)
    {
	    int x, y;
	    int xNeighbor, yNeighbor;
	    convVertIdToXY(vertId, x, y);
	    convVertIdToXY(neighborVertId, xNeighbor, yNeighbor);

	    if (isInStopPt(x, yNeighbor) || isInStopPt(xNeighbor, y))
	    {
		    return true;
	    }

	    return false;
    }

    public void findNeighborVertIdArr(int vertId)
    {
	    int x, y;
	    convVertIdToXY(vertId, x, y);

	    const int[] dx = new int[8]{ -1, 0, 1, -1, 1, -1, 0, 1 };
	    const int[] dy = new int[8]{ -1, -1, 1, 0, 0, 1, 1, 1 };

	    int nx = 0;
	    int ny = 0;

	    // 遍历 8 个邻居顶点
	    for (int i = 0; i < 8; ++i)
	    {
		    nx = x + dx[i];
		    ny = y + dy[i];

		    if (nx >= 0 && nx < m_xCount &&
			    ny >= 0 && ny < m_yCount)		// 如果邻居顶点在范围内
		    {
			    if (!isInStopPt(nx, ny))		// 如果不在阻挡点内
			    {
				    m_neighborVertIdArr[i] = convXYToVertId(nx, ny);
			    }
			    else
			    {
				    m_neighborVertIdArr[i] = -1;
			    }
		    }
		    else
		    {
			    m_neighborVertIdArr[i] = -1;
		    }
	    }
    }

    public void setNeighborInvalidByVertId(int vertId)
    {
	    // 需要修改邻居是这个顶点的其它顶点的邻居
	    if (!m_vertsVec[vertId]->m_bNeighborValid)
	    {
		    findNeighborVertIdArr(vertId);
		    m_vertsVec[vertId]->setNeighborVertsId(m_neighborVertIdArr);
	    }

	    for (int neighborIdx = 0; neighborIdx < m_vertsVec[vertId]->m_vertsIdVec.size(); ++neighborIdx)
	    {
		    m_vertsVec[m_vertsVec[vertId]->m_vertsIdVec[neighborIdx]]->m_bNeighborValid = false;
	    }
    }

    public Vertex* getVertexByPos(float fx, float fy)
    {
	    int ix = fx / m_gridWidth;
	    int iy = fx / m_gridHeight;

	    return m_vertsVec[convXYToVertId(ix, iy)];
    }

    public void getVertexCenterByPos(float fx, float fy, ref float centerX, ref float centerY)
    {
	    int ix = fx / m_gridWidth;
	    int iy = fx / m_gridHeight;

	    m_vertsVec[convXYToVertId(ix, iy)];

	    centerX = ix * m_gridWidth + m_gridWidth / 2;
	    centerY = ix * m_gridHeight + m_gridHeight / 2;
    }
}