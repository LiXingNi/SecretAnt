#include"t1.h"
#include<ctime>
#include<exception>

int calcWeight(AntColonySystem & acs)
{
	shared_ptr<AntColonySystem::Ant> ant_ptr = acs._best_ant;
	int weight = 0;
	
	for (int i = 1; i != ant_ptr->_path.size(); ++i)
	{
		weight += acs._graph[ant_ptr->_path[i - 1]][ant_ptr->_path[i]]._adj_e_weight;
	}
	return weight;
}

int main()
{
	AntColonySystem acs("topo.csv", "demand.csv", 0.90, 0.90, 0.2, 300, 1000);
	acs.run();
	cout << "**********************************" << endl;
	if (!acs._best_ant->_survive)
		throw std::runtime_error("ËÀÁË");
	for (int i(1); i != acs._best_ant->_path.size(); ++i)
	{
		int src = acs._best_ant->_path[i - 1];
		int dst = acs._best_ant->_path[i];
		std::cout << acs._graph[src][dst]._adj_e_index << ' ';
	}
	cout << endl;
	std::cout << calcWeight(acs) << endl;
	std::cout << std::endl;
	system("pause");
	return 0;
}