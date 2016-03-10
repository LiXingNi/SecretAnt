#include <iostream>

#include "t1.h"


////////////////////////////// ANT /////////////////////////////////
void AntColonySystem::Ant::resetAnt()
{
	_loc = _ant_system._src;	//当前位置初始化为起点
	
	//置空各个数组
	_path.resize(0);
	_food_bag.clear();
	_forbiden_table.clear();
	_forbiden_table.insert(_ant_system._dest);  //初始时禁忌表中就有终点
}


bool AntColonySystem::Ant::isForbidden(VIndex index)
{
	return _forbiden_table.find(index) != _forbiden_table.end();
}

bool AntColonySystem::Ant::isMistake()
{
	static default_random_engine e;
	static uniform_int_distribution<int> u(0, 99);
	int random_num = static_cast<int>(_ant_system._mrate * 100);
	if (u(e) < random_num)  //在犯错概率内
		return true;
	else
		return false;
}


int AntColonySystem::Ant::checkAdj()
{
	//检查周围的点
	AdjSet & adj_set = _ant_system._graph[_loc];
	for (auto iter = adj_set.begin(); iter != adj_set.end(); ++iter)
	{
		// 是食物顶点，且不是禁忌顶点
		if (_ant_system._food.find(iter->first) != _ant_system._food.end() &&
			!isForbidden(iter->first)) 
			return iter->first;
	}
	return -1;
}


int AntColonySystem::Ant::chooseWay()
{
	AdjSet& adj_set = _ant_system._graph[_loc]; //当前顶点的邻接表
	vector<VIndex> unf_vertex;	//非禁忌邻接顶点
	vector<VIndex> max_vertex;  //信息量最大的顶点集合
	Info	max_info = DBL_MIN;			//最大信息量
	for (auto iter = adj_set.begin(); iter != adj_set.end(); ++iter)
	{
		if (isForbidden(iter->first))  //当前邻接顶点是禁忌点
			continue;
		unf_vertex.push_back(iter->first);  //加入非禁忌顶点集合

		Info curr_info = _ant_system.calInfo(*this, iter->first);
		if (curr_info > max_info)
		{
			max_info = curr_info;
			max_vertex.resize(0);
			max_vertex.push_back(iter->first);
		}
		else if (curr_info == max_info)
		{
			max_vertex.push_back(iter->first);
		}
		else;
	}

	//找不到可行节点。蚂蚁死亡
	if (unf_vertex.size() == 0)  
		return -1;
	else if (isMistake())
	{
		static default_random_engine e;
		uniform_int_distribution<int> u(0, unf_vertex.size() - 1);
		return unf_vertex[u(e)];	//犯错的情况下随机选取
	}
	else if (max_vertex.size() == 1) return max_vertex[0];

	else;

	//若有多个顶点的信息量相同，则根据随机因子选取
	//若当前可选的顶点数大于随机因子，则直接选取。否则，用随机因子对顶点集合的大小取余
	int s_vec = max_vertex.size();
	if (_rand_fac < s_vec)
		return max_vertex[_rand_fac];
	else
		return max_vertex[_rand_fac % s_vec];
}

void AntColonySystem::Ant::moveNext()
{
	VIndex index;  //选中的顶点
	if ((index = checkAdj()) >= 0) //找到食物顶点
	{
		_ant_system.activeFood(*this, index); //激活当前蚂蚁路径上的食物信息增量
		//_ant_system.localUpdate(*this, index); //找到了食物顶点，在将食物加入背包前要先更新食物信息量。
		//先更新信息量后再更新背包中的食物数量。上一句话必须位于下一句话前方
		updateFoodBag(index);		//更新食物背包，若食物背包装满，则将终点移出禁忌表
	}
	else if ((index = chooseWay()) < 0) //找不到合适的非禁忌顶点
	{
		resetAnt();	//重置蚂蚁
		return;
	}
	else
		_ant_system.localUpdate(*this, index); //选中普通的顶点，则反馈蚁群系统

	_path.push_back(index);
	_forbiden_table.insert(index);
	_loc = index;	//更新节点位置
}