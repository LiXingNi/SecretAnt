#include <iostream>

#include "t1.h"


////////////////////////////// ANT /////////////////////////////////
void AntColonySystem::Ant::resetAnt()
{
	_loc = _ant_system._src;	//��ǰλ�ó�ʼ��Ϊ���
	
	//�ÿո�������
	_path.resize(0);
	_food_bag.clear();
	_forbiden_table.clear();
	_forbiden_table.insert(_ant_system._dest);  //��ʼʱ���ɱ��о����յ�
}


bool AntColonySystem::Ant::isForbidden(VIndex index)
{
	return _forbiden_table.find(index) != _forbiden_table.end();
}

bool AntColonySystem::Ant::isMistake()
{
	static default_random_engine e;
	static uniform_int_distribution<int> u(0, 99);
	int random_num = static_cast<int>(_ant_system._mrate * 100);
	if (u(e) < random_num)  //�ڷ��������
		return true;
	else
		return false;
}


int AntColonySystem::Ant::checkAdj()
{
	//�����Χ�ĵ�
	AdjSet & adj_set = _ant_system._graph[_loc];
	for (auto iter = adj_set.begin(); iter != adj_set.end(); ++iter)
	{
		// ��ʳ�ﶥ�㣬�Ҳ��ǽ��ɶ���
		if (_ant_system._food.find(iter->first) != _ant_system._food.end() &&
			!isForbidden(iter->first)) 
			return iter->first;
	}
	return -1;
}


int AntColonySystem::Ant::chooseWay()
{
	AdjSet& adj_set = _ant_system._graph[_loc]; //��ǰ������ڽӱ�
	vector<VIndex> unf_vertex;	//�ǽ����ڽӶ���
	vector<VIndex> max_vertex;  //��Ϣ�����Ķ��㼯��
	Info	max_info = DBL_MIN;			//�����Ϣ��
	for (auto iter = adj_set.begin(); iter != adj_set.end(); ++iter)
	{
		if (isForbidden(iter->first))  //��ǰ�ڽӶ����ǽ��ɵ�
			continue;
		unf_vertex.push_back(iter->first);  //����ǽ��ɶ��㼯��

		Info curr_info = _ant_system.calInfo(*this, iter->first);
		if (curr_info > max_info)
		{
			max_info = curr_info;
			max_vertex.resize(0);
			max_vertex.push_back(iter->first);
		}
		else if (curr_info == max_info)
		{
			max_vertex.push_back(iter->first);
		}
		else;
	}

	//�Ҳ������нڵ㡣��������
	if (unf_vertex.size() == 0)  
		return -1;
	else if (isMistake())
	{
		static default_random_engine e;
		uniform_int_distribution<int> u(0, unf_vertex.size() - 1);
		return unf_vertex[u(e)];	//�������������ѡȡ
	}
	else if (max_vertex.size() == 1) return max_vertex[0];

	else;

	//���ж���������Ϣ����ͬ��������������ѡȡ
	//����ǰ��ѡ�Ķ���������������ӣ���ֱ��ѡȡ��������������ӶԶ��㼯�ϵĴ�Сȡ��
	int s_vec = max_vertex.size();
	if (_rand_fac < s_vec)
		return max_vertex[_rand_fac];
	else
		return max_vertex[_rand_fac % s_vec];
}

void AntColonySystem::Ant::moveNext()
{
	VIndex index;  //ѡ�еĶ���
	if ((index = checkAdj()) >= 0) //�ҵ�ʳ�ﶥ��
	{
		_ant_system.activeFood(*this, index); //���ǰ����·���ϵ�ʳ����Ϣ����
		//_ant_system.localUpdate(*this, index); //�ҵ���ʳ�ﶥ�㣬�ڽ�ʳ����뱳��ǰҪ�ȸ���ʳ����Ϣ����
		//�ȸ�����Ϣ�����ٸ��±����е�ʳ����������һ�仰����λ����һ�仰ǰ��
		updateFoodBag(index);		//����ʳ�ﱳ������ʳ�ﱳ��װ�������յ��Ƴ����ɱ�
	}
	else if ((index = chooseWay()) < 0) //�Ҳ������ʵķǽ��ɶ���
	{
		resetAnt();	//��������
		return;
	}
	else
		_ant_system.localUpdate(*this, index); //ѡ����ͨ�Ķ��㣬������Ⱥϵͳ

	_path.push_back(index);
	_forbiden_table.insert(index);
	_loc = index;	//���½ڵ�λ��
}