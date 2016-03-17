#include <iostream>
#include <ctime>
#include "ant.h"

const double AntColonySystem::_init_info = 10000;	//初始信息量
default_random_engine AntColonySystem::Ant::e(std::time(0));
uniform_int_distribution<int> AntColonySystem::Ant::u(0, 7); 
uniform_real_distribution<double> AntColonySystem::Ant::u_mistake; 
AntColonySystem::Info AntColonySystem::EInfo::Qmax, AntColonySystem::EInfo::Qmin;
double AntColonySystem::EInfo::ratio;

double getPbest()
{
	static double pBest = 0.005;
	return pBest;
}

////////////////////////////// ANT /////////////////////////////////
void AntColonySystem::Ant::resetAnt()
{
	//初始化禁忌表
	_forbiden_table.clear();
	_forbiden_table.insert(_ant_system._dest);  //初始时禁忌表中就有终点

	//初始化食物包裹
	_food_bag.clear();

	//初始化路径集合，加入起点
	_path.resize(0);
	_path.push_back(_ant_system._src);


	//初始化一些成员
	_loc = _ant_system._src;	//当前位置初始化为起点
	_rand_fac = AntRandNum();
	_path_weight = 0;
	_survive = false;			//初始化成功标志位为：否
}


bool AntColonySystem::Ant::isForbidden(VIndex index)
{
	return _forbiden_table.find(index) != _forbiden_table.end();
}

bool AntColonySystem::Ant::isMistake()
{
	if (u_mistake(e) < _ant_system._mrate)  //在犯错概率内
		return true;
	else
	{
		return false;
	}
}

//检查周围的顶点是否是非禁忌表中的食物顶点或终点
//若是食物顶点，则返回该食物顶点 >= 0;
//若是终点，则返回 -2
//若无特殊点，则返回 -1
int AntColonySystem::Ant::checkAdj()  
{
	//检查周围的点
	AdjSet & adj_set = _ant_system._graph[_loc];
	for (auto iter = adj_set.begin(); iter != adj_set.end(); ++iter)
	{
		if (!isForbidden(iter->first)) //考虑非禁忌顶点
		{
			if (_ant_system.isFood(iter->first))  //若是食物顶点，则直接返回该顶点
				return iter->first;
			else if (iter->first == _ant_system._dest) //到达终点，则返回-2
				return -2;
		}
	}
	return -1; //普通顶点，返回 -1
}


int AntColonySystem::Ant::chooseWay()
{
	AdjSet& adj_set = _ant_system._graph[_loc]; //当前顶点的邻接表
	//vector<VIndex> unf_vertex;	//非禁忌邻接顶点
	VIndex min_vertex;
	Info	min_info = DBL_MAX;
	vector<pair<VIndex,Info>> unf_info;  //信息量最大的顶点集合
	Info total_info = 0;

	for (auto iter = adj_set.begin(); iter != adj_set.end(); ++iter)
	{
		if (isForbidden(iter->first))  //当前邻接顶点是禁忌点
			continue;
		Info curr_info = _ant_system.calInfo(*this, iter->first);
		Weight curr_weight = _ant_system._graph[_loc][iter->first]._adj_e_weight;
		curr_info /= (curr_weight * curr_weight);

		total_info += curr_info;  //统计所有信息量
		unf_info.push_back({ iter->first,curr_info });

		if (curr_info < min_info)
		{
			min_info = curr_info;
			min_vertex = iter->first;
		}
	}

	//找不到可行节点。蚂蚁死亡
	if (unf_info.size() == 0)
	{
		//_ant_system.activeClear(*this);
		return -1;
	}
	else if (isMistake())
	{
		return min_vertex;
	}
	else;

	//******** 这一段等田野改 **********************************

	//根据信息量分配选中概率
	for (auto & info : unf_info)
	{
		if (total_info == 0)
			info.second = 1.0 / unf_info.size();	//如果每条路信息素均为0，则每条路均摊概率
		info.second /= total_info;					//计算概率
	}

	double rand_num = u_mistake(e);					//生成一个double型随机数
	VIndex i = 0;
	while (rand_num > 0)							//轮流从rand_num中减去每一个概率值，如果减为负数或0，则选择这条路。
	{
		rand_num -= unf_info[i].second;
		++i;					
	}
	return unf_info[i - 1].first;					//因为在循环体内最后调用了++i,故这里返回i-1的索引值。
	
	//******** 这一段等田野改 **********************************
}

