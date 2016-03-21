#ifndef NOTE_HELP_T1_H_
#define NOTE_HELP_T1_H_
#include <climits>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <random>
#include <set>
#include <cmath>
#include <map>
#include <cstdint>
#include <cfloat>

using std::map;
using std::set;
using std::vector;
using std::unordered_map;
using std::unordered_set;
using std::string;
using std::pair;
using std::default_random_engine;
using std::uniform_int_distribution;
using std::uniform_real_distribution;
using std::shared_ptr;

typedef int		RIndex;		//��ʵ������
typedef int		VIndex;		//ת����Ķ���
typedef int		EIndex;		//�߱��
typedef double	Weight;		//Ȩ��
typedef int		FIndex;		//ʳ����
typedef double	Info;		//��Ϣ��
typedef double	AInfo;		//��Ϣ����
typedef double	DisRate;	//��ɢϵ��
typedef int		AdjIndex;	//�ڽӶ�����
typedef	int		Number;		//ʳ��������λ

class AdjEdg;
class Ant;
class AntColonySystem;

const Info init_info = 10000;

inline unordered_map<VIndex, RIndex>& getV2rTable()	//��Ч��������ʵ����Ķ��ձ�
{
	static unordered_map<VIndex, RIndex> V2rTable;
	return V2rTable;
}
inline vector<vector<AdjEdg>>& getGraph()			//ͼ
{
	static vector<vector<AdjEdg>> graph;
	return graph;
}
inline vector<vector<VIndex>>& getAdjGraph()		//�ڽ�ͼ����¼��ÿ��������ڽӼ���
{
	static vector<vector<VIndex>> adj_graph;
	return adj_graph;
}
inline Number& getFoodNum()		//ʳ������
{
	static Number food_number(0);
	return food_number;
}

inline VIndex& getSrc()
{
	static VIndex src;
	return src;
}

inline VIndex& getDst()
{
	static VIndex dst;
	return dst;
}

inline void setN(uint64_t& num, size_t digital)	//���õ�digitalλΪ1
{
	num |= ((uint64_t)1 << digital);
}

inline void clearN(uint64_t& num, size_t digital)	//���õ�digitalλΪ0
{
	num &= !((uint64_t)1 << digital);
}

double getPbest();

void loadFile(const string& graph_file, const string& key_file);

struct EInfo
{
	Info	_info;			//��Ϣ��
	static Info Qmax, Qmin;
	static double ratio;//undefined

	EInfo(Info info = -1) : _info(info){
	}

	void addInfo(Info add_value){
		_info += add_value;
		_info > Qmax ? _info = Qmax : 1;
		_info < Qmin ? _info = Qmin : 1;
	}

	static void updateMinMax(Ant& best_ant, double rate);

	void diss(double rate){
		_info *= rate;
		_info < Qmin ? _info = Qmin : 1;
		_info > Qmax ? _info = Qmax : 1;
	}
};

struct AdjEdg
{
	EIndex _adj_e_index;	//�ڽӱߵı߱��
	Weight _adj_e_weight;	//�ڽӱߵ�Ȩ��
	vector<EInfo>	_adj_e_ftable;	//�ڽӱߵ�ʳ����Ϣ��
	EInfo	_adj_e_home;	//�ڽӱߵļ���Ϣ
};

struct Ant
{
	struct AntRandNum{
		AntRandNum(){
			_rand_num = u(e);
		}
		AntRandNum(const AntRandNum& arn){
			_rand_num = u(e);
		}
		operator int(){
			return _rand_num;
		}
		int _rand_num;
	};
	unordered_set < VIndex >	_forbiden_table;	//���ɱ�  ����ʼʱ�����յ�
	uint64_t					_food_bag;			//�����±�洢ĳ��ʳ���Ƿ��Ѿ���ȡ
	int							_food_num;			//��¼�Ѿ��Ե���ʳ������
	vector	<VIndex>			_path;				//��ǰ���Ͼ�����·��
	VIndex						_loc;				//���ϵ�ǰ����λ��
	AntColonySystem&			_ant_system;		//��Ⱥ��������
	AntRandNum					_rand_fac;			//���ϵ��������

	Weight						_path_weight;		//��ǰ����Ȩ��
	bool						_survive;			//�ɹ������յ������Ϊtrue
	bool						_mistake;			//�Ƿ񷸹���

	Ant& operator=(const Ant& rhs){
		_forbiden_table = rhs._forbiden_table;
		_food_bag = rhs._food_bag;
		_food_num = rhs._food_num;
		_path = rhs._path;
		_loc = rhs._loc;
		_rand_fac = rhs._rand_fac;
		_path_weight = rhs._path_weight;
		_survive = rhs._survive;
		_mistake = rhs._mistake;
		return *this;
	}

