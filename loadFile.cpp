#include "antColonySystem.h"
#include <fstream>
#include <sstream>

void loadFile
(const string& graph_file, const string& key_file){
	std::fstream graph_holder, key_holder;

	graph_holder.open(graph_file), key_holder.open(key_file);

	//edge_id:边索引，src_id:出发顶点索引，dst_id:到达顶点索引，cost:权重，key_id:关键节点
	VIndex	src_id, dst_id, key_id;		
	EIndex	edge_id;
	Weight  cost;				

	VIndex  src, dst;				//读取起点终点
	char	rubbish;				//用来读取并抛弃逗号和|等分隔符
	int		mapIndex(0);			//递增记录当前映射的值
	Number	food_number(0);			//记录食物数

	key_holder >> src >> rubbish
			   >> dst >> rubbish;				//解析起点和终点

	auto& map_table = getV2rTable();
	while (key_holder >> key_id)
	{
		++food_number;
		map_table[key_id] = mapIndex++;			//对于读取到的食物节点，依次映射为0,1,2,...

		if (!(key_holder >> rubbish))			//关键节点后没有'|'符号了则中断循环
			break;
	}

	unordered_map<VIndex, unordered_map<VIndex, pair<EIndex, int>>> temp_graph;	//读取到的图的信息先存在这里面

	while (graph_holder	>> edge_id >> rubbish
						>> src_id >> rubbish
						>> dst_id >> rubbish
						>> cost)
	{
		//如果读取到的起点没有映射过，则映射后将其更改为映射值，否则直接更改为映射值
		if (map_table.find(src_id) == map_table.end())
		{
			map_table[src_id]=mapIndex++;
			src_id = map_table[src_id];
		}
		else
			src_id = map_table[src_id];			

		//如果读取到的终点没有映射过，则映射后将其更改为映射值，否则直接更改为映射值
		if (map_table.find(dst_id) == map_table.end())
		{
			map_table[dst_id]=mapIndex++;
			dst_id = map_table[dst_id];
		}
		else
			dst_id = map_table[dst_id];
		
		//先把数据存在temp_graph中
		auto& temp = temp_graph[src_id];
		//如果src_id -> dst_id之间的边已经存在了，则取cost最小的边，否则直接存入
		if (temp.find(dst_id) == temp.end())
			temp[dst_id] = { edge_id, cost };
		else
		{
			if (cost < temp[dst_id].second)
				temp[dst_id] = { edge_id, cost };
		}
	}

	//为graph和adj_graph分配空间
	auto& graph = getGraph();
	graph.resize(map_table.size());
	for (auto& subGraph : graph){
		subGraph.resize(map_table.size());
	}
	auto& adj_graph = getAdjGraph();
	adj_graph.resize(graph.size());

	//将数据存入graph和adj_graph中，并为边的信息赋初值
	for (int i = 0; i != graph.size(); ++i)
	{
		for (auto& tmp : temp_graph[i])
		{
			adj_graph[i].push_back(tmp.first);
			auto& edge = graph[i][tmp.first];
			edge._adj_e_home._info = init_info;
			edge._adj_e_index = tmp.second.first;
			edge._adj_e_ftable.resize(food_number, init_info);
			edge._adj_e_weight = tmp.second.second;
		}
	}

	getSrc() = map_table[src];
	getDst() = map_table[dst];

	getFoodNum() = food_number;
	return;
}
