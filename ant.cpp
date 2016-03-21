#include <ctime>
#include "antColonySystem.h"

default_random_engine Ant::e(std::time(0));
uniform_int_distribution<int> Ant::u(0, 7); 
uniform_real_distribution<double> Ant::u_mistake; 
Info EInfo::Qmax, EInfo::Qmin;
double EInfo::ratio;

double getPbest()
{
	static double pBest = 0.005;
	return pBest;
}

////////////////////////////// ANT /////////////////////////////////
void Ant::resetAnt()
{
	//初始化禁忌表
	_forbiden_table.clear();
	_forbiden_table.insert(getDst());  //初始时禁忌表中就有终点

	//初始化食物包裹
	_food_bag = 0;
	//初始化已经收集到的食物数量
	_food_num = 0;

	//初始化路径集合，加入起点
	_path.resize(0);
	_path.push_back(getSrc());


	//初始化一些成员
	_loc = getSrc();	//当前位置初始化为起点
	_rand_fac = AntRandNum();
	_path_weight = 0;
	_survive = false;			//初始化成功标志位为：否
	_mistake = false;			//初始化从来没有犯过错，允许犯一次错误
}


bool Ant::isForbidden(VIndex index)
{
	return _forbiden_table.find(index) != _forbiden_table.end();
}

bool Ant::isMistake()
{
	if (_mistake == true)	//曾经犯过错
		return false;		//不允许犯错
	if (u_mistake(e) < _ant_system._mrate)  //在犯错概率内
	{
		_mistake = true; //记录已经犯过错
		return true;
	}
	else
		return false;
}

//如果有终点，可能会直接去到终点，但是食物顶点不直接过去
//若是终点，则返回 1
//若无特殊点，则返回 -1
int Ant::checkAdj()  
{
	//检查周围的点
	auto & adj_graph = getAdjGraph();
	auto & dest_index = getDst();
	vector<VIndex> & adj_index = adj_graph[_loc]; //取出当前顶点的邻接集合
	for (auto & dst : adj_index) //邻接顶点
	{
		if (dst == dest_index && !isForbidden(dst)) //当前邻接点是终点，且此时终点不在禁忌表中
			return 1; //终点
	}
	return -1; //普通顶点
}



int Ant::chooseWay()
{
	auto & adj_graph = getAdjGraph();
	auto & graph = getGraph();

	vector<VIndex> & adj_index = adj_graph[_loc]; //当前顶点的邻接集合
	VIndex min_vertex;
	Info	min_info = DBL_MAX;
	vector<pair<VIndex,Info>> unf_info;  //信息量最大的顶点集合
	Info total_info = 0;

	for (auto dst : adj_index) //邻接点
	{
		if (isForbidden(dst))  //当前邻接顶点是禁忌点
			continue;
		Info curr_info = _ant_system.calInfo(*this, dst);
		Weight curr_weight = graph[_loc][dst]._adj_e_weight;

		_ant_system.isFood(dst) ? curr_weight = calcRealWeight(curr_weight, 1) : 1;
		
		curr_info /= (curr_weight * curr_weight);

		total_info += curr_info;  //统计所有信息量
		unf_info.push_back({ dst,curr_info });

		if (curr_info < min_info)
		{
			min_info = curr_info;
			min_vertex = dst;
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
	
}

void Ant::updateFoodBag(VIndex index)
{
	++_food_num;
	setN(_food_bag, index); //设置食物背包
	if (_food_num == getFoodNum())  //检查包裹是否装满
	{
		auto iter = _forbiden_table.find(getDst());
		_forbiden_table.erase(iter);
	}
}


void Ant::dealCommonVIndex(VIndex index)
{
	//加入路径
	_path.push_back(index);
	//加入禁忌表
	_forbiden_table.insert(index);
	//更新当前位置
	_loc = index;
}

void Ant::dealFoodVIndex(VIndex index)
{
	//与处理普通顶点一样
	dealCommonVIndex(index);
	//加入食物包裹
	updateFoodBag(index);
}


int Ant::moveNext()  //正常移动，返回顶点值，返回-2 即为到达终点，返回 -1即为死亡
{
	VIndex index;  //选中的顶点

	if ((index = checkAdj()) > 0 && !isMistake()) //若找到终点，则按照一定的概率犯错，若犯错，则往下，若不犯错，则处理终点
	{
		dealCommonVIndex(getDst()); //插入的点是终点
		return -2;	//蚂蚁存活
	}
	else if ((index = chooseWay()) < 0) //在可行点中按照信息量查找，返回 index >= 0 则找到可行点，返回 < 0，则周围无可行点
	{
		return -1;	 //蚂蚁死亡
	}
	else if (index == getDst()) // 若犯错后再次选择还是终点，则继续终点的处理
	{
		dealCommonVIndex(index);
		return -2;
	}
	else
	{
		//判断新选中的点是食物顶点还是普通顶点，分别调用不同的处理函数
		_ant_system.isFood(index) ? dealFoodVIndex(index) : dealCommonVIndex(index);

		return index;  //蚂蚁继续运动
	}
}



Weight Ant::calcRealWeight(Weight weight, int food_num)
{
	food_num += 1;
	int times = getGraph().size() > 100 ? 2 : 1;   // new add 
	double res = 1;
	while (times--)
		res *= food_num;
	return weight / double(res);
}

void Ant::antRun()
{
	resetAnt();		//蚂蚁重置

	int move_res;

	auto & graph = getGraph();

	while ((move_res = moveNext()) >= 0)
	{
		//累加路径权重
		_path_weight += graph[_path[_path.size() - 2]][move_res]._adj_e_weight;
	}

	if (move_res == -1) //蚂蚁死亡
		_survive = false;
	else
	{
		_path_weight += graph[_path[_path.size() - 2]][getDst()]._adj_e_weight;
		_survive = true;
	}

	//将权重调整为每收集一个食物需要移动的距离
	_path_weight = calcRealWeight(_path_weight, _food_num); //计算每只蚂蚁存储的权重。即为 路径 / 食物数量

}