	static default_random_engine e;
	static uniform_int_distribution<int> u; 
	static uniform_real_distribution<double> u_mistake; 

public:
	void resetAnt();		//������������
	int moveNext();	//���������ƶ�����һ��λ�ã�����������Ⱥ�ֲ���Ϣ�ĸ���
	int checkAdj();		//����ڽӶ����Ƿ��зǽ��ɵ�ʳ�ﶥ��, ����ʳ�ﶥ�㣬����>=0; ���ޣ�����-1
	void updateFoodBag(VIndex);	//�ߵ��µ�ʳ�ﶥ��ʱ����ʳ�����,��װ���ˣ����յ��Ƴ����ɱ�
	int chooseWay();		//����Χ�ǽ��ɵ��ڽӶ����Һ��ʵĶ��㣬��һ����ʧ���� �����ҵ��˺��ʵ��ڽӶ��� ,���� >=0������������ ,���� < 0

	void dealCommonVIndex(VIndex);	//������ͨ���㣬�յ�Ҳ������ͨ����
	void dealFoodVIndex(VIndex);	//����ʳ�ﶥ��

	//һЩ�������ܵĽӿں���
	Weight calcRealWeight(Weight,int);		//����ʳ����������������Ȩ��

	void antRun();			//�����˶�
	
	inline bool isForbidden(VIndex); //������ڽӶ����Ƿ��ǽ��ɵ�
	bool isMistake();

	Ant(AntColonySystem & s) :_ant_system(s)
	{
		//��̬���������
		resetAnt();
	}
};

struct Err
{
	string msg;
	Err(const string & m) :msg(m){}
};


class AntColonySystem
{

public:

public:


private:
	void loadInfo(const string& f1, const string& f2);		//����ͼ��Ϣ������ʳ����Ϣ

public:

	DisRate		_frate;						//ʳ����Ϣ��ɢ��
	DisRate		_hrate;						//����Ϣ��ɢ��
	size_t		_ants_num;					//��������
	vector<Ant> _ants;						//���ϼ�Ⱥ
	size_t		_steps_num;					//����

	bool				_any_arrived;		//��ǰ���Ž��Ƿ�Ϊ�����յ�����Ž�
	shared_ptr<Ant>		_best_ant;			//ȫ���������
	double				_mrate;				//���Ϸ������

	static const Weight		_init_info;		//��ʼ��Ϣ��

public:
	AntColonySystem(double frate, double hrate, double mis_rate, size_t ants_num, size_t steps_num)  //����˳��ʳ����Ϣ����ɢ�ʣ�����Ϣ����ɢ�ʣ����Ϸ�����ʣ����������������
		:_frate(frate), _hrate(hrate), _mrate(mis_rate)
		, _ants_num(ants_num), _steps_num(steps_num)
	{

		EInfo::Qmax = EInfo::Qmin = 0;
		double city_num = getGraph().size();
		double temp = std::exp(std::log(getPbest()) / city_num); //��Pbest��N_CITY_COUNT�η�
		EInfo::ratio = (2.0 / temp - 2.0) / (city_num - 2.0);

		_ants.resize(_ants_num, *this);

		//��ʼ���������
		_best_ant = shared_ptr<Ant>(new Ant(*this)); 
		_best_ant->_path_weight = INT_MAX;
		_best_ant->_survive = false;	//�ȳ�ʼ��Ϊʳ������
	}
	
	double calInfo(Ant&, AdjIndex);			//����������Ϣ����������ϵ��ڽӶ������Ϣ���ܺ�
	void updateBestAnt(Ant&);				//����ȫ����������
	void run();								//�пغ���
	void infoDiss();						//��Ϣ˥��
	bool isFood(VIndex);					//�ж�ĳ�������Ƿ���ʳ�ﶥ��		


	bool cmpAnts(const Ant &, const Ant&);				//����һֻ���ϱȵڶ�ֻ��(<=)���򷵻�true������(>)������ false
	Ant& chooseBestAnt(bool);				//����������ϣ��β�����ѡ��ȫ����ѻ�ֲ���ѣ�����updataPath����ȫ���������
	void updateInfo(Ant&);					//���¼�¼��Ϣ��������������ϵ���·����Ϣ��


	//һЩ���ܲ����ӿ�
	void adjustMisRate(double rate)
	{
		if (_best_ant->_survive)
			_mrate += rate;
	}

};


#endif
