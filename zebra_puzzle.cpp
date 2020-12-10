//BuDDy 2.4 {https://sourceforge.net/projects/buddy/}
#pragma comment(lib, "bdd.lib")
#include "bdd.h"

#include <iostream>
#include <cmath>
#include <vector>

namespace values {
	const std::size_t	       N = 9;						//objects count
	const std::size_t	       M = 4;						//properies count
	const int			   LOG_N = std::ceil(std::log2(N));	//bool fluents count
	const std::size_t	   N_VAR = M * N * LOG_N;			//all fluents count
	const std::size_t		   K = 30;						//nessesary const for 7th Constraint
	const std::size_t ROW_LENGTH = 3;						//for work with arr[][] like arr[]
}

namespace entities {
	struct Object_t
	{
		Object_t(int propNum, int objNum, int propVal) :
			propNum_(propNum),
			objNum_(objNum),
			propVal_(propVal)
		{ }

		Object_t(std::size_t propNum, int propVal) :
			propNum_(propNum),
			propVal_(propVal)
		{
			objNum_ = -1;
		}

		int propNum_;
		int objNum_;
		int propVal_;
	};

	enum class Neighbour
	{
		NEIGHBOUR_HIGH_RIGHT,	// |   | * |
		NEIGHBOUR_RIGHT			// | O | * | 
	};
}

namespace constraints {
	void useConstraintType1(bdd&, const bdd [values::M][values::N][values::N], const entities::Object_t);
	void useConstraintType2(bdd &, const bdd [values::M][values::N][values::N], const entities::Object_t, const entities::Object_t);
	bdd useConstraintType3(const bdd [values::M][values::N][values::N], const entities::Neighbour, const entities::Object_t, entities::Object_t);
	void useConstraintType4(bdd&, const bdd [values::M][values::N][values::N], const entities::Object_t, const entities::Object_t);
	void useConstraintType5(bdd&, const bdd [values::M][values::N][values::N]);
	void useConstraintType6(bdd&, const bdd [values::M][values::N][values::N]);
	void useConstraintType7(bdd& , const bdd [values::M][values::N][values::N]);
}

namespace utils {
	void initPFunction(bdd [values::M][values::N][values::N]);
	void print();
	void build(char*, unsigned, unsigned);
	void fun(char*, int);

	std::vector<char> var;
}

void workWithBdd();

int main(int argc, char* argv[])
{
	workWithBdd();

	return 0;
}

void workWithBdd()
{
	//---------bdd base initialization
	bdd_init(10000000, 1000000);
	bdd_setvarnum(values::N_VAR);
	bdd p[values::M][values::N][values::N]; //P fubction
	utils::initPFunction(p);
	bdd tree = bddtrue; //it's bdd in tree form
	//---------

	//using constraints
	constraints::useConstraintType6(tree, p);
	
	constraints::useConstraintType1(tree, p, entities::Object_t(0, 8, 7));
	constraints::useConstraintType1(tree, p, entities::Object_t(3, 2, 6));
	constraints::useConstraintType1(tree, p, entities::Object_t(1, 4, 2));
	constraints::useConstraintType1(tree, p, entities::Object_t(2, 0, 3));
	constraints::useConstraintType1(tree, p, entities::Object_t(0, 7, 2));

	//extra 
	constraints::useConstraintType1(tree, p, entities::Object_t(0, 2, 6));
	constraints::useConstraintType1(tree, p, entities::Object_t(1, 2, 6));
	constraints::useConstraintType1(tree, p, entities::Object_t(2, 3, 2));
	constraints::useConstraintType1(tree, p, entities::Object_t(0, 4, 1));
	//constraints::useConstraintType1(tree, p, entities::Object_t(2, 4, 7));
	constraints::useConstraintType1(tree, p, entities::Object_t(1, 8, 5));
	constraints::useConstraintType1(tree, p, entities::Object_t(3, 6, 3));
	constraints::useConstraintType1(tree, p, entities::Object_t(3, 8, 4));

	constraints::useConstraintType2(tree, p, entities::Object_t(2, 4), entities::Object_t(3, 5));
	constraints::useConstraintType2(tree, p, entities::Object_t(0, 5), entities::Object_t(1, 3));
	constraints::useConstraintType2(tree, p, entities::Object_t(2, 8), entities::Object_t(1, 1));
	constraints::useConstraintType2(tree, p, entities::Object_t(3, 8), entities::Object_t(0, 8));
	constraints::useConstraintType2(tree, p, entities::Object_t(0, 3), entities::Object_t(2, 6));

	//extra
	constraints::useConstraintType2(tree, p, entities::Object_t(3, 0), entities::Object_t(2, 7));

	tree &= constraints::useConstraintType3(p, entities::Neighbour::NEIGHBOUR_RIGHT, entities::Object_t(3, 7), entities::Object_t(3, 2));
	tree &= constraints::useConstraintType3(p, entities::Neighbour::NEIGHBOUR_HIGH_RIGHT, entities::Object_t(1, 0), entities::Object_t(0, 0));
	tree &= constraints::useConstraintType3(p, entities::Neighbour::NEIGHBOUR_RIGHT, entities::Object_t(1, 8), entities::Object_t(3, 1));
	
	tree &= constraints::useConstraintType3(p, entities::Neighbour::NEIGHBOUR_HIGH_RIGHT, entities::Object_t(2, 1), entities::Object_t(1, 7));

	//super-extra
	tree &= constraints::useConstraintType3(p, entities::Neighbour::NEIGHBOUR_RIGHT, entities::Object_t(2, 1), entities::Object_t(0, 7));

	constraints::useConstraintType4(tree, p, entities::Object_t(3, 0), entities::Object_t(0, 4));
	constraints::useConstraintType4(tree, p, entities::Object_t(1, 4), entities::Object_t(2, 5));
	
	//extra
	constraints::useConstraintType4(tree, p, entities::Object_t(3, 8), entities::Object_t(1, 2));
	constraints::useConstraintType4(tree, p, entities::Object_t(1, 1), entities::Object_t(0, 6));

	constraints::useConstraintType5(tree, p);
	//constraints::useConstraintType7(tree, p);

	std::cout << bdd_satcount(tree) << "\n";
	bdd_allsat(tree, utils::fun);
	system("pause");

	bdd_done();
}

