#include "antColonySystem.h"
#include <fstream>
#include <sstream>

void loadFile
(const string& graph_file, const string& key_file){
	std::fstream graph_holder, key_holder;

	graph_holder.open(graph_file), key_holder.open(key_file);

	//edge_id:��������src_id:��������������dst_id:���ﶥ��������cost:Ȩ�أ�key_id:�ؼ��ڵ�
	VIndex	src_id, dst_id, key_id;		
	EIndex	edge_id;
	Weight  cost;				

	VIndex  src, dst;				//��ȡ����յ�
	char	rubbish;				//������ȡ���������ź�|�ȷָ���
	int		mapIndex(0);			//������¼��ǰӳ���ֵ
	Number	food_number(0);			//��¼ʳ����

	key_holder >> src >> rubbish
			   >> dst >> rubbish;				//���������յ�

	auto& map_table = getV2rTable();
	while (key_holder >> key_id)
	{
		++food_number;
		map_table[key_id] = mapIndex++;			//���ڶ�ȡ����ʳ��ڵ㣬����ӳ��Ϊ0,1,2,...

		if (!(key_holder >> rubbish))			//�ؼ��ڵ��û��'|'���������ж�ѭ��
			break;
	}

	unordered_map<VIndex, unordered_map<VIndex, pair<EIndex, int>>> temp_graph;	//��ȡ����ͼ����Ϣ�ȴ���������

	while (graph_holder	>> edge_id >> rubbish
						>> src_id >> rubbish
						>> dst_id >> rubbish
						>> cost)
	{
		//�����ȡ�������û��ӳ�������ӳ��������Ϊӳ��ֵ������ֱ�Ӹ���Ϊӳ��ֵ
		if (map_table.find(src_id) == map_table.end())
		{
			map_table[src_id]=mapIndex++;
			src_id = map_table[src_id];
		}
		else
			src_id = map_table[src_id];			

		//�����ȡ�����յ�û��ӳ�������ӳ��������Ϊӳ��ֵ������ֱ�Ӹ���Ϊӳ��ֵ
		if (map_table.find(dst_id) == map_table.end())
		{
			map_table[dst_id]=mapIndex++;
			dst_id = map_table[dst_id];
		}
		else
			dst_id = map_table[dst_id];
		
		//�Ȱ����ݴ���temp_graph��
		auto& temp = temp_graph[src_id];
		//���src_id -> dst_id֮��ı��Ѿ������ˣ���ȡcost��С�ıߣ�����ֱ�Ӵ���
		if (temp.find(dst_id) == temp.end())
			temp[dst_id] = { edge_id, cost };
		else
		{
			if (cost < temp[dst_id].second)
				temp[dst_id] = { edge_id, cost };
		}
	}

	//Ϊgraph��adj_graph����ռ�
	auto& graph = getGraph();
	graph.resize(map_table.size());
	for (auto& subGraph : graph){
		subGraph.resize(map_table.size());
	}
	auto& adj_graph = getAdjGraph();
	adj_graph.resize(graph.size());

	//�����ݴ���graph��adj_graph�У���Ϊ�ߵ���Ϣ����ֵ
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
