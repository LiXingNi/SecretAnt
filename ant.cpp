#include <iostream>
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
	//��ʼ�����ɱ�
	_forbiden_table.clear();
	_forbiden_table.insert(getDst());  //��ʼʱ���ɱ��о����յ�

	//��ʼ��ʳ�����
	_food_bag = 0;
	//��ʼ���Ѿ��ռ�����ʳ������
	_food_num = 0;

	//��ʼ��·�����ϣ��������
	_path.resize(0);
	_path.push_back(getSrc());


	//��ʼ��һЩ��Ա
	_loc = getSrc();	//��ǰλ�ó�ʼ��Ϊ���
	_rand_fac = AntRandNum();
	_path_weight = 0;
	_survive = false;			//��ʼ���ɹ���־λΪ����
	_mistake = false;			//��ʼ������û�з���������һ�δ���
}


bool Ant::isForbidden(VIndex index)
{
	return _forbiden_table.find(index) != _forbiden_table.end();
}

bool Ant::isMistake()
{
	if (_mistake == true)	//����������
		return false;		//��������
	if (u_mistake(e) < _ant_system._mrate)  //�ڷ��������
	{
		_mistake = true; //��¼�Ѿ�������
		return true;
	}
	else
		return false;
}

//������յ㣬���ܻ�ֱ��ȥ���յ㣬����ʳ�ﶥ�㲻ֱ�ӹ�ȥ
//�����յ㣬�򷵻� 1
//��������㣬�򷵻� -1
int Ant::checkAdj()  
{
	//�����Χ�ĵ�
	auto & adj_graph = getAdjGraph();
	auto & dest_index = getDst();
	vector<VIndex> & adj_index = adj_graph[_loc]; //ȡ����ǰ������ڽӼ���
	for (auto & dst : adj_index) //�ڽӶ���
	{
		if (dst == dest_index && !isForbidden(dst)) //��ǰ�ڽӵ����յ㣬�Ҵ�ʱ�յ㲻�ڽ��ɱ���
			return 1; //�յ�
	}
	return -1; //��ͨ����
}



int Ant::chooseWay()
{
	auto & adj_graph = getAdjGraph();
	auto & graph = getGraph();

	vector<VIndex> & adj_index = adj_graph[_loc]; //��ǰ������ڽӼ���
	VIndex min_vertex;
	Info	min_info = DBL_MAX;
	vector<pair<VIndex,Info>> unf_info;  //��Ϣ�����Ķ��㼯��
	Info total_info = 0;

	for (auto dst : adj_index) //�ڽӵ�
	{
		if (isForbidden(dst))  //��ǰ�ڽӶ����ǽ��ɵ�
			continue;
		Info curr_info = _ant_system.calInfo(*this, dst);
		Weight curr_weight = graph[_loc][dst]._adj_e_weight;

		_ant_system.isFood(dst) ? curr_weight = calcRealWeight(curr_weight, 1) : 1;
		
		curr_info /= (curr_weight * curr_weight);

		total_info += curr_info;  //ͳ��������Ϣ��
		unf_info.push_back({ dst,curr_info });

		if (curr_info < min_info)
		{
			min_info = curr_info;
			min_vertex = dst;
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
	
}

void Ant::updateFoodBag(VIndex index)
{
	++_food_num;
	setN(_food_bag, index); //����ʳ�ﱳ��
	if (_food_num == getFoodNum())  //�������Ƿ�װ��
	{
		auto iter = _forbiden_table.find(getDst());
		_forbiden_table.erase(iter);
	}
}


void Ant::dealCommonVIndex(VIndex index)
{
	//����·��
	_path.push_back(index);
	//������ɱ�
	_forbiden_table.insert(index);
	//���µ�ǰλ��
	_loc = index;
}

void Ant::dealFoodVIndex(VIndex index)
{
	//�봦����ͨ����һ��
	dealCommonVIndex(index);
	//����ʳ�����
	updateFoodBag(index);
}


int Ant::moveNext()  //�����ƶ������ض���ֵ������-2 ��Ϊ�����յ㣬���� -1��Ϊ����
{
	VIndex index;  //ѡ�еĶ���

	if ((index = checkAdj()) > 0 && !isMistake()) //���ҵ��յ㣬����һ���ĸ��ʷ��������������£��������������յ�
	{
		dealCommonVIndex(getDst()); //����ĵ����յ�
		return -2;	//���ϴ��
	}
	else if ((index = chooseWay()) < 0) //�ڿ��е��а�����Ϣ�����ң����� index >= 0 ���ҵ����е㣬���� < 0������Χ�޿��е�
	{
		return -1;	 //��������
	}
	else if (index == getDst()) // ��������ٴ�ѡ�����յ㣬������յ�Ĵ���
	{
		dealCommonVIndex(index);
		return -2;
	}
	else
	{
		//�ж���ѡ�еĵ���ʳ�ﶥ�㻹����ͨ���㣬�ֱ���ò�ͬ�Ĵ�����
		_ant_system.isFood(index) ? dealFoodVIndex(index) : dealCommonVIndex(index);

		return index;  //���ϼ����˶�
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
	resetAnt();		//��������

	int move_res;

	auto & graph = getGraph();

	while ((move_res = moveNext()) >= 0)
	{
		//�ۼ�·��Ȩ��
		_path_weight += graph[_path[_path.size() - 2]][move_res]._adj_e_weight;
	}

	if (move_res == -1) //��������
		_survive = false;
	else
	{
		_path_weight += graph[_path[_path.size() - 2]][getDst()]._adj_e_weight;
		_survive = true;
	}

	//��Ȩ�ص���Ϊÿ�ռ�һ��ʳ����Ҫ�ƶ��ľ���
	_path_weight = calcRealWeight(_path_weight, _food_num); //����ÿֻ���ϴ洢��Ȩ�ء���Ϊ ·�� / ʳ������

}