//---------------CONSTRAINTS---------------

void constraints::useConstraintType1(bdd& mainTree, const bdd p[values::M][values::N][values::N], const entities::Object_t object)
{
	mainTree &= p[object.propNum_][object.objNum_][object.propVal_];
}

void constraints::useConstraintType2(bdd& mainTree, const bdd p[values::M][values::N][values::N], const entities::Object_t object1, const entities::Object_t object2)
{
	for (int i = 0; i < values::N; ++i) {
		mainTree &= !(p[object1.propNum_][i][object1.propVal_] 
					^ p[object2.propNum_][i][object2.propVal_]);
	}
}

bdd constraints::useConstraintType3(const bdd p[values::M][values::N][values::N], const entities::Neighbour neighbour, const entities::Object_t objectGeneral, entities::Object_t objectNeighbour)
{
	bdd tmpTree = bddtrue;

	switch (neighbour) {
		case entities::Neighbour::NEIGHBOUR_HIGH_RIGHT: {
			for (int i = 0; i < values::N; ++i) {
				if ((i > values::ROW_LENGTH - 1) && ((i + 1) % values::ROW_LENGTH != 0)) {
					tmpTree &= !(p[objectGeneral.propNum_][i][objectGeneral.propVal_] 
								^ p[objectNeighbour.propNum_][i - 2 * (int)(values::ROW_LENGTH / 2)][objectNeighbour.propVal_]);
				}
				else {
					tmpTree &= !p[objectGeneral.propNum_][i][objectGeneral.propVal_];
				}
			}
			break;
		}
		case entities::Neighbour::NEIGHBOUR_RIGHT: {
			for (int i = 0; i < values::N - 1; ++i) {
				if ((i + 1) % values::ROW_LENGTH != 0) {
					tmpTree &= !(p[objectGeneral.propNum_][i][objectGeneral.propVal_] 
								^ p[objectNeighbour.propNum_][i + 1][objectNeighbour.propVal_]);
				}
				else {
					tmpTree &= !p[objectGeneral.propNum_][i][objectGeneral.propVal_];
				}
			}
			break;
		}
	}

	return tmpTree;
}

void constraints::useConstraintType4(bdd& mainTree, const bdd p[values::M][values::N][values::N], const entities::Object_t objectGeneral, const entities::Object_t objectNeighbour)
{
	bdd tmpTree = bddfalse;

	std::vector<entities::Neighbour> vecNeighbours;
	vecNeighbours.push_back(entities::Neighbour::NEIGHBOUR_HIGH_RIGHT);
	vecNeighbours.push_back(entities::Neighbour::NEIGHBOUR_RIGHT);

	for (entities::Neighbour neighbour : vecNeighbours) {
		tmpTree |= useConstraintType3(p, neighbour, objectGeneral, objectNeighbour);
	}

	mainTree &= tmpTree;
}

void constraints::useConstraintType5(bdd& mainTree, const bdd p[values::M][values::N][values::N])
{
	for (int m = 0; m < values::M; ++m) {
		for (int i = 0; i < values::N - 1; ++i) {
			for (int j = i + 1; j < values::N; ++j) {
				for (int k = 0; k < values::N; ++k) {
					mainTree &= p[m][i][k] >> !p[m][j][k];
				}
			}
		}
	}
}

