#include <iostream>
#include <ctime>
<<<<<<< HEAD
#include "t1.h"

const double AntColonySystem::_init_info = 10000;	//��ʼ��Ϣ��
default_random_engine AntColonySystem::Ant::e(std::time(0));
uniform_int_distribution<int> AntColonySystem::Ant::u(0, 7); 
uniform_real_distribution<double> AntColonySystem::Ant::u_mistake; 
AntColonySystem::Info AntColonySystem::EInfo::Qmax, AntColonySystem::EInfo::Qmin;
double AntColonySystem::EInfo::ratio;

=======
#include "ant.h"

const double AntColonySystem::_init_info = 10000;	//��ʼ��Ϣ��
default_random_engine AntColonySystem::Ant::e(std::time(0));
uniform_int_distribution<int> AntColonySystem::Ant::u(0, 7); 
uniform_real_distribution<double> AntColonySystem::Ant::u_mistake; 
AntColonySystem::Info AntColonySystem::EInfo::Qmax, AntColonySystem::EInfo::Qmin;
double AntColonySystem::EInfo::ratio;

>>>>>>> refs/remotes/LiXingNi/master
double getPbest()
{
	static double pBest = 0.005;
	return pBest;
}

////////////////////////////// ANT /////////////////////////////////
void AntColonySystem::Ant::resetAnt()
{
	//��ʼ�����ɱ�
	_forbiden_table.clear();
	_forbiden_table.insert(_ant_system._dest);  //��ʼʱ���ɱ��о����յ�

	//��ʼ��ʳ�����
	_food_bag.clear();

	//��ʼ��·�����ϣ��������
	_path.resize(0);
	_path.push_back(_ant_system._src);


	//��ʼ��һЩ��Ա
	_loc = _ant_system._src;	//��ǰλ�ó�ʼ��Ϊ���
	_rand_fac = AntRandNum();
	_path_weight = 0;
	_survive = false;			//��ʼ���ɹ���־λΪ����
}


bool AntColonySystem::Ant::isForbidden(VIndex index)
{
	return _forbiden_table.find(index) != _forbiden_table.end();
}

bool AntColonySystem::Ant::isMistake()
{
	if (u_mistake(e) < _ant_system._mrate)  //�ڷ��������
		return true;
	else
	{
		return false;
	}
}

//�����Χ�Ķ����Ƿ��Ƿǽ��ɱ��е�ʳ�ﶥ����յ�
//����ʳ�ﶥ�㣬�򷵻ظ�ʳ�ﶥ�� >= 0;
//�����յ㣬�򷵻� -2
//��������㣬�򷵻� -1
int AntColonySystem::Ant::checkAdj()  
{
	//�����Χ�ĵ�
	AdjSet & adj_set = _ant_system._graph[_loc];
	for (auto iter = adj_set.begin(); iter != adj_set.end(); ++iter)
	{
		if (!isForbidden(iter->first)) //���Ƿǽ��ɶ���
		{
			if (_ant_system.isFood(iter->first))  //����ʳ�ﶥ�㣬��ֱ�ӷ��ظö���
				return iter->first;
			else if (iter->first == _ant_system._dest) //�����յ㣬�򷵻�-2
				return -2;
		}
	}
	return -1; //��ͨ���㣬���� -1
}


int AntColonySystem::Ant::chooseWay()
{
	AdjSet& adj_set = _ant_system._graph[_loc]; //��ǰ������ڽӱ�
	//vector<VIndex> unf_vertex;	//�ǽ����ڽӶ���
	VIndex min_vertex;
	Info	min_info = DBL_MAX;
	vector<pair<VIndex,Info>> unf_info;  //��Ϣ�����Ķ��㼯��
	Info total_info = 0;

	for (auto iter = adj_set.begin(); iter != adj_set.end(); ++iter)
	{
		if (isForbidden(iter->first))  //��ǰ�ڽӶ����ǽ��ɵ�
			continue;
		Info curr_info = _ant_system.calInfo(*this, iter->first);
		Weight curr_weight = _ant_system._graph[_loc][iter->first]._adj_e_weight;
		curr_info /= (curr_weight * curr_weight);

		total_info += curr_info;  //ͳ��������Ϣ��
		unf_info.push_back({ iter->first,curr_info });

		if (curr_info < min_info)
		{
			min_info = curr_info;
			min_vertex = iter->first;
		}
	}

	//�Ҳ������нڵ㡣��������
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

	//******** ��һ�ε���Ұ�� **********************************

	//������Ϣ������ѡ�и���
	for (auto & info : unf_info)
	{
		if (total_info == 0)
			info.second = 1.0 / unf_info.size();	//���ÿ��·��Ϣ�ؾ�Ϊ0����ÿ��·��̯����
		info.second /= total_info;					//�������
	}

	double rand_num = u_mistake(e);					//����һ��double�������
	VIndex i = 0;
	while (rand_num > 0)							//������rand_num�м�ȥÿһ������ֵ�������Ϊ������0����ѡ������·��
	{
		rand_num -= unf_info[i].second;
		++i;					
	}
	return unf_info[i - 1].first;					//��Ϊ��ѭ��������������++i,�����ﷵ��i-1������ֵ��
	
	//******** ��һ�ε���Ұ�� **********************************
}

