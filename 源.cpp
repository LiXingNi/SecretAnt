#include"t1.h"
int main()
{
	AntColonySystem acs("topo.csv", "demand.csv", 0.5, 0.5, 0.3, 1000, 1000);
	acs.run();
	cout << "**********************************" << endl;
	for (int i(1); i != acs._best_path.size(); ++i)
	{
		int src = acs._best_path[i - 1];
		int dst = acs._best_path[i];
		std::cout << acs._graph[src][dst]._adj_e_index << ' ';
	}
	std::cout << std::endl;
	std::cout << acs._best_weight << std::endl;
	//system("pause");
	return 0;
}