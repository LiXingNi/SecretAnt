#include "antColonySystem.h"

bool AntColonySystem::isFood(VIndex index)
{
	return index < getFoodNum();
}

void AntColonySystem::updateBestAnt(Ant& ant)
{
	if (cmpAnts(ant, *_best_ant)) //�����ϸ�����
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
		//��ֻ����Ϊͬһ�����ϣ���Ƚ� power
	{
		return ant1._path_weight < ant2._path_weight;
	}
	//��ֻ�������಻ͬʱ���յ����ϸ�����
	else if (ant1._survive)
		return true;
	else
		return false;
}


////******************���޸�
//double AntColonySystem::calInfo(Ant& ant, AdjIndex adj_index)
//{
//	auto & _graph = getGraph();
//	AdjEdg& adj_edge = _graph[ant._loc][adj_index];					//ȡ�������ߵ�����
//	if (getFoodNum() == ant._food_num)								//���ʳ���������������ʳ���������򷵻�home��Ϣ��
//		return adj_edge._adj_e_home._info;
//
//	Info food_info(0);														//��ʼ��ʳ����Ϣ��
//	vector<pair<VIndex,EInfo>> & food_table = adj_edge._adj_e_ftable;
//	for (auto food_iter = food_table.begin(); food_iter != food_table.end(); ++ food_iter)
//	{
//		if (ant._food_bag[_food[food_iter->first]] == 0)	//��������в����е�ǰ�ߵ�ʳ����Ϣ��x�����ۼӸ���Ϣ��
//			food_info += food_iter->second._info;
//	}
//
//
//	food_info += (_food.size() - adj_edge._adj_e_ftable.size()) * _record_info;
//
//	return food_info;														//����ʳ����Ϣ��  
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
	uint64_t food_bag = ant._food_bag;

	int loc = 0;
	while (food_num--)
	{
		//ȡ�����λ
		if ((food_bag & 1) == 0)  //food_bag��ǰλΪ0 ������ӵ�ǰʳ�����Ϣ��
			food_info += food_table[loc]._info;
		++loc;
		food_bag >>= 1;   //ȡ����һλ
	}
	return food_info;
}


//ÿ�Ĵ�ѭ��ѡ��ֲ���ѣ�һ��ѡ��ȫ�����
//�ֲ���� �� true
//ȫ����� �� false
void AntColonySystem::run()
{
	int count = 0;
	for (int i(0); i != _steps_num; ++i)
	{
		if (i % 20 == 0)
			cout << i << endl;
		for (auto & ant : _ants)
			ant.antRun();
		bool flag = count++ % 5 == 0 ? false : true;

		Ant & best_ant = chooseBestAnt(flag);
		updateInfo(best_ant);

		//adjustMisRate(0.0006);
	}
}



//* method:
//* false->ѡȡȫ����������
//* true->ѡȡ������������ 
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

	//������ɢ����
	for (int src = 0; src != _adj_graph.size(); ++src)
	{
		vector<VIndex> & adj_index = _adj_graph[src];
		for (auto & dst : adj_index)
		{
			//�����ڽӶ��� src->dst �ļ�Ȩ��
			_graph[src][dst]._adj_e_home.diss(_hrate);

			//�����ڽӶ��� src->dst ��ʳ�ﶥ��
			auto & adj_food_table = _graph[src][dst]._adj_e_ftable;
			for (auto & food_info: adj_food_table)
			{
				food_info.diss(_frate);
			}
		}
	}

	//�������������ɢ����
	vector<int> food_vec;	//��Ҫ���µ�ʳ���

	bool update_home(false);
	ant._survive ? update_home = true : 1;  //�������Ǵ��ģ������Home ��Ϣ��

	auto path = ant._path;
	double add_value = 1.0 / ant._path_weight;

	for (int i = path.size() - 1; i > 0; --i)
	{
		if (update_home)
			_graph[path[i - 1]][path[i]]._adj_e_home.addInfo(add_value);

		if (isFood(path[i]))  //����ǰ������ʳ�ﶥ�㣬���������Ҫ���µ�ʳ���
			food_vec.push_back(path[i]);

		auto & ftable = _graph[path[i - 1]][path[i]]._adj_e_ftable;

		for (auto food_index : food_vec)  //������Ҫ���µ�ʳ���������Ϣ��
		{
			ftable[food_index].addInfo(add_value);
		}
	}

}

void EInfo::updateMinMax(Ant& best_ant, double rate){
	Qmax = 1.0 / (best_ant._path_weight *(1 - rate));
	Qmin = Qmax * ratio;
}