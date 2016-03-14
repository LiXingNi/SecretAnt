#ifndef NOTE_HELP_T1_H_
#define NOTE_HELP_T1_H_
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <algorithm>
#include <functional>
#include <random>
#include <set>

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

struct Err
{
	string msg;
	Err(const string & m) :msg(m){}
};


class AntColonySystem
{
	typedef int		VIndex;		//顶点编号
	typedef int		EIndex;		//边编号
	typedef int		Weight;		//权重
	typedef int		FIndex;		//食物编号
	typedef double	Info;		//信息量
	typedef double	AInfo;		//信息增量
	typedef double	DisRate;	//消散系数
	typedef int		AdjIndex;	//邻接顶点编号

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
		unordered_set < VIndex >	_forbiden_table;	//禁忌表  ，初始时含有终点
		set < VIndex >	_food_bag;			//当前已经获取的食物编号
		vector	<VIndex>			_path;				//当前蚂蚁经过的路径
		VIndex						_loc;				//蚂蚁当前所在位置
		AntColonySystem&			_ant_system;		//蚁群控制中心
		AntRandNum					_rand_fac;			//蚂蚁的随机因子
		static default_random_engine e;
		static uniform_int_distribution<int> u; 
		static uniform_int_distribution<int> u_mistake; 

	public:
		void resetAnt();		//蚂蚁死后重置
		void moveNext();	//控制蚂蚁移动到下一个位置，包括触发蚁群局部信息的更新
		int checkAdj();		//检查邻接顶点是否有非禁忌的食物顶点, 若有食物顶点，返回>=0; 若无，返回-1
		void updateFoodBag(VIndex);	//走到新的食物顶点时更新食物包裹,若装满了，则将终点移出禁忌表
		int chooseWay();		//在周围非禁忌的邻接顶点找合适的顶点，有一定的失误率 。若找到了合适的邻接顶点 ,返回 >=0，若蚂蚁死亡 ,返回 < 0
		void terminate();		//蚂蚁移动结束
		
		inline bool isForbidden(VIndex); //传入的邻接顶点是否是禁忌点
		bool isMistake();

		Ant(AntColonySystem & s) :_ant_system(s)
		{
			//静态随机数引擎
			resetAnt();
		}
	};

public:

	struct EInfo
	{
		Info	_info;			//信息量
		AInfo	_ainfo;			//信息增量
		static const Info _max_info;
		EInfo(AInfo ainfo = 0, Info info = 0.0) :_ainfo(ainfo), _info(info){}
		void addInfo()
		{
			Info n_info = _info + _ainfo;
			_info = n_info > _max_info ? _max_info : n_info;
		}
	};

	struct AdjEdg
	{
		EIndex _adj_e_index;	//邻接边的边编号
		Weight _adj_e_weight;	//邻接边的权重
		unordered_map<FIndex, EInfo>	_adj_e_ftable;	//邻接边的食物信息表
		EInfo	_adj_e_home;	//邻接边的家信息

		AdjEdg(EIndex index = 0, Weight weight = 0) :_adj_e_index(index), _adj_e_weight(weight){ _adj_e_home._ainfo = 1.0 / _adj_e_weight; }
	};

private:
	void loadInfo(const string& f1, const string& f2);		//载入图信息，载入食物信息

public:
	typedef unordered_map<AdjIndex, AdjEdg> AdjSet;
	
	unordered_map<VIndex, AdjSet> _graph;	//图
	unordered_set<VIndex> _food;			//必过顶点
	DisRate		_frate;						//食物信息消散率
	DisRate		_hrate;						//家信息消散率
	size_t		_ants_num;					//蚂蚁数量
	vector<Ant> _ants;						//蚂蚁集群
	size_t		_steps_num;					//步数
	VIndex		_src;						//起点顶点号
	VIndex		_dest;						//终点顶点号
	vector<VIndex>		_best_path;			//最优的路径
	Weight				_best_weight;		//最优路径对应的权重
	double		_mrate;						//蚂蚁犯错概率


public:
	AntColonySystem(const string& f1, const string & f2, double frate, double hrate, double mis_rate, size_t ants_num, size_t steps_num)  //输入顺序：食物信息量消散率，家信息量消散率，蚂蚁犯错概率，蚂蚁数量，最大步数
		:_frate(frate), _hrate(hrate), _mrate(mis_rate)
		, _ants_num(ants_num), _steps_num(steps_num)
	{
		loadInfo(f1, f2);
		_ants.resize(_ants_num, *this);
		_best_weight = INT_MAX;
	}
	
	double calInfo(Ant&, AdjIndex);			//根据蚂蚁信息，计算该蚂蚁到邻接顶点的信息量总和
	void localUpdate(Ant&, AdjIndex);		//根据蚂蚁选择的顶点，更新该边的信息量
	void activeFood(Ant&, FIndex);			//找到食物的蚂蚁，激活所经过路径上的食物增量
	//void init();							//初始化各项增量
	void updataPath(vector<VIndex> &);		//计算给定路径上的权重和
	void run();								//中控函数
	void infoDiss();						//信息衰减

};


#endif