void AntColonySystem::Ant::updateFoodBag(VIndex index)
{
	_food_bag.insert(index);
	if (_food_bag.size() == _ant_system._food.size())  //�������Ƿ�װ��
	{
		auto iter = _forbiden_table.find(_ant_system._dest);
		_forbiden_table.erase(iter);
	}
}

//void AntColonySystem::Ant::terminate()
//{
//	_ant_system.updataPath(_path);
//	//���ϵ��Һ����·����Ϣ���棬�����ظ���һ��·��
//	//_ant_system.activeClear(*this);
//}


void AntColonySystem::Ant::dealCommonVIndex(VIndex index)
<<<<<<< HEAD
{
	//����·��
	_path.push_back(index);
	//������ɱ�
	_forbiden_table.insert(index);
	//���µ�ǰλ��
	_loc = index;
}

void AntColonySystem::Ant::dealFoodVIndex(VIndex index)
{
	//�봦����ͨ����һ��
	dealCommonVIndex(index);
	//����ʳ�����
	updateFoodBag(index);
}

=======
{
	//����·��
	_path.push_back(index);
	//������ɱ�
	_forbiden_table.insert(index);
	//���µ�ǰλ��
	_loc = index;
}

void AntColonySystem::Ant::dealFoodVIndex(VIndex index)
{
	//�봦����ͨ����һ��
	dealCommonVIndex(index);
	//����ʳ�����
	updateFoodBag(index);
}

>>>>>>> refs/remotes/LiXingNi/master

int AntColonySystem::Ant::moveNext()  //�����ƶ������ض���ֵ������-2 ��Ϊ�����յ㣬���� -1��Ϊ����
{
	VIndex index;  //ѡ�еĶ���

	if ((index = checkAdj()) >= 0 && !isMistake()) //�����������ߣ���������ҵ�ʳ�ﶥ��
	{
		dealFoodVIndex(index);
	}
	else if (index == -2 && !isMistake()) //�ڽӵ������յ���ʳ���ռ�����
	{
		dealCommonVIndex(_ant_system._dest);
		return -2;  //����-2 �����ϴ��
	}
	else if ((index = chooseWay()) < 0) //�Ҳ������ʵķǽ��ɶ���
	{
		return -1;
	}
	else dealCommonVIndex(index);  //ѡ������ͨ����

	return index;
}

void AntColonySystem::Ant::antRun()
{
	resetAnt();		//��������

	int move_res;
	while ((move_res = moveNext()) >= 0)
	{
		//�ۼ�·��Ȩ��
		_path_weight += _ant_system._graph[_path[_path.size() - 2]][move_res]._adj_e_weight;
	}

	//��Ȩ�ص���Ϊÿ�ռ�һ��ʳ����Ҫ�ƶ��ľ���
	_path_weight = double(_path_weight) / double(_food_bag.size() + 1); 

	if (move_res == -1) //��������
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
	if (cmpAnts(ant, *_best_ant)) //�����ϸ�����
	{
		_best_ant->_path = ant._path;
		_best_ant->_path_weight = ant._path_weight;
		_best_ant->_survive = ant._survive;
	}
}

bool AntColonySystem::cmpAnts(const Ant& ant1, const Ant& ant2)
{

	if ((ant1._survive && ant2._survive) || ((!ant1._survive) && (!ant2._survive)))
		//��ֻ����Ϊͬһ�����ϣ���Ƚ� power
	{
		return ant1._path_weight <= ant2._path_weight;
	}
	//��ֻ�������಻ͬʱ���յ����ϸ�����
	else if (ant1._survive)
		return true;
	else
		return false;
}

//void AntColonySystem::localUpdate(Ant & ant, AdjIndex index)  //����������ѡ����¶��㣬·����Ϣ
//{
//	AdjEdg & adj_edg = _graph[ant._loc][index]; 
//
//	// �����������,����homeȨ��
//	if (ant._food_bag.size() == _food.size())
//		adj_edg._adj_e_home.addInfo(); 
//	else{
//		//���������Ƿ�ӵ��ĳ��ʳ�����ʳ����Ϣ��
//		unordered_map<FIndex, EInfo> & adj_ftable = adj_edg._adj_e_ftable;
//		for (unordered_map<FIndex, EInfo>::iterator it = adj_ftable.begin(); it != adj_ftable.end(); ++it)
//		{
//			if (ant._food_bag.find(it->first) == ant._food_bag.end())  //������û���ռ�����ʳ��
//			{
//				it->second.addInfo();  //���Ӹ�ʳ�����Ϣ��
//			}
//		}
//	}
//}

