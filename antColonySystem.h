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

typedef int		RIndex;		//真实顶点编号
typedef int		VIndex;		//转换后的顶点
typedef int		EIndex;		//边编号
typedef double	Weight;		//权重
typedef int		FIndex;		//食物编号
typedef double	Info;		//信息量
typedef double	AInfo;		//信息增量
typedef double	DisRate;	//消散系数
typedef int		AdjIndex;	//邻接顶点编号
typedef	int		Number;		//食物数量单位

class AdjEdg;
class Ant;
class AntColonySystem;

const Info init_info = 10000;

inline unordered_map<VIndex, RIndex>& getV2rTable()	//有效顶点与真实顶点的对照表
{
	static unordered_map<VIndex, RIndex> V2rTable;
	return V2rTable;
}
inline vector<vector<AdjEdg>>& getGraph()			//图
{
	static vector<vector<AdjEdg>> graph;
	return graph;
}
inline vector<vector<VIndex>>& getAdjGraph()		//邻接图，记录了每个顶点的邻接集合
{
	static vector<vector<VIndex>> adj_graph;
	return adj_graph;
}
inline Number& getFoodNum()		//食物数量
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

inline void setN(uint64_t& num, size_t digital)	//设置第digital位为1
{
	num |= ((uint64_t)1 << digital);
}

inline void clearN(uint64_t& num, size_t digital)	//设置第digital位为0
{
	num &= !((uint64_t)1 << digital);
}

double getPbest();

void loadFile(const string& graph_file, const string& key_file);

struct EInfo
{
	Info	_info;			//信息量
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
	EIndex _adj_e_index;	//邻接边的边编号
	Weight _adj_e_weight;	//邻接边的权重
	vector<EInfo>	_adj_e_ftable;	//邻接边的食物信息表
	EInfo	_adj_e_home;	//邻接边的家信息
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
	unordered_set < VIndex >	_forbiden_table;	//禁忌表  ，初始时含有终点
	uint64_t					_food_bag;			//根据下标存储某个食物是否已经获取
	int							_food_num;			//记录已经吃到的食物数量
	vector	<VIndex>			_path;				//当前蚂蚁经过的路径
	VIndex						_loc;				//蚂蚁当前所在位置
	AntColonySystem&			_ant_system;		//蚁群控制中心
	AntRandNum					_rand_fac;			//蚂蚁的随机因子

	Weight						_path_weight;		//当前蚂蚁权重
	bool						_survive;			//成功到达终点的蚂蚁为true
	bool						_mistake;			//是否犯过错

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
	void resetAnt();		//蚂蚁死后重置
	int moveNext();	//控制蚂蚁移动到下一个位置，包括触发蚁群局部信息的更新
	int checkAdj();		//检查邻接顶点是否有非禁忌的食物顶点, 若有食物顶点，返回>=0; 若无，返回-1
	void updateFoodBag(VIndex);	//走到新的食物顶点时更新食物包裹,若装满了，则将终点移出禁忌表
	int chooseWay();		//在周围非禁忌的邻接顶点找合适的顶点，有一定的失误率 。若找到了合适的邻接顶点 ,返回 >=0，若蚂蚁死亡 ,返回 < 0

	void dealCommonVIndex(VIndex);	//处理普通顶点，终点也属于普通顶点
	void dealFoodVIndex(VIndex);	//处理食物顶点

	//一些调整性能的接口函数
	Weight calcRealWeight(Weight,int);		//根据食物数量处理真正的权重

	void antRun();			//蚂蚁运动
	
	inline bool isForbidden(VIndex); //传入的邻接顶点是否是禁忌点
	bool isMistake();

	Ant(AntColonySystem & s) :_ant_system(s)
	{
		//静态随机数引擎
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
	void loadInfo(const string& f1, const string& f2);		//载入图信息，载入食物信息

public:

	DisRate		_frate;						//食物信息消散率
	DisRate		_hrate;						//家信息消散率
	size_t		_ants_num;					//蚂蚁数量
	vector<Ant> _ants;						//蚂蚁集群
	size_t		_steps_num;					//步数

	bool				_any_arrived;		//当前最优解是否为到达终点的最优解
	shared_ptr<Ant>		_best_ant;			//全局最佳蚂蚁
	double				_mrate;				//蚂蚁犯错概率

	static const Weight		_init_info;		//初始信息量

public:
	AntColonySystem(double frate, double hrate, double mis_rate, size_t ants_num, size_t steps_num)  //输入顺序：食物信息量消散率，家信息量消散率，蚂蚁犯错概率，蚂蚁数量，最大步数
		:_frate(frate), _hrate(hrate), _mrate(mis_rate)
		, _ants_num(ants_num), _steps_num(steps_num)
	{

		EInfo::Qmax = EInfo::Qmin = 0;
		double city_num = getGraph().size();
		double temp = std::exp(std::log(getPbest()) / city_num); //对Pbest开N_CITY_COUNT次方
		EInfo::ratio = (2.0 / temp - 2.0) / (city_num - 2.0);

		_ants.resize(_ants_num, *this);

		//初始化最佳蚂蚁
		_best_ant = shared_ptr<Ant>(new Ant(*this)); 
		_best_ant->_path_weight = INT_MAX;
		_best_ant->_survive = false;	//先初始化为食物蚂蚁
	}
	
	double calInfo(Ant&, AdjIndex);			//根据蚂蚁信息，计算该蚂蚁到邻接顶点的信息量总和
	void updateBestAnt(Ant&);				//更新全局最优蚂蚁
	void run();								//中控函数
	void infoDiss();						//信息衰减
	bool isFood(VIndex);					//判断某个顶点是否是食物顶点		


	bool cmpAnts(const Ant &, const Ant&);				//若第一只蚂蚁比第二只好(<=)，则返回true；否则(>)，返回 false
	Ant& chooseBestAnt(bool);				//挑出最佳蚂蚁，形参用来选择全局最佳或局部最佳，调用updataPath更新全局最佳蚂蚁
	void updateInfo(Ant&);					//更新记录信息量，根据最佳蚂蚁调整路径信息量


	//一些性能参数接口
	void adjustMisRate(double rate)
	{
		if (_best_ant->_survive)
			_mrate += rate;
	}

};


#endif
