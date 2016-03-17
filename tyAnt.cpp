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
	char	rubbish;				//������ȡ���������ź�|�ȷָ���
	//edge_id:��������src_id:��������������dst_id:���ﶥ��������cost:Ȩ�أ�key_id:�ؼ��ڵ�

	while (graph_holder	>> edge_id >> rubbish
						>> src_id >> rubbish
						>> dst_id >> rubbish
						>> cost)
	{
		if (_graph[src_id].find(dst_id) != _graph[src_id].end()) //�����Ѵ��ڵ��ظ�·����������Ȩ����С������
		{
			Weight& _e_weight = _graph[src_id][dst_id]._adj_e_weight;
			_e_weight = std::min(_e_weight, cost);
		}											
		else
			_graph[src_id][dst_id] = AdjEdg(edge_id, cost);		//��·����������ֱ����ӵ�ͼ�С�
	}

	key_holder >> _src >> rubbish
			   >> _dest>> rubbish;				//���������յ�

	while (key_holder >> key_id)
	{
		_food.insert(key_id);
		if (!(key_holder >> rubbish))			//�ؼ��ڵ��û��'|'���������ж�ѭ��
			break;
	}

	return;
}

double AntColonySystem::calInfo(Ant& ant, AdjIndex adj_index)
{
	const AdjEdg& adj_edge = _graph[ant._loc][adj_index];					//ȡ�������ߵ�����
	if (_food.size() == ant._food_bag.size())								//���ʳ���������������ʳ���������򷵻�home��Ϣ��
		return adj_edge._adj_e_home._info;

	Info food_info(0);														//��ʼ��ʳ����Ϣ��

	for (auto& food : adj_edge._adj_e_ftable)
	{
		if (ant._food_bag.find(food.first) == ant._food_bag.end())	//��������в����е�ǰ�ߵ�ʳ����Ϣ��x�����ۼӸ���Ϣ��
			food_info += food.second._info;
	}

	return food_info;														//����ʳ����Ϣ��  
}

void AntColonySystem::activeFood(Ant& ant, FIndex food_index)
{
	vector<VIndex>& ant_path = ant._path;										//ȡ����ǰ���Ͼ�����·��
	Weight sum_weight = 0;														//���ӵ�Ȩ�غ�
	VIndex src, dst(food_index);												//��ʼ��Ŀ�Ľڵ㣬Ŀ�Ľڵ��ʼ��Ϊ������ʳ��ڵ�

	for (auto riter = ant_path.rbegin(); riter != ant_path.rend(); ++riter)
	{
		src = *riter;															//�������·��
		AdjEdg& adj_edg = _graph[src][dst];										//ȡ����ʼ��Ŀ�Ľڵ��·����Ϣ
		EInfo& edg_info = adj_edg._adj_e_ftable[food_index];					//ȡ��·����Ϣ��ʳ����Ϣ�صı�������û�л��Զ��½���Ĭ�ϳ�ʼ��)
		sum_weight += adj_edg._adj_e_weight;									//����Ȩ�غ�
		edg_info._ainfo = std::max(edg_info._ainfo, 1.0 / static_cast<double>(sum_weight));	//����Ӧʳ���������Ϊ��ǰ����ͽϴ������еĴ���Ǹ�
		edg_info.addInfo();										//�Լ�һ�ζ�Ӧʳ�����Ϣ��
		dst = src;																//��Ŀ�Ľڵ���Ϊ��ǰ�ڵ㲢������һ��ѭ��
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
	for (auto& src : _graph)										//ȡ��ͼ�е�һ����ʼ�ڵ�������������
	{
		for (auto& edge : src.second)								//ȡ������ʼ�ڵ��һ������
		{
			EInfo& home_info = edge.second._adj_e_home;				
			home_info._info *= _hrate;								//����Ϣ����ɢ
			for (auto& food_info : edge.second._adj_e_ftable)
				food_info.second._info *= _frate;					//�����ñ����е�ʳ����Ϣ�ز���ɢ
		}
	}
}