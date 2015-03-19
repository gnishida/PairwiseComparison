/**
 * Pairwise comparisonsから、ユーザのpreferenceベクトルを計算する。
 *
 * 本テストプログラムの目的：
 * あるアイテムが複数の要素で構成されているとした時、ユーザがどのアイテムをより好むかは、
 * ユーザのpreferenceベクトルとアイテムのfeatureベクトルの内積によって決定できるという
 * モデルが、本当に妥当なのか、確認したい。
 * 
 * 確認方法：
 * discrete（各要素について、高・中・小）で生成した選択肢について、総当りでpairwise comparisonを行い、
 * その結果から、gradient descentでユーザのpreferenceベクトルを求める。
 * 上の仮説が合っていれば、得られたpreferenceベクトルとfeatureベクトルの内積が、
 * pairwise comparisonの結果とほぼ一致するはずだ。
 *
 * @authro	Gen Nishida
 * @date	3/19/2015
 * @version	1.0
 */

#include <vector>
#include "PairwiseComparison.h"

using namespace std;

int main() {
	// ランキングの答え
	vector<int> rankings(27);
	rankings[0] = 19;
	rankings[1] = 20;
	rankings[2] = 21;
	rankings[3] = 3;

	// featureベクトル
	vector<vector<float> > features(27, vector<float>(3));
	features[0][0] = 1.0f; features[0][1] = 1.0f; features[0][2] = 1.0f;
	features[1][0] = 1.0f; features[1][1] = 1.0f; features[1][2] = 0.5f;
	features[2][0] = 1.0f; features[2][1] = 1.0f; features[2][2] = 0.1f;
	features[3][0] = 1.0f; features[3][1] = 0.5f; features[3][2] = 1.0f;
	features[4][0] = 1.0f; features[4][1] = 0.5f; features[4][2] = 0.5f;
	features[5][0] = 1.0f; features[5][1] = 0.5f; features[2][2] = 0.1f;
	features[6][0] = 1.0f; features[6][1] = 0.1f; features[6][2] = 1.0f;
	features[7][0] = 1.0f; features[7][1] = 0.1f; features[7][2] = 0.5f;
	features[8][0] = 1.0f; features[8][1] = 0.1f; features[8][2] = 0.1f;

	// 総当りのpairwise comparisonsを作成する
	float val[3] = {1.0f, 0.5f, 0.1f};
	vector<pair<int, pair<vector<float>, vector<float> > > > comparisons;
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			for (int k = 0; k < 3; ++k) {
				int index = i * 9 + j * 3 + k;
				vector<float> feature1(3);
				feature1[0] = val[i];
				feature1[1] = val[j];
				feature1[2] = val[k];

				for (int i2 = 0; i2 < 3; ++i2) {
					for (int j2 = 0; j2 < 3; ++j2) {
						for (int k2 = 0; k2 < 3; ++k2) {
							int index2 = i2 * 9 + j2 * 3 + k2;
							vector<float> feature2(3);
							feature2[0] = val[i2];
							feature2[1] = val[j2];
							feature2[2] = val[k2];

							int choice = rankings[index] < rankings[index2] ? 1 : 0;
							comparisons.push_back(make_pair(choice, make_pair(feature1, feature2)));
						}
					}
				}
			}
		}
	}

	vector<float> preferences = PairwiseComparison::computePreferences(comparisons);

	return 0;
}
