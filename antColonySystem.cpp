#include "antColonySystem.h"

bool AntColonySystem::isFood(VIndex index)
{
	return index < getFoodNum();
}

void AntColonySystem::updateBestAnt(Ant& ant)
{
	if (cmpAnts(ant, *_best_ant)) //新蚂蚁更优秀
	{
		_best_ant->_path = ant._path;
		_best_ant->_path_weight = ant._path_weight;
		_best_ant->_survive = ant._survive;
		_best_ant->_food_num = ant._food_num;
		_best_ant->_food_bag = ant._food_bag;
	}
}

bool AntColonySystem::cmpAnts(const Ant& ant1, const Ant& ant2)
{

	if ((ant1._survive && ant2._survive) || ((!ant1._survive) && (!ant2._survive)))
		//两只蚂蚁为同一种蚂蚁，则比较 power
	{
		return ant1._path_weight < ant2._path_weight;
	}
	//两只蚂蚁种类不同时，终点蚂蚁更优秀
	else if (ant1._survive)
		return true;
	else
		return false;
}


////******************待修改
//double AntColonySystem::calInfo(Ant& ant, AdjIndex adj_index)
//{
//	auto & _graph = getGraph();
//	AdjEdg& adj_edge = _graph[ant._loc][adj_index];					//取出这条边的引用
//	if (getFoodNum() == ant._food_num)								//如果食物包裹的数量等于食物数量，则返回home信息素
//		return adj_edge._adj_e_home._info;
//
//	Info food_info(0);														//初始化食物信息素
//	vector<pair<VIndex,EInfo>> & food_table = adj_edge._adj_e_ftable;
//	for (auto food_iter = food_table.begin(); food_iter != food_table.end(); ++ food_iter)
//	{
//		if (ant._food_bag[_food[food_iter->first]] == 0)	//如果背包中不含有当前边的食物信息素x，则累加该信息素
//			food_info += food_iter->second._info;
//	}
//
//
//	food_info += (_food.size() - adj_edge._adj_e_ftable.size()) * _record_info;
//
//	return food_info;														//返回食物信息素  
//}

double AntColonySystem::calInfo(Ant & ant, AdjIndex adj_index)
{
	auto & graph = getGraph();
	AdjEdg & adj_edge = graph[ant._loc][adj_index];
	Number food_num = getFoodNum();
	if (ant._food_num == food_num)
		return adj_edge._adj_e_home._info;

	Info food_info(0);
	vector<EInfo> & food_table = adj_edge._adj_e_ftable;
	register uint64_t food_bag = ant._food_bag;
	int loc = 0;
	while (food_num--)
	{
		//取出最低位
		if ((food_bag & 1) == 0)  //food_bag当前位为0 。则迭加当前食物的信息量
			food_info += food_table[loc]._info;
		++loc;
		food_bag >>= 1;   //取出下一位
	}
	return food_info;
}


//每四次循环选择局部最佳，一次选择全局最佳
//局部最佳 ： true
//全局最佳 ： false
void AntColonySystem::run()
{
	int count = 0;
	for (int i(0); i != _steps_num; ++i)
	{
		if (i % 100 == 0)
			std::cout << i << std::endl;
		for (auto & ant : _ants)
			ant.antRun();
		bool flag = count++ % 5 == 0 ? false : true;

		Ant & best_ant = chooseBestAnt(flag);
		updateInfo(best_ant);

		//adjustMisRate(0.0006);
	}
}



//* method:
//* false->选取全局最优蚂蚁
//* true->选取迭代最优蚂蚁 
Ant& AntColonySystem::chooseBestAnt(bool method) {
	auto best_ant_iter = _ants.begin();
	for (auto ant_iter = _ants.begin() + 1; ant_iter != _ants.end(); ++ant_iter)
	{
		if (cmpAnts(*ant_iter, *best_ant_iter))
		{
			best_ant_iter = ant_iter;
		}
	}
	updateBestAnt(*best_ant_iter);
	switch (method){
	case false: return *_best_ant;
	case true: return *best_ant_iter;
	default: throw std::runtime_error("error method in chooseBestAnt function");
	}
}

void AntColonySystem::updateInfo(Ant& ant){
	auto & _graph = getGraph();
	auto & _adj_graph = getAdjGraph();

	EInfo::updateMinMax(ant, _frate);///question

	//正常消散部分
	for (int src = 0; src != _adj_graph.size(); ++src)
	{
		vector<VIndex> & adj_index = _adj_graph[src];
		for (auto & dst : adj_index)
		{
			//更新邻接顶点 src->dst 的家权重
			_graph[src][dst]._adj_e_home.diss(_hrate);

			//更新邻接顶点 src->dst 的食物顶点
			auto & adj_food_table = _graph[src][dst]._adj_e_ftable;
			for (auto & food_info: adj_food_table)
			{
				food_info.diss(_frate);
			}
		}
	}

	//根据最佳蚂蚁消散部分
	vector<int> food_vec;	//需要更新的食物表

	bool update_home(false);
	ant._survive ? update_home = true : 1;  //若蚂蚁是存活的，则更新Home 信息素

	auto path = ant._path;
	double add_value = 1.0 / ant._path_weight;

	for (int i = path.size() - 1; i > 0; --i)
	{
		if (update_home)
			_graph[path[i - 1]][path[i]]._adj_e_home.addInfo(add_value);

		if (isFood(path[i]))  //若当前顶点是食物顶点，则将其加入需要更新的食物表
			food_vec.push_back(path[i]);

		auto & ftable = _graph[path[i - 1]][path[i]]._adj_e_ftable;

		for (auto food_index : food_vec)  //遍历需要更新的食物表并更新信息素
		{
			ftable[food_index].addInfo(add_value);
		}
	}

}

void EInfo::updateMinMax(Ant& best_ant, double rate){
	Qmax = 1.0 / (best_ant._path_weight *(1 - rate));
	Qmin = Qmax * ratio;
}
