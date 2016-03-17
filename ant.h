#ifndef NOTE_HELP_T1_H_
#define NOTE_HELP_T1_H_
#include <iostream>
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

using std::endl;
using std::cout;
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

double getPbest();

struct Err
{
	string msg;
	Err(const string & m) :msg(m){}
};


class AntColonySystem
{
	typedef int		VIndex;		//������
	typedef int		EIndex;		//�߱��
	typedef double	Weight;		//Ȩ��
	typedef int		FIndex;		//ʳ����
	typedef double	Info;		//��Ϣ��
	typedef double	AInfo;		//��Ϣ����
	typedef double	DisRate;	//��ɢϵ��
	typedef int		AdjIndex;	//�ڽӶ�����

public:
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
<<<<<<< HEAD
		set < VIndex >				_food_bag;			//��ǰ�Ѿ���ȡ��ʳ����
=======
<<<<<<< HEAD
		set < VIndex >				_food_bag;			//��ǰ�Ѿ���ȡ��ʳ����
=======
		set < VIndex >	_food_bag;			//��ǰ�Ѿ���ȡ��ʳ����
>>>>>>> origin/master
>>>>>>> refs/remotes/LiXingNi/master
		vector	<VIndex>			_path;				//��ǰ���Ͼ�����·��
		VIndex						_loc;				//���ϵ�ǰ����λ��
		AntColonySystem&			_ant_system;		//��Ⱥ��������
		AntRandNum					_rand_fac;			//���ϵ��������
<<<<<<< HEAD
=======
<<<<<<< HEAD
>>>>>>> refs/remotes/LiXingNi/master

		Weight						_path_weight;		//��ǰ����Ȩ��
		bool						_survive;			//�ɹ������յ������Ϊtrue


<<<<<<< HEAD
		static default_random_engine e;
		static uniform_int_distribution<int> u; 
		static uniform_real_distribution<double> u_mistake; 
=======
		static default_random_engine e;
		static uniform_int_distribution<int> u; 
		static uniform_real_distribution<double> u_mistake; 
=======
		static default_random_engine e;
		static uniform_int_distribution<int> u; 
		static uniform_int_distribution<int> u_mistake; 
>>>>>>> origin/master
>>>>>>> refs/remotes/LiXingNi/master

	public:
		void resetAnt();		//������������
		int moveNext();	//���������ƶ�����һ��λ�ã�����������Ⱥ�ֲ���Ϣ�ĸ���
		int checkAdj();		//����ڽӶ����Ƿ��зǽ��ɵ�ʳ�ﶥ��, ����ʳ�ﶥ�㣬����>=0; ���ޣ�����-1
		void updateFoodBag(VIndex);	//�ߵ��µ�ʳ�ﶥ��ʱ����ʳ�����,��װ���ˣ����յ��Ƴ����ɱ�
		int chooseWay();		//����Χ�ǽ��ɵ��ڽӶ����Һ��ʵĶ��㣬��һ����ʧ���� �����ҵ��˺��ʵ��ڽӶ��� ,���� >=0������������ ,���� < 0
		//void terminate();		//�����ƶ�����

		void dealCommonVIndex(VIndex);	//������ͨ���㣬�յ�Ҳ������ͨ����
		void dealFoodVIndex(VIndex);	//����ʳ�ﶥ��


		void antRun();			//�����˶�
		
		inline bool isForbidden(VIndex); //������ڽӶ����Ƿ��ǽ��ɵ�
		bool isMistake();

		Ant(AntColonySystem & s) :_ant_system(s)
		{
			//��̬���������
			resetAnt();
		}
	};

