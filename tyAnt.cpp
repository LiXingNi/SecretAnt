#include "t1.h"
#include <fstream>
#include <sstream>
#include <stack>
#include <algorithm>

void AntColonySystem::loadInfo
(const string& graph_file, const string& key_file){
	std::fstream graph_holder, key_holder;
	graph_holder.open(graph_file), key_holder.open(key_file);

	VIndex	src_id, dst_id, key_id;		
	EIndex	edge_id;
	int		cost;
	char	rubbish;				//用来读取并抛弃逗号和|等分隔符
	//edge_id:边索引，src_id:出发顶点索引，dst_id:到达顶点索引，cost:权重，key_id:关键节点

	while (graph_holder	>> edge_id >> rubbish
						>> src_id >> rubbish
						>> dst_id >> rubbish
						>> cost)
	{
		if (_graph[src_id].find(dst_id) != _graph[src_id].end()) //对于已存在的重复路径，仅保留权重最小的那条
		{
			Weight& _e_weight = _graph[src_id][dst_id]._adj_e_weight;
			_e_weight = std::min(_e_weight, cost);
		}											
		else
			_graph[src_id][dst_id] = AdjEdg(edge_id, cost);		//若路径不存在则直接添加到图中。
	}

	key_holder >> _src >> rubbish
			   >> _dest>> rubbish;				//解析起点和终点

	while (key_holder >> key_id)
	{
		_food.insert(key_id);
		if (!(key_holder >> rubbish))			//关键节点后没有'|'符号了则中断循环
			break;
	}

	return;
}

double AntColonySystem::calInfo(Ant& ant, AdjIndex adj_index)
{
	const AdjEdg& adj_edge = _graph[ant._loc][adj_index];					//取出这条边的引用
	if (_food.size() == ant._food_bag.size())								//如果食物包裹的数量等于食物数量，则返回home信息素
		return adj_edge._adj_e_home._info;

	Info food_info(0);														//初始化食物信息素

	for (auto& food : adj_edge._adj_e_ftable)
	{
		if (ant._food_bag.find(food.first) == ant._food_bag.end())	//如果背包中不含有当前边的食物信息素x，则累加该信息素
			food_info += food.second._info;
	}

	return food_info;														//返回食物信息素  
}

void AntColonySystem::activeFood(Ant& ant, FIndex food_index)
{
	vector<VIndex>& ant_path = ant._path;										//取出当前蚂蚁经过的路径
	Weight sum_weight = 0;														//叠加的权重和
	VIndex src, dst(food_index);												//起始和目的节点，目的节点初始化为遇到的食物节点

	for (auto riter = ant_path.rbegin(); riter != ant_path.rend(); ++riter)
	{
		src = *riter;															//反向遍历路径
		AdjEdg& adj_edg = _graph[src][dst];										//取出起始到目的节点的路径信息
		EInfo& edg_info = adj_edg._adj_e_ftable[food_index];					//取出路径信息中食物信息素的变量（若没有会自动新建并默认初始化)
		sum_weight += adj_edg._adj_e_weight;									//叠加权重和
		edg_info._ainfo = std::max(edg_info._ainfo, 1.0 / static_cast<double>(sum_weight));	//将对应食物的增益设为当前增益和较大增益中的大的那个
		edg_info.addInfo();										//自加一次对应食物的信息素
		dst = src;																//将目的节点设为当前节点并进入下一轮循环
	}
}

void AntColonySystem::run()
{
	for (int i(0); i != _steps_num; ++i)
	{
		std::cout << i << std::endl;
		for (auto& ant : _ants){
			ant.moveNext();
		}
		infoDiss();					
	}
}

void AntColonySystem::infoDiss(){
	for (auto& src : _graph)										//取出图中的一个起始节点和他的相关数据
	{
		for (auto& edge : src.second)								//取出该起始节点的一条出边
		{
			EInfo& home_info = edge.second._adj_e_home;				
			home_info._info *= _hrate;								//家信息素消散
			for (auto& food_info : edge.second._adj_e_ftable)
				food_info.second._info *= _frate;					//遍历该边所有的食物信息素并消散
		}
	}
}