void constraints::useConstraintType6(bdd& mainTree, const bdd p[values::M][values::N][values::N])
{
	for (int i = 0; i < values::N; ++i) {
		bdd arrProps[values::M] = {bddfalse};

		for (int j = 0; j < values::N; j++) {
			for (int m = 0; m < values::M; m++) {
				arrProps[m] |= p[m][i][j];
			}
		}

		for (int m = 0; m < values::M; m++) {
			mainTree &= arrProps[m];
		}
	}
}

void constraints::useConstraintType7(bdd& mainTree, const bdd p[values::M][values::N][values::N])
{
	for (int i = 0; i < values::N; i++)
	{
		if ((i + 1) % values::ROW_LENGTH == 0) { //without neighbours
			continue;
		}

		bdd tmpTreeRightNeighbour = bddfalse;
		bdd tmpTreeHighRightNeighbour = bddfalse;

		bool hasHighRightNeighbour = false;

		for (int j0 = 0; j0 < values::N; ++j0) {
			for (int j1 = 0; j1 < values::N; ++j1) {
				for (int j2 = 0; j2 < values::N; ++j2) {
					for (int j3 = 0; j3 < values::N; ++j3) {
						for (int k0 = 0; k0 < values::N; ++k0) {
							for (int k1 = 0; k1 < values::N; ++k1) {
								for (int k2 = 0; k2 < values::N; ++k2) {
									for (int k3 = 0; k3 < values::N; ++k3) {

										if (j0 + j1 + j2 + j3 + k0 + k1 + k2 + k3 <= values::K) {
											tmpTreeRightNeighbour |= (p[0][i][j0] & p[1][i][j1] & p[2][i][j2] & p[3][i][j3] //right neighbour
														& p[0][i + 1][k0] & p[1][i + 1][k1] & p[2][i + 1][k2] & p[3][i + 1][k3]);
											
											if (i > (values::ROW_LENGTH - 1)) {
												tmpTreeHighRightNeighbour |= (p[0][i][j0] & p[1][i][j1] & p[2][i][j2] & p[3][i][j3] // high-right neighbour
															& p[0][i - 2 * (int)(values::ROW_LENGTH / 2)][k0] 
															& p[1][i - 2 * (int)(values::ROW_LENGTH / 2)][k1] 
															& p[2][i - 2 * (int)(values::ROW_LENGTH / 2)][k2] 
															& p[3][i - 2 * (int)(values::ROW_LENGTH / 2)][k3]);
												if (!hasHighRightNeighbour) 
													hasHighRightNeighbour = true;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}

		mainTree &= tmpTreeRightNeighbour;

		if (hasHighRightNeighbour) {
			mainTree &= tmpTreeHighRightNeighbour;
		}
	}
}

//---------------UTILS---------------

void utils::initPFunction(bdd p[values::M][values::N][values::N])
{
	utils::var.resize(values::N_VAR);

	for (int i = 0; i < values::M; ++i) {
		for (int j = 0; j < values::N; ++j) {
			for (int k = 0; k < values::N; ++k) {
				p[i][j][k] = bddtrue;

				for (int l = 0; l < values::LOG_N; ++l) {
					p[i][j][k] &= ((k >> l) & 1) ? bdd_ithvar(j * values::LOG_N * values::M + l + values::LOG_N * i) :
						bdd_nithvar(j * values::LOG_N * values::M + l + values::LOG_N * i);
				}
			}
		}
	}
}

void utils::print()
{
	for (unsigned i = 0; i < values::N; ++i) {
		std::cout << i + 1 << ": ";
		for (unsigned j = 0; j < values::M; ++j) {
			int J = i * values::M * values::LOG_N + j * values::LOG_N;
			int num = 0;
			for (unsigned k = 0; k < values::LOG_N; ++k) num += (unsigned)(utils::var[J + k] << k);
			std::cout << num + 1 << ' ';
		}
		std::cout << "\n";
	}
	std::cout << "\n";
}

void utils::build(char* varset, unsigned n, unsigned I)
{
	if (I == n - 1) {
		if (varset[I] >= 0) {
			utils::var[I] = varset[I];
			print();
			
			return;
		}
		utils::var[I] = 0;
		print();
		utils::var[I] = 1;
		print();
		
		return;
	}
	if (varset[I] >= 0) {
		utils::var[I] = varset[I];
		build(varset, n, I + 1);
		
		return;
	}

	utils::var[I] = 0;
	build(varset, n, I + 1);
	
	utils::var[I] = 1;
	build(varset, n, I + 1);
}

void utils::fun(char* varset, int size)
{
	build(varset, size, 0);
}