public:

	struct EInfo
	{
		Info	_info;			//��Ϣ��
<<<<<<< HEAD
=======
<<<<<<< HEAD
>>>>>>> refs/remotes/LiXingNi/master
		static Info Qmax, Qmin;
		static double ratio;//undefined

		EInfo(Info info = -1) : _info(info){
		}

		void addInfo(Info add_value, Info base_info){
			if (_info == -1)
				_info = base_info;
			_info += add_value;
			_info > Qmax ? _info = Qmax : 1;
			_info < Qmin ? _info = Qmin : 1;
		}

		static void updateMinMax(Ant& best_ant, double rate);

		void diss(double rate){
			_info *= rate;
			rate < Qmin ? rate = Qmin : 1;
			rate > Qmax ? rate = Qmax : 1;
<<<<<<< HEAD
=======
=======
		AInfo	_ainfo;			//��Ϣ����
		static const Info _max_info;
		EInfo(AInfo ainfo = 0, Info info = 0.0) :_ainfo(ainfo), _info(info){}
		void addInfo()
		{
			Info n_info = _info + _ainfo;
			_info = n_info > _max_info ? _max_info : n_info;
>>>>>>> origin/master
>>>>>>> refs/remotes/LiXingNi/master
		}
	};

	struct AdjEdg
	{
		EIndex _adj_e_index;	//�ڽӱߵı߱��
		Weight _adj_e_weight;	//�ڽӱߵ�Ȩ��
		unordered_map<FIndex, EInfo>	_adj_e_ftable;	//�ڽӱߵ�ʳ����Ϣ��
		EInfo	_adj_e_home;	//�ڽӱߵļ���Ϣ

		AdjEdg(EIndex index = 0, Weight weight = 0) :_adj_e_index(index), _adj_e_weight(weight),_adj_e_home(AntColonySystem::_init_info){ }
	};

private:
	void loadInfo(const string& f1, const string& f2);		//����ͼ��Ϣ������ʳ����Ϣ

public:
	typedef unordered_map<AdjIndex, AdjEdg> AdjSet;
	
	unordered_map<VIndex, AdjSet> _graph;	//ͼ
	unordered_set<VIndex> _food;			//�ع�����
	DisRate		_frate;						//ʳ����Ϣ��ɢ��
	DisRate		_hrate;						//����Ϣ��ɢ��
	size_t		_ants_num;					//��������
	vector<Ant> _ants;						//���ϼ�Ⱥ
	size_t		_steps_num;					//����
	VIndex		_src;						//��㶥���
	VIndex		_dest;						//�յ㶥���

	bool				_any_arrived;		//��ǰ���Ž��Ƿ�Ϊ�����յ�����Ž�
	shared_ptr<Ant>		_best_ant;			//ȫ���������
	double				_mrate;				//���Ϸ������

	Weight				_record_info;		//��¼û�м����·���ĵ�ǰ��Ϣ��
	static const Weight		_init_info;	//��ʼ��Ϣ��

public:
	AntColonySystem(const string& f1, const string & f2, double frate, double hrate, double mis_rate, size_t ants_num, size_t steps_num)  //����˳��ʳ����Ϣ����ɢ�ʣ�����Ϣ����ɢ�ʣ����Ϸ�����ʣ����������������
		:_frate(frate), _hrate(hrate), _mrate(mis_rate)
		, _ants_num(ants_num), _steps_num(steps_num)
	{
		loadInfo(f1, f2);
<<<<<<< HEAD
=======
<<<<<<< HEAD
>>>>>>> refs/remotes/LiXingNi/master

		EInfo::Qmax = EInfo::Qmin = 0;
		double city_num = _graph.size();
		double temp = std::exp(std::log(getPbest()) / city_num); //��Pbest��N_CITY_COUNT�η�
		EInfo::ratio = (2.0 / temp - 2.0) / (city_num - 2.0);

<<<<<<< HEAD
		_ants.resize(_ants_num, *this);
		_record_info = _init_info;			//��ʼ����Ϣ��

		//��ʼ���������
		_best_ant = shared_ptr<Ant>(new Ant(*this)); 
		_best_ant->_path_weight = INT_MAX;
		_best_ant->_survive = false;	//�ȳ�ʼ��Ϊʳ������
=======
		_ants.resize(_ants_num, *this);
		_record_info = _init_info;			//��ʼ����Ϣ��

		//��ʼ���������
		_best_ant = shared_ptr<Ant>(new Ant(*this)); 
		_best_ant->_path_weight = INT_MAX;
		_best_ant->_survive = false;	//�ȳ�ʼ��Ϊʳ������
=======
		_ants.resize(_ants_num, *this);
		_best_weight = INT_MAX;
>>>>>>> origin/master
>>>>>>> refs/remotes/LiXingNi/master
	}
	
	double calInfo(Ant&, AdjIndex);			//����������Ϣ����������ϵ��ڽӶ������Ϣ���ܺ�
	//void localUpdate(Ant&, AdjIIndex);		//��������ѡ��Ķ��㣬���¸ñߵ���Ϣ��
	//void activeFood(Ant&, FIndex);			//�ҵ�ʳ������ϣ�����������·���ϵ�ʳ������
	//void activeClear(Ant&);					//�����������򽫵�ǰ·���ϵ���Ϣ�����㣨��Ϣ�������Ҳ��䣩
	//void init();							//��ʼ����������
	void updateBestAnt(Ant&);				//����ȫ����������
	void run();								//�пغ���
	void infoDiss();						//��Ϣ˥��
	bool isFood(VIndex);					//�ж�ĳ�������Ƿ���ʳ�ﶥ��		



	bool cmpAnts(const Ant &, const Ant&);				//����һֻ���ϱȵڶ�ֻ��(<=)���򷵻�true������(>)������ false
	Ant& chooseBestAnt(bool);				//����������ϣ��β�����ѡ��ȫ����ѻ�ֲ���ѣ�����updataPath����ȫ���������
	void updateInfo(Ant&);					//���¼�¼��Ϣ��������������ϵ���·����Ϣ��

};


#endif
