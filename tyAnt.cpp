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
	Weight  cost;
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

	food_info += (_food.size() - adj_edge._adj_e_ftable.size()) * _record_info;

	return food_info;														//����ʳ����Ϣ��  
}

/*void AntColonySystem::activeFood(Ant& ant, FIndex food_index)
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
}*/

//ÿ�Ĵ�ѭ��ѡ��ֲ���ѣ�һ��ѡ��ȫ�����
//�ֲ���� �� true
//ȫ����� �� false
void AntColonySystem::run()
{
	int count = 0;
	for (int i(0); i != _steps_num; ++i)
	{
		if (i % 200 == 0)
			cout << i << endl;
		for (auto & ant : _ants)
			ant.antRun();
		bool flag = count++ % 5 == 0 ? false : true;

		Ant & best_ant = chooseBestAnt(flag);
		updateInfo(best_ant);
	}
}


/*void AntColonySystem::infoDiss(){
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

void AntColonySystem::activeClear(Ant& ant){
	for (int i = 1; i < ant._path.size(); ++i)
	{
		VIndex curr = ant._path[i];
		VIndex prev = ant._path[i - 1];
		AdjEdg& edge = _graph[prev][curr];
		if (ant._food_bag.size() == _food.size())
			edge._adj_e_home._info = 0;
		for (auto & food : ant._food_bag)
		{
			edge._adj_e_ftable[food]._info = 0;
			edge._adj_e_ftable[food]._ainfo = 0;
		}
	}
}*/

//* method:
//* false->ѡȡȫ����������
//* true->ѡȡ������������ 
AntColonySystem::Ant& AntColonySystem::chooseBestAnt(bool method) {
	auto ant_iter = std::max_element(_ants.begin(), _ants.end(), [this](const Ant& x, const Ant& y){return !this->cmpAnts(x, y); });//��Ҫ�����,�ȵ�������
	updateBestAnt(*ant_iter);
	switch (method){
	case false: return *_best_ant;
	case true: return *ant_iter;
	default: throw std::runtime_error("error method in chooseBestAnt function");
	}
}

void AntColonySystem::updateInfo(Ant& ant){
	EInfo::updateMinMax(ant, _frate);///question
	_record_info *= _frate;
	_record_info < EInfo::Qmin ? _record_info = EInfo::Qmin : 1;
	_record_info > EInfo::Qmax ? _record_info = EInfo::Qmax : 1;

	for (auto & src : _graph) {
		for (auto &edge : src.second){
			edge.second._adj_e_home.diss(_hrate);
			for (auto &food_info : edge.second._adj_e_ftable)
			{
				food_info.second.diss(_frate);
			}
		}
	}

	vector<int> food_vec;
	bool update_home(false);
	if (ant._survive)
		update_home = true;
	auto path = ant._path;
	double add_value = 1.0 / ant._path_weight;
	for (int i = path.size() - 1; i > 0; --i)
	{
		if (update_home)
			_graph[path[i - 1]][path[i]]._adj_e_home.diss(_frate);
		if (isFood(path[i]))
			food_vec.push_back(path[i]);
		for (auto food_index : food_vec)
		{
			_graph[path[i - 1]][path[i]]._adj_e_ftable[food_index].addInfo(add_value, _record_info);
		}
	}

}

void AntColonySystem::EInfo::updateMinMax(Ant& best_ant, double rate){
	Qmax = 1.0 / (best_ant._path_weight *(1 - rate));
	Qmin = Qmax * ratio;
}