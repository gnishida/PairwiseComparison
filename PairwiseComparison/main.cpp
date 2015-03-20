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
 * 結果：
 * [0.571667, -0.808937, 0.137177]
 *
 * @authro	Gen Nishida
 * @date	3/19/2015
 * @version	1.0
 */

#include <iostream>
#include <vector>
#include <map>
#include "PairwiseComparison.h"

using namespace std;

int main() {
	// ランキングの答え
	vector<int> rankings(27);
	rankings[0] = 19;
	rankings[1] = 20;
	rankings[2] = 21;
	rankings[3] = 3;
	rankings[4] = 4;
	rankings[5] = 6;
	rankings[6] = 1;
	rankings[7] = 2;
	rankings[8] = 5;
	rankings[9] = 22;
	rankings[10] = 23;
	rankings[11] = 24;
	rankings[12] = 10;
	rankings[13] = 11;
	rankings[14] = 12;
	rankings[15] = 7;
	rankings[16] = 8;
	rankings[17] = 9;
	rankings[18] = 25;
	rankings[19] = 26;
	rankings[20] = 27;
	rankings[21] = 16;
	rankings[22] = 17;
	rankings[23] = 18;
	rankings[24] = 13;
	rankings[25] = 14;
	rankings[26] = 15;

	// 総当りのpairwise comparisonsを作成する
	float val[3] = {1.0f, 0.5f, 0.0f};
	vector<pair<int, pair<vector<float>, vector<float> > > > comparisons;
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			for (int k = 0; k < 3; ++k) {
				int index1 = i * 9 + j * 3 + k;
				vector<float> feature1(3);
				feature1[0] = val[i];
				feature1[1] = val[j];
				feature1[2] = val[k];

				for (int i2 = 0; i2 < 3; ++i2) {
					for (int j2 = 0; j2 < 3; ++j2) {
						for (int k2 = 0; k2 < 3; ++k2) {
							int index2 = i2 * 9 + j2 * 3 + k2;
							if (index1 >= index2) continue;

							vector<float> feature2(3);
							feature2[0] = val[i2];
							feature2[1] = val[j2];
							feature2[2] = val[k2];

							int choice = rankings[index1] < rankings[index2] ? 1 : 0;
							comparisons.push_back(make_pair(choice, make_pair(feature1, feature2)));
						}
					}
				}
			}
		}
	}

	// preferenceベクトルを推定する
	vector<float> preferences = PairwiseComparison::computePreferences(comparisons, 10000, false, 0.1, 0.002, 0.00001);
	cout << "Preference: " << endl;
	for (int i = 0; i < preferences.size(); ++i) {
		cout << preferences[i] << ",";
	}
	cout << endl;

	// ランキングの答え合わせ
	vector<float> scores(27);
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			for (int k = 0; k < 3; ++k) {
				int index = i * 9 + j * 3 + k;
				vector<float> feature1(3);
				feature1[0] = val[i];
				feature1[1] = val[j];
				feature1[2] = val[k];
				float score = PairwiseComparison::dot(preferences, feature1);

				scores[rankings[index] - 1] = score;
			}
		}
	}

	// 結果をファイルに書き込む
	FILE* fp = fopen("scores.txt", "w");
	for (int i = 0; i < 27; ++i) {
		fprintf(fp, "%lf\n", scores[i]);
	}
	fclose(fp);

	return 0;
}