void AntColonySystem::Ant::updateFoodBag(VIndex index)
{
	_food_bag.insert(index);
	if (_food_bag.size() == _ant_system._food.size())  //检查包裹是否装满
	{
		auto iter = _forbiden_table.find(_ant_system._dest);
		_forbiden_table.erase(iter);
	}
}

//void AntColonySystem::Ant::terminate()
//{
//	_ant_system.updataPath(_path);
//	//蚂蚁到家后清空路径信息增益，避免重复走一条路径
//	//_ant_system.activeClear(*this);
//}


void AntColonySystem::Ant::dealCommonVIndex(VIndex index)
{
	//加入路径
	_path.push_back(index);
	//加入禁忌表
	_forbiden_table.insert(index);
	//更新当前位置
	_loc = index;
}

void AntColonySystem::Ant::dealFoodVIndex(VIndex index)
{
	//与处理普通顶点一样
	dealCommonVIndex(index);
	//加入食物包裹
	updateFoodBag(index);
}


int AntColonySystem::Ant::moveNext()  //正常移动，返回顶点值，返回-2 即为到达终点，返回 -1即为死亡
{
	VIndex index;  //选中的顶点

	if ((index = checkAdj()) >= 0 && !isMistake()) //若犯错往下走；否则，如果找到食物顶点
	{
		dealFoodVIndex(index);
	}
	else if (index == -2 && !isMistake()) //邻接点中有终点且食物收集结束
	{
		dealCommonVIndex(_ant_system._dest);
		return -2;  //返回-2 则蚂蚁存活
	}
	else if ((index = chooseWay()) < 0) //找不到合适的非禁忌顶点
	{
		return -1;
	}
	else dealCommonVIndex(index);  //选中了普通顶点

	return index;
}

void AntColonySystem::Ant::antRun()
{
	resetAnt();		//蚂蚁重置

	int move_res;
	while ((move_res = moveNext()) >= 0)
	{
		//累加路径权重
		_path_weight += _ant_system._graph[_path[_path.size() - 2]][move_res]._adj_e_weight;
	}

	//将权重调整为每收集一个食物需要移动的距离
	_path_weight = double(_path_weight) / double(_food_bag.size() + 1); 

	if (move_res == -1) //蚂蚁死亡
		_survive = false;
	else
		_survive = true;
}


bool AntColonySystem::isFood(VIndex index)
{
	return _food.find(index) != _food.end();
}

void AntColonySystem::updateBestAnt(Ant& ant)
{	
	if (cmpAnts(ant, *_best_ant)) //新蚂蚁更优秀
	{
		_best_ant->_path = ant._path;
		_best_ant->_path_weight = ant._path_weight;
		_best_ant->_survive = ant._survive;
	}
}

bool AntColonySystem::cmpAnts(const Ant& ant1, const Ant& ant2)
{

	if ((ant1._survive && ant2._survive) || ((!ant1._survive) && (!ant2._survive)))
		//两只蚂蚁为同一种蚂蚁，则比较 power
	{
		return ant1._path_weight <= ant2._path_weight;
	}
	//两只蚂蚁种类不同时，终点蚂蚁更优秀
	else if (ant1._survive)
		return true;
	else
		return false;
}

//void AntColonySystem::localUpdate(Ant & ant, AdjIndex index)  //根据蚂蚁所选择的新顶点，路径信息
//{
//	AdjEdg & adj_edg = _graph[ant._loc][index]; 
//
//	// 如果背包满了,更新home权重
//	if (ant._food_bag.size() == _food.size())
//		adj_edg._adj_e_home.addInfo(); 
//	else{
//		//根据蚂蚁是否拥有某种食物，更新食物信息量
//		unordered_map<FIndex, EInfo> & adj_ftable = adj_edg._adj_e_ftable;
//		for (unordered_map<FIndex, EInfo>::iterator it = adj_ftable.begin(); it != adj_ftable.end(); ++it)
//		{
//			if (ant._food_bag.find(it->first) == ant._food_bag.end())  //该蚂蚁没有收集到该食物
//			{
//				it->second.addInfo();  //增加该食物的信息量
//			}
//		}
//	}
//}

