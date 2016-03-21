#include"antColonySystem.h"
#include<ctime>
#include<exception>
#include<fstream>
#include<cmath>
#include<iostream>
using std::fstream;

int calcWeight(AntColonySystem & acs)
{
	shared_ptr<Ant> ant_ptr = acs._best_ant;
	auto & graph = getGraph();
	int weight = 0;
	
	for (int i = 1; i != ant_ptr->_path.size(); ++i)
	{
		weight += graph[ant_ptr->_path[i - 1]][ant_ptr->_path[i]]._adj_e_weight;
	}
	return weight;
}

int main(int argc, char** argv)
{
	loadFile(argv[1], argv[2]);
	int node_num = getGraph().size();
	node_num = node_num / 2;
	node_num = std::max(node_num, 1);
	node_num = std::min(node_num, 100);
	AntColonySystem *acs;
	if(node_num <= 50)
	{
		acs = new AntColonySystem(0.95, 0.95, 0.2, node_num, 500);
	}
	else
	{
		acs = new AntColonySystem(0.95, 0.95, 0.2, node_num, 1500);
	}
	acs->run();
	auto& graph = getGraph();

	auto& output = std::cout;

	if (!acs->_best_ant->_survive)
		output << "NA" ;
	else{
		output << graph[acs->_best_ant->_path[0]][acs->_best_ant->_path[1]]._adj_e_index;
		for (int i(2); i < acs->_best_ant->_path.size(); ++i)
		{
			int src = acs->_best_ant->_path[i - 1];
			int dst = acs->_best_ant->_path[i];
			output << '|' << graph[src][dst]._adj_e_index;
		}
	}
	output << std::endl << "*****************************" << std::endl << "The weight is: " << calcWeight(*acs);
	output << std::endl;
	system("pause");
	return 0;
}